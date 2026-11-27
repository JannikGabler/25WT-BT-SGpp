// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <omp.h>

#include <cstddef>
#include <exception>
#include <limits>
#include <memory>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_engine.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_node_storage.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_state.hpp>
#include <sgpp/combigrid/refinement/dimension_adaptive_refinement.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_decision_tools.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_hand_over_tools.hpp>
#include <span>
#include <stdexcept>
#include <vector>

namespace refinement_tools = sgpp::combigrid::tools::refinement;

namespace sgpp {
namespace combigrid {

namespace {

/// Whether a call with @p options stops by itself (not only when no candidate is left).
bool hasLimit(const RefinementOptions& options) {
  return options.absoluteTolerance > 0.0 || options.relativeTolerance > 0.0 ||
         options.maxGridPoints != std::numeric_limits<size_t>::max() ||
         options.maxTensorGrids != std::numeric_limits<size_t>::max() ||
         options.maxSeconds != std::numeric_limits<double>::infinity();
}

}  // namespace

DimensionAdaptiveRefinement::DimensionAdaptiveRefinement(SparseGrid& grid, RefinementTarget& target,
                                                         const RefinementOptions& options)
    : grid_(grid),
      target_(target),
      options_(options),
      state_(std::make_unique<misc::RefinementState>(grid.nDim(), target.nValues())) {}

DimensionAdaptiveRefinement::~DimensionAdaptiveRefinement() = default;

RefinementStatistics DimensionAdaptiveRefinement::refine() {
  if (!hasLimit(options_)) {
    throw std::invalid_argument(
        "No stopping criterion set (a tolerance, the grid-point budget, the tensor-grid limit or "
        "the time limit)");
  }
  return runCall(options_, std::numeric_limits<size_t>::max());
}

RefinementStatistics DimensionAdaptiveRefinement::step() {
  RefinementOptions options = options_;
  options.schedule = RefinementSchedule::Synchronized;
  return runCall(options, 1);
}

void DimensionAdaptiveRefinement::setOptions(const RefinementOptions& options) {
  options_ = options;
}

const RefinementOptions& DimensionAdaptiveRefinement::getOptions() const noexcept {
  return options_;
}

std::span<const double> DimensionAdaptiveRefinement::getEstimate() const noexcept {
  return estimate_;
}

double DimensionAdaptiveRefinement::getErrorEstimate() const noexcept { return errorEstimate_; }

/**
 * The grid's storage is handed over to the call and back in any case, so the grid is usable
 * afterwards even if the target throws; the exception is rethrown then.
 */
RefinementStatistics DimensionAdaptiveRefinement::runCall(const RefinementOptions& options,
                                                          const size_t maxDecisions) {
  if (target_.nValues() != state_->nValues) {
    throw std::invalid_argument("The number of values of the target changed");
  }

  refinement_tools::synchronizeState(grid_, *state_, options.maxLevel);
  const std::vector<CTCoeffType> coefficientsBefore = state_->coefficients;
  const size_t nIndicesBefore = state_->states.size();
  misc::RefinementNodeStorage nodes(grid_.getSetup(),
                                    refinement_tools::nodeCapacity(*state_, options.maxLevel));
  refinement_tools::beginHandOver(grid_, nodes);

  RefinementStatistics statistics;
  const std::exception_ptr error = runEngine(options, maxDecisions, nodes, statistics);

  const bool changed = state_->states.size() != nIndicesBefore ||
                       state_->coefficients != coefficientsBefore || statistics.nDecisions > 0;
  refinement_tools::endHandOver(grid_, *state_, nodes, changed);
  estimate_ = refinement_tools::combineEstimate(*state_, target_);
  errorEstimate_ = refinement_tools::errorEstimateOf(*state_);

  if (error) {
    std::rethrow_exception(error);
  }

  return statistics;
}

/// Runs the engine between the target's begin and end of the call; returns its first exception.
std::exception_ptr DimensionAdaptiveRefinement::runEngine(const RefinementOptions& options,
                                                          const size_t maxDecisions,
                                                          misc::RefinementNodeStorage& nodes,
                                                          RefinementStatistics& statistics) {
  std::exception_ptr error;
  try {
    const size_t nWorkers = static_cast<size_t>(omp_get_max_threads());
    target_.beginRefinementCall(grid_.getSetup(), nodes.unitTable(), nWorkers);
    misc::RefinementEngine engine(*state_, nodes, target_, grid_.getSetup(), options, maxDecisions);
    statistics = engine.run(nWorkers);
  } catch (...) {
    error = std::current_exception();
  }
  try {
    target_.endRefinementCall();
  } catch (...) {
    if (!error) error = std::current_exception();
  }
  return error;
}

RefinementStatistics refine(SparseGrid& grid, RefinementTarget& target,
                            const RefinementOptions& options) {
  DimensionAdaptiveRefinement refinement(grid, target, options);
  return refinement.refine();
}

}  // namespace combigrid
}  // namespace sgpp
