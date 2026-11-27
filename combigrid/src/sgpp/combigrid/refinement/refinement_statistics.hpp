// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_statistics.hpp
 * @brief Counters and state of a call of the dimension-adaptive refinement.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/refinement/refinement_stop_reason.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Counters of one call of a dimension-adaptive refinement and the state after it (see
 * @c DimensionAdaptiveRefinement).
 */
struct RefinementStatistics {
  /// @name The call
  /// @{

  size_t nDecisions = 0;                     ///< Decisions (accepted batches) of the call.
  size_t nEvaluatedTensorGrids = 0;          ///< Tensor grids evaluated by the call.
  size_t nEvaluatedGridPoints = 0;           ///< Their grid points (the budget's measure).
  size_t nSpeculativeTensorGrids = 0;        ///< Of them evaluated ahead of the decisions.
  size_t nUnusedSpeculativeTensorGrids = 0;  ///< Of those not requested by the end of the call.
  RefinementStopReason stopReason = RefinementStopReason::None;  ///< Why the call stopped.
  double seconds = 0.0;                                          ///< Wall time of the call.
  double workerIdleSeconds = 0.0;  ///< Time the threads waited for work, summed over them.

  /// @}

  /// @name State after the call
  /// @{

  size_t nTensorGrids = 0;     ///< Multi-indices of the index set (including coefficient 0).
  size_t nOld = 0;             ///< Of them refined (all forward neighbours examined).
  size_t nActive = 0;          ///< Of them not refined yet (candidates for the next decision).
  double errorEstimate = 0.0;  ///< Sum of the gains of the active multi-indices.

  /// @}
};

}  // namespace combigrid
}  // namespace sgpp
