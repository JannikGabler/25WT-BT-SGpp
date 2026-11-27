// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <omp.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <mutex>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_engine.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_state.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_decision_tools.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_index_set_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::tools::numerics::CompensatedSum;
namespace refinement_tools = sgpp::combigrid::tools::refinement;

namespace sgpp {
namespace combigrid {
namespace misc {

RefinementEngine::RefinementEngine(RefinementState& state, RefinementNodeStorage& nodes,
                                   RefinementTarget& target, const SparseGridSetup& setup,
                                   const RefinementOptions& options, const size_t maxDecisions)
    : state_(state),
      nodes_(nodes),
      target_(target),
      setup_(setup),
      options_(options),
      maxDecisions_(maxDecisions) {}

RefinementStatistics RefinementEngine::run(const size_t nWorkers) {
  nWorkers_ = std::max<size_t>(nWorkers, 1);
  start_ = std::chrono::steady_clock::now();
  workerLevels_.assign(nWorkers_, LvlMI(setup_.nDim()));
  workerTasks_.assign(nWorkers_, 0);
  workerValues_.assign(nWorkers_, std::vector<double>(state_.nValues));
  workerIdleSeconds_.assign(nWorkers_, 0.0);
  hasTask_.assign(state_.states.size(), 0);
  nSpeculative_ =
      static_cast<size_t>(std::ranges::count(state_.states, RefinementIndexState::Speculative));

  scheduleInitialTasks();

#pragma omp parallel num_threads(static_cast<int>(nWorkers_))
  work(static_cast<size_t>(omp_get_thread_num()));

  fillStatistics();
  if (error_) {
    std::rethrow_exception(error_);
  }
  return statistics_;
}

/*******
Workers
*******/

/**
 * Every worker runs the management first (whoever holds the lock and finds something to do does
 * it), then takes a task or waits for one.
 */
void RefinementEngine::work(const size_t worker) {
  std::unique_lock<std::mutex> lock(mutex_);

  while (true) {
    manageSafely();
    if (isFinished()) {
      workAvailable_.notify_all();
      return;
    }
    if (!takeTask(worker)) {
      waitForWork(lock, worker);
      continue;
    }

    lock.unlock();
    const std::exception_ptr error = evaluateTask(worker);
    lock.lock();
    running_--;
    storeResult(worker, error);
  }
}

/// Runs the management; an exception must not leave the parallel region (std::terminate), so it
/// stops the call instead and is rethrown after the region.
void RefinementEngine::manageSafely() {
  try {
    manage();
  } catch (...) {
    if (!error_) error_ = std::current_exception();
    stop(RefinementStopReason::None);
  }
}

/// Takes the next task (needed ones first) for @p worker; false if there is none.
bool RefinementEngine::takeTask(const size_t worker) {
  std::vector<Task>& tasks = queue_.empty() ? speculativeQueue_ : queue_;
  if (tasks.empty()) {
    return false;
  }
  std::ranges::pop_heap(tasks, isCheaper);
  const size_t idx = tasks.back().second;
  tasks.pop_back();
  running_++;
  workerTasks_[worker] = idx;
  const LvlMIView levels = state_.indexSet.levels(idx);
  std::copy(levels.data(), levels.data() + levels.size(), workerLevels_[worker].begin());
  return true;
}

/**
 * Evaluates the task of @p worker (called without the lock; the node rows of its levels were
 * generated before the task was queued and do not move).
 */
std::exception_ptr RefinementEngine::evaluateTask(const size_t worker) {
  try {
    const TensorGridView domainTG(workerLevels_[worker], nodes_.domainTable());
    const TensorGridView unitTG(workerLevels_[worker], nodes_.unitTable());
    target_.estimate(domainTG, unitTG, worker, workerValues_[worker]);
  } catch (...) {
    return std::current_exception();
  }
  return nullptr;
}

/// Stores the result of the task of @p worker (with the lock held); an error stops the call.
void RefinementEngine::storeResult(const size_t worker, const std::exception_ptr& error) {
  const size_t idx = workerTasks_[worker];
  if (error) {
    if (!error_) error_ = error;
    queue_.clear();
    speculativeQueue_.clear();
    return;
  }
  std::ranges::copy(workerValues_[worker], state_.valuesOf(idx).begin());
  state_.evaluated[idx] = 1;
  statistics_.nEvaluatedTensorGrids++;
  statistics_.nEvaluatedGridPoints += state_.costs[idx];
  if (state_.states[idx] == RefinementIndexState::Speculative) {
    speculativeEvaluated_.push_back(idx);  // kept until a decision requests it
    return;
  }
  nNeeded_--;
  if (state_.states[idx] == RefinementIndexState::Pending) {
    completed_.push_back(idx);
  }
}

void RefinementEngine::waitForWork(std::unique_lock<std::mutex>& lock, const size_t worker) {
  const auto begin = std::chrono::steady_clock::now();
  workAvailable_.wait(lock);
  workerIdleSeconds_[worker] +=
      std::chrono::duration<double>(std::chrono::steady_clock::now() - begin).count();
}

bool RefinementEngine::isFinished() const noexcept {
  return (stopReason_ != RefinementStopReason::None || error_) && running_ == 0;
}

/**********
Management
**********/

/**
 * Commits and decides as far as the evaluations allow. The initial phase (the grid's multi-indices
 * without values) ends when all of its tasks are evaluated; a stop during it commits nothing (the
 * values are kept for a later call).
 */
void RefinementEngine::manage() {
  const bool stopped = stopReason_ != RefinementStopReason::None || error_;
  if (!stopped && options_.maxSeconds <= elapsedSeconds()) {
    stop(RefinementStopReason::TimeLimit);
  }
  if (initialPhase_) {
    if (nNeeded_ > 0 || stopReason_ != RefinementStopReason::None || error_) {
      return;
    }
    finishInitialPhase();
  }
  if (stopReason_ != RefinementStopReason::None || error_) {
    commitCompleted();  // started tasks are admissible candidates: they join the grid
    return;
  }
  decideWhileNeeded();
}

/**
 * Synchronized: commit and decide only when the previous decision's tasks are all evaluated; a
 * decision without new tasks is followed by the next one at once. Asynchronous: commit what has
 * arrived and decide while fewer than two tasks per worker are queued or running.
 */
void RefinementEngine::decideWhileNeeded() {
  const bool synchronized = options_.schedule == RefinementSchedule::Synchronized;

  while (true) {
    if (synchronized && nNeeded_ > 0) return;
    commitCompleted();
    if (checkStop() || !needsDecision()) return;

    const size_t nQueued = queue_.size();
    const size_t nCandidates = candidates_.size();
    decide();
    if (synchronized && stopReason_ == RefinementStopReason::None) speculate();
    workAvailable_.notify_all();
    if (stopReason_ != RefinementStopReason::None) return;
    if (synchronized && nNeeded_ > 0) return;
    if (!synchronized && queue_.size() == nQueued && candidates_.size() == nCandidates) return;
  }
}

void RefinementEngine::scheduleInitialTasks() {
  for (size_t idx = 0; idx < state_.states.size(); idx++) {
    const bool pending = state_.states[idx] == RefinementIndexState::Pending;
    if (state_.isInGrid(idx) || pending) nInGridOrPending_++;
    if ((state_.isInGrid(idx) || pending) && !state_.evaluated[idx]) {
      reservedCost_ += state_.costs[idx];
      schedule(idx);
    } else if (pending) {
      completed_.push_back(idx);  // evaluated in an earlier call, not committed yet
    }
  }
  if (reservedCost_ > options_.maxGridPoints) {
    stop(RefinementStopReason::GridPointBudget);  // the grid cannot even be evaluated
  }
}

/**
 * The grid's multi-indices all have values now: the estimate, the gains of the active ones
 * (those not computed in an earlier call) and the candidate heap with the current indicator.
 */
void RefinementEngine::finishInitialPhase() {
  initialPhase_ = false;
  computeRunningEstimate();

  candidates_.clear();
  for (size_t idx = 0; idx < state_.states.size(); idx++) {
    if (state_.states[idx] != RefinementIndexState::Active) continue;
    if (!(state_.gains[idx] >= 0.0)) {
      refinement_tools::computeIncrement(state_, idx, sums_, increment_, support_);
      state_.gains[idx] = refinement_tools::gainOf(increment_, target_, finished_);
    }
    candidates_.emplace_back(priority(idx), idx);
  }
  std::ranges::make_heap(candidates_, hasLowerPriority);
}

/// The combination @f$\sum_{\vec{\ell}} c_{\vec{\ell}} Q_{\vec{\ell}}@f$ of the grid, in index
/// order.
void RefinementEngine::computeRunningEstimate() {
  runningEstimate_.assign(state_.nValues, CompensatedSum<double>());
  for (size_t idx = 0; idx < state_.states.size(); idx++) {
    const CTCoeffType coefficient = state_.coefficients[idx];
    if (!state_.isInGrid(idx) || coefficient == 0) continue;
    const std::span<const double> values = state_.valuesOf(idx);
    for (size_t i = 0; i < state_.nValues; i++) {
      runningEstimate_[i].addProduct(static_cast<double>(coefficient), values[i]);
    }
  }
}

/// Commits the evaluated pending multi-indices in index order (deterministic when synchronized).
void RefinementEngine::commitCompleted() {
  if (initialPhase_ || completed_.empty()) return;
  std::ranges::sort(completed_);
  for (const size_t idx : completed_) {
    if (state_.states[idx] == RefinementIndexState::Pending) commit(idx);
  }
  completed_.clear();
}

/// Adds the evaluated pending multi-index @p idx to the grid as an active one.
void RefinementEngine::commit(const size_t idx) {
  state_.states[idx] = RefinementIndexState::Active;
  refinement_tools::addToCombinationCoefficients(state_.indexSet, idx, state_.coefficients,
                                                 support_);
  refinement_tools::computeIncrement(state_, idx, sums_, increment_, support_);
  state_.gains[idx] = refinement_tools::gainOf(increment_, target_, finished_);
  for (size_t i = 0; i < state_.nValues; i++) {
    runningEstimate_[i].add(increment_[i]);
  }
  candidates_.emplace_back(priority(idx), idx);
  std::ranges::push_heap(candidates_, hasLowerPriority);
}

/// Checks the tolerances and the decision limit; stops if one holds.
bool RefinementEngine::checkStop() {
  if (nDecisions_ >= maxDecisions_) {
    stop(RefinementStopReason::StepDone);
    return true;
  }
  if (options_.absoluteTolerance <= 0.0 && options_.relativeTolerance <= 0.0) return false;

  const double errorEstimate = refinement_tools::errorEstimateOf(state_);
  if (options_.absoluteTolerance > 0.0 && errorEstimate <= options_.absoluteTolerance) {
    stop(RefinementStopReason::Tolerance);
    return true;
  }
  if (options_.relativeTolerance <= 0.0) return false;
  finished_.resize(state_.nValues);
  for (size_t i = 0; i < state_.nValues; i++) finished_[i] = runningEstimate_[i].value();
  target_.finishCombination(finished_);
  double norm = 0.0;
  for (const double value : finished_) norm = std::max(norm, std::abs(value));
  if (errorEstimate <= options_.relativeTolerance * norm) {
    stop(RefinementStopReason::RelativeTolerance);
    return true;
  }
  return false;
}

bool RefinementEngine::needsDecision() const noexcept {
  if (options_.schedule == RefinementSchedule::Synchronized) {
    return nNeeded_ == 0;
  }
  return queue_.size() + running_ < 2 * nWorkers_;
}

/**
 * Accepts the active multi-indices with priority at least batchThreshold times the best, best
 * first, at most maxBatchSize, until one does not fit the budget or the index-set limit (it stays
 * active). Stops if nothing could be accepted.
 */
void RefinementEngine::decide() {
  while (!candidates_.empty() &&
         state_.states[candidates_.front().second] != RefinementIndexState::Active) {
    std::ranges::pop_heap(candidates_, hasLowerPriority);  // stale entry
    candidates_.pop_back();
  }
  if (candidates_.empty()) {
    if (nNeeded_ == 0 && completed_.empty()) {
      stop(RefinementStopReason::NoCandidates);
    }
    return;
  }

  const double threshold = options_.batchThreshold * candidates_.front().first;
  size_t nAccepted = 0;
  while (nAccepted < options_.maxBatchSize && !candidates_.empty() &&
         candidates_.front().first >= threshold) {
    const size_t idx = candidates_.front().second;
    std::ranges::pop_heap(candidates_, hasLowerPriority);
    candidates_.pop_back();
    if (state_.states[idx] != RefinementIndexState::Active) continue;
    const RefinementStopReason refusal = tryAccept(idx);
    if (refusal != RefinementStopReason::None) {
      candidates_.emplace_back(priority(idx), idx);  // stays active
      std::ranges::push_heap(candidates_, hasLowerPriority);
      if (nAccepted == 0) stop(refusal);  // not even the best one fits
      break;
    }
    nAccepted++;
  }
  nDecisions_ += nAccepted > 0 ? 1 : 0;
}

/**
 * Makes @p idx old and adds its new admissible forward neighbours as pending (scheduled unless
 * evaluated ahead), if their tensor grids fit the remaining budget and the index-set limit;
 * otherwise changes nothing and returns the limit that refused it.
 */
RefinementStopReason RefinementEngine::tryAccept(const size_t idx) {
  refinement_tools::collectNewCandidates(state_, idx, options_.maxLevel, neighbour_, candidateDims_,
                                         candidateIndices_);
  // Every candidate counts, also one evaluated ahead: the budget does not depend on timing
  size_t cost = 0;
  for (const size_t dim : candidateDims_) {
    cost += refinement_tools::tensorGridCost(setup_, forwardNeighbour(idx, dim));
  }
  if (nInGridOrPending_ + candidateDims_.size() > options_.maxTensorGrids) {
    return RefinementStopReason::TensorGridLimit;
  }
  if (cost > options_.maxGridPoints - std::min(reservedCost_, options_.maxGridPoints)) {
    return RefinementStopReason::GridPointBudget;
  }

  state_.states[idx] = RefinementIndexState::Old;
  for (size_t i = 0; i < candidateDims_.size(); i++) {
    addCandidate(idx, candidateDims_[i], candidateIndices_[i]);
  }
  return RefinementStopReason::None;
}

/// Writes @p idx + e_dim to the scratch multi-index and returns it.
const LvlMI& RefinementEngine::forwardNeighbour(const size_t idx, const size_t dim) {
  const LvlMIView mi = state_.indexSet.levels(idx);
  neighbour_.resize(mi.size());
  std::copy(mi.data(), mi.data() + mi.size(), neighbour_.begin());
  neighbour_[dim]++;
  return neighbour_;
}

/// Makes the forward neighbour (@p idx, @p dim) pending: new, or evaluated ahead (@p existing).
void RefinementEngine::addCandidate(const size_t idx, const size_t dim, const size_t existing) {
  nInGridOrPending_++;
  if (existing == RefinementIndexSet::NOT_FOUND) {
    const LvlMI& mi = forwardNeighbour(idx, dim);
    const size_t candidate = addIndex(mi, RefinementIndexState::Pending);
    reservedCost_ += state_.costs[candidate];
    schedule(candidate);
    return;
  }

  state_.states[existing] = RefinementIndexState::Pending;
  nSpeculative_--;
  reservedCost_ += state_.costs[existing];
  if (state_.evaluated[existing]) {
    completed_.push_back(existing);  // committed with the decision's other results
  } else if (hasTask_[existing]) {
    // Queued ahead (moved to the needed tasks) or being evaluated: the decision waits for it
    const auto queued = std::ranges::find(speculativeQueue_, existing, &Task::second);
    if (queued != speculativeQueue_.end()) {
      speculativeQueue_.erase(queued);
      std::ranges::make_heap(speculativeQueue_, isCheaper);
      queue_.emplace_back(state_.costs[existing], existing);
      std::ranges::push_heap(queue_, isCheaper);
    }
    nNeeded_++;
  } else {
    schedule(existing);
  }
}

/// Queues a needed task for @p idx (the node rows of its levels are generated first).
void RefinementEngine::schedule(const size_t idx) {
  nodes_.ensureRows(state_.indexSet.levels(idx));
  queue_.emplace_back(state_.costs[idx], idx);
  std::ranges::push_heap(queue_, isCheaper);
  hasTask_[idx] = 1;
  nNeeded_++;
}

/**
 * Queues tasks ahead of the decisions (synchronized schedule, up to speculationWindow multi-indices
 * evaluated or queued ahead and not requested yet): the forward neighbours of the active
 * multi-indices of highest priority, whose other backward neighbours are in the grid. They are
 * chosen from the state only, so the choice does not depend on timing; idle workers evaluate them.
 */
void RefinementEngine::speculate() {
  const size_t window = options_.speculationWindow;
  if (window == 0 || nSpeculative_ >= window || candidates_.empty()) return;

  std::vector<Candidate> best(std::min(window, candidates_.size()));
  std::ranges::partial_sort_copy(candidates_, best, [](const Candidate& a, const Candidate& b) {
    return hasLowerPriority(b, a);
  });
  for (const Candidate& candidate : best) {
    const size_t idx = candidate.second;
    if (state_.states[idx] != RefinementIndexState::Active) continue;
    for (size_t dim = 0; dim < state_.indexSet.nDim() && nSpeculative_ < window; dim++) {
      if (!isSpeculationTarget(idx, dim)) continue;
      const size_t ahead = addIndex(forwardNeighbour(idx, dim), RefinementIndexState::Speculative);
      nSpeculative_++;
      nodes_.ensureRows(state_.indexSet.levels(ahead));
      speculativeQueue_.emplace_back(state_.costs[ahead], ahead);
      std::ranges::push_heap(speculativeQueue_, isCheaper);
      hasTask_[ahead] = 1;
    }
  }
}

/// Whether idx + e_dim is new and its backward neighbours other than idx are all in the grid.
bool RefinementEngine::isSpeculationTarget(const size_t idx, const size_t dim) {
  const RefinementIndexSet& set = state_.indexSet;
  if (set.levels(idx)[dim] >= options_.maxLevel ||
      set.findNeighbour(set.levels(idx), set.hash(idx), dim, 1) != RefinementIndexSet::NOT_FOUND) {
    return false;
  }
  const LvlMI& ahead = forwardNeighbour(idx, dim);
  const uint64_t hash = set.hashOf(ahead);
  for (size_t other = 0; other < ahead.size(); other++) {
    if (other == dim || ahead[other] == 0) continue;
    const size_t backward = set.findNeighbour(ahead, hash, other, -1);
    if (backward == RefinementIndexSet::NOT_FOUND || !state_.isInGrid(backward)) return false;
  }
  return true;
}

/// Appends @p mi to the state with @p indexState (and its cost).
size_t RefinementEngine::addIndex(const LvlMI& mi, const RefinementIndexState indexState) {
  const size_t idx = state_.add(mi, indexState, refinement_tools::tensorGridCost(setup_, mi));
  hasTask_.push_back(0);
  return idx;
}

void RefinementEngine::stop(const RefinementStopReason reason) {
  if (stopReason_ == RefinementStopReason::None) {
    stopReason_ = reason;
  }
  queue_.clear();  // unstarted tasks stay pending (or ahead) for a later call
  speculativeQueue_.clear();
}

/******
Helpers
******/

bool RefinementEngine::hasLowerPriority(const Candidate& a, const Candidate& b) noexcept {
  return a.first < b.first || (a.first == b.first && a.second > b.second);
}

bool RefinementEngine::isCheaper(const Task& a, const Task& b) noexcept {
  return a.first < b.first || (a.first == b.first && a.second > b.second);
}

double RefinementEngine::priority(const size_t idx) const {
  return refinement_tools::priorityOf(state_.gains[idx], state_.costs[idx], options_.indicator);
}

double RefinementEngine::elapsedSeconds() const {
  return std::chrono::duration<double>(std::chrono::steady_clock::now() - start_).count();
}

void RefinementEngine::fillStatistics() {
  statistics_.nDecisions = nDecisions_;
  statistics_.stopReason = stopReason_;
  statistics_.seconds = elapsedSeconds();
  for (const double idle : workerIdleSeconds_) statistics_.workerIdleSeconds += idle;
  for (size_t idx = 0; idx < state_.states.size(); idx++) {
    statistics_.nOld += state_.states[idx] == RefinementIndexState::Old ? 1 : 0;
    statistics_.nActive += state_.states[idx] == RefinementIndexState::Active ? 1 : 0;
  }
  statistics_.nTensorGrids = statistics_.nOld + statistics_.nActive;
  statistics_.nSpeculativeTensorGrids = speculativeEvaluated_.size();
  for (const size_t idx : speculativeEvaluated_) {
    statistics_.nUnusedSpeculativeTensorGrids +=
        state_.states[idx] == RefinementIndexState::Speculative ? 1 : 0;
  }
  statistics_.errorEstimate = refinement_tools::errorEstimateOf(state_);
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
