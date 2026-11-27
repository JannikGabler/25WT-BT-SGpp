// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_options.hpp
 * @brief Arguments of a dimension-adaptive refinement: indicator, stopping criteria, batches,
 * schedule.
 */
#pragma once

#include <cstddef>
#include <limits>
#include <sgpp/combigrid/refinement/refinement_indicator.hpp>
#include <sgpp/combigrid/refinement/refinement_schedule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Arguments of a dimension-adaptive refinement (see @c DimensionAdaptiveRefinement).
 *
 * A call stops as soon as one of the stopping criteria holds; a criterion is off at its default
 * (@c 0 for the tolerances, the maximum value for the limits). @c
 * DimensionAdaptiveRefinement::refine requires at least one of the tolerances or limits.
 */
struct RefinementOptions {
  /// Ranks the candidates (which multi-index is refined next).
  RefinementIndicator indicator = RefinementIndicator::Profit;

  /// @name Stopping criteria
  /// @{

  /// Stop when the error estimate (the sum of the gains of the active multi-indices) is at most
  /// this value.
  double absoluteTolerance = 0.0;
  /// Stop when the error estimate is at most this value times the max-norm of the estimate.
  double relativeTolerance = 0.0;
  /// Evaluation budget of one call: the sum of the grid points of the tensor grids it evaluates
  /// (a tensor grid that does not fit is not evaluated; the budget is never exceeded).
  size_t maxGridPoints = std::numeric_limits<size_t>::max();
  /// Maximum number of multi-indices in the index set (including those with coefficient 0).
  size_t maxTensorGrids = std::numeric_limits<size_t>::max();
  /// Maximum wall time of one call in seconds (tensor grids being evaluated are finished).
  double maxSeconds = std::numeric_limits<double>::infinity();
  /// Maximum level per dimension: candidates beyond it are not admissible.
  LvlType maxLevel = 30;

  /// @}

  /// @name Parallelism
  /// @{

  /// A decision accepts every active multi-index whose priority is at least this factor times
  /// the best priority (1: only the best; smaller values give larger batches).
  double batchThreshold = 0.5;
  /// Largest number of multi-indices one decision accepts (independent of the thread count, so
  /// the refined grid is too).
  size_t maxBatchSize = 64;
  /// Order of decisions and evaluations.
  RefinementSchedule schedule = RefinementSchedule::Synchronized;
  /**
   * @c RefinementSchedule::Synchronized only: number of likely future candidates (forward
   * neighbours of the best active multi-indices) that idle threads may evaluate ahead of the
   * decisions (@c 0: none). The choice depends on the state only, so the result still does not
   * depend on the number of threads (but differs from the result without evaluation ahead in
   * round-off). A tensor grid evaluated ahead counts against @c maxGridPoints when a decision
   * requests it; up to this many tensor grids may be evaluated beyond the budget.
   */
  size_t speculationWindow = 0;

  /// @}
};

}  // namespace combigrid
}  // namespace sgpp
