// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_stop_reason.hpp
 * @brief Why a call of the dimension-adaptive refinement stopped.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/// @brief Why a call of a dimension-adaptive refinement stopped (see @c RefinementOptions).
enum class RefinementStopReason {
  None,               ///< No call has stopped yet.
  Tolerance,          ///< The error estimate reached @c RefinementOptions::absoluteTolerance.
  RelativeTolerance,  ///< The error estimate reached @c RefinementOptions::relativeTolerance.
  GridPointBudget,    ///< The next tensor grid would exceed @c RefinementOptions::maxGridPoints.
  TensorGridLimit,    ///< The index set reached @c RefinementOptions::maxTensorGrids.
  TimeLimit,          ///< The call ran @c RefinementOptions::maxSeconds.
  NoCandidates,       ///< No admissible multi-index is left (e.g. all at the maximum level).
  StepDone            ///< @c DimensionAdaptiveRefinement::step performed its decision.
};

}  // namespace combigrid
}  // namespace sgpp
