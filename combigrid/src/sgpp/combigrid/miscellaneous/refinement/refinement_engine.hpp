// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_engine.hpp
 * @brief The parallel engine of one call of the dimension-adaptive refinement.
 */
#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <mutex>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_state.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_node_storage.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_state.hpp>
#include <sgpp/combigrid/refinement/refinement_options.hpp>
#include <sgpp/combigrid/refinement/refinement_statistics.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief One call of a dimension-adaptive refinement (Gerstner-Griebel with batches): a pool of
 * workers evaluates tensor grids (tasks) while the management (decisions, commits, stopping
 * criteria) runs on whichever worker holds the lock and has finished a task; there is no barrier
 * between decisions.
 *
 * Phases: first every multi-index of the grid without values is evaluated (a new state, e.g. the
 * first call on a grid); then decisions accept the active multi-indices whose priority is at
 * least @c batchThreshold times the best (at most @c maxBatchSize, cut where the new candidates
 * would exceed the budget) and schedule their new admissible forward neighbours. With
 * @c RefinementSchedule::Synchronized, a decision is made only when the tasks of the previous one
 * are evaluated, and its results are committed in index order: the state does not depend on the
 * number of threads or on timing. With @c RefinementSchedule::Asynchronous, results are committed
 * as they arrive and a decision is made whenever fewer than two tasks per worker are queued or
 * running. Once a stopping criterion holds, queued tasks are dropped (their multi-indices stay
 * pending for a later call) and started ones are finished and committed. With a speculation window
 * (synchronized only), idle workers evaluate likely future candidates while a decision's tasks are
 * still being evaluated; a decision waits only for the tasks it needs.
 */
class RefinementEngine {
 public:
  /**
   * @param state        State of the refinement (modified).
   * @param nodes        Node tables of the grid (rows are generated as tasks need them).
   * @param target       Refinement target (its call has begun).
   * @param setup        Setup of the grid.
   * @param options      Options of the call.
   * @param maxDecisions Largest number of decisions of the call (@c
   * RefinementStopReason::StepDone).
   */
  RefinementEngine(RefinementState& state, RefinementNodeStorage& nodes, RefinementTarget& target,
                   const SparseGridSetup& setup, const RefinementOptions& options,
                   size_t maxDecisions);

  /**
   * @brief Runs the call with @p nWorkers threads.
   * @return The statistics of the call (and the state after it).
   * @throws The first exception of the target after all workers have stopped; the results
   * committed until then stay in the state.
   */
  RefinementStatistics run(size_t nWorkers);

 private:
  /// Task: (cost, index); the most expensive task first, then the smaller index.
  using Task = std::pair<size_t, size_t>;
  /// Active multi-index: (priority, index); the highest priority first, then the smaller index.
  using Candidate = std::pair<double, size_t>;

  /// @name Workers
  /// @{
  void work(size_t worker);
  void manageSafely();
  bool takeTask(size_t worker);
  std::exception_ptr evaluateTask(size_t worker);
  void storeResult(size_t worker, const std::exception_ptr& error);
  void waitForWork(std::unique_lock<std::mutex>& lock, size_t worker);
  bool isFinished() const noexcept;
  /// @}

  /// @name Management (with the lock held)
  /// @{
  void manage();
  void decideWhileNeeded();
  void scheduleInitialTasks();
  void finishInitialPhase();
  void computeRunningEstimate();
  void commitCompleted();
  void commit(size_t idx);
  bool checkStop();
  bool needsDecision() const noexcept;
  void decide();
  RefinementStopReason tryAccept(size_t idx);
  const LvlMI& forwardNeighbour(size_t idx, size_t dim);
  void addCandidate(size_t idx, size_t dim, size_t existing);
  void schedule(size_t idx);
  void speculate();
  bool isSpeculationTarget(size_t idx, size_t dim);
  size_t addIndex(const LvlMI& mi, RefinementIndexState indexState);
  void stop(RefinementStopReason reason);
  /// @}

  /// @name Helpers
  /// @{
  static bool hasLowerPriority(const Candidate& a, const Candidate& b) noexcept;
  static bool isCheaper(const Task& a, const Task& b) noexcept;
  double priority(size_t idx) const;
  double elapsedSeconds() const;
  void fillStatistics();
  /// @}

  RefinementState& state_;            ///< State of the refinement.
  RefinementNodeStorage& nodes_;      ///< Node tables.
  RefinementTarget& target_;          ///< Target.
  const SparseGridSetup& setup_;      ///< Setup of the grid.
  const RefinementOptions& options_;  ///< Options of the call.
  const size_t maxDecisions_;         ///< Largest number of decisions of the call.
  size_t nWorkers_ = 1;               ///< Number of workers.

  std::mutex mutex_;                          ///< Guards everything below and the state.
  std::condition_variable workAvailable_;     ///< Signals new tasks or the end of the call.
  std::vector<Task> queue_;                   ///< Needed tasks not started (heap, @ref isCheaper).
  std::vector<Task> speculativeQueue_;        ///< Tasks ahead of the decisions (heap, same order).
  std::vector<uint8_t> hasTask_;              ///< Per multi-index: queued or evaluated in the call.
  std::vector<size_t> speculativeEvaluated_;  ///< Multi-indices evaluated ahead in the call.
  size_t nNeeded_ = 0;                        ///< Needed tasks queued or being evaluated.
  size_t nSpeculative_ = 0;                   ///< Multi-indices ahead, not requested yet.
  std::vector<size_t> completed_;             ///< Evaluated multi-indices not committed yet.
  std::vector<Candidate> candidates_;  ///< Active multi-indices (heap, @ref hasLowerPriority).
  size_t running_ = 0;                 ///< Tasks being evaluated.
  bool initialPhase_ = true;           ///< The grid's multi-indices are being evaluated.
  size_t nDecisions_ = 0;              ///< Decisions of the call.
  size_t reservedCost_ = 0;            ///< Grid points of the tasks scheduled in the call.
  size_t nInGridOrPending_ = 0;        ///< For the limit on the index set.
  RefinementStopReason stopReason_ = RefinementStopReason::None;  ///< Why the call stops.
  std::exception_ptr error_;                     ///< First exception of the target.
  std::chrono::steady_clock::time_point start_;  ///< Start of the call.
  RefinementStatistics statistics_;              ///< Statistics of the call.

  /// Sum of the increments committed so far (not finished): the estimate of the grid.
  std::vector<tools::numerics::CompensatedSum<double>> runningEstimate_;
  std::vector<tools::numerics::CompensatedSum<double>> sums_;  ///< Scratch of the increments.
  std::vector<double> increment_;                              ///< Scratch: increment.
  std::vector<double> finished_;          ///< Scratch: finished increment / estimate.
  std::vector<size_t> support_;           ///< Scratch: support of a multi-index.
  std::vector<size_t> candidateDims_;     ///< Scratch: dimensions of new candidates.
  std::vector<size_t> candidateIndices_;  ///< Scratch: their indices (or not found).
  LvlMI neighbour_;                       ///< Scratch: a forward neighbour.

  std::vector<LvlMI> workerLevels_;                ///< Per worker: levels of its task.
  std::vector<size_t> workerTasks_;                ///< Per worker: index of its task.
  std::vector<std::vector<double>> workerValues_;  ///< Per worker: values of its task.
  std::vector<double> workerIdleSeconds_;          ///< Per worker: time spent waiting.
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
