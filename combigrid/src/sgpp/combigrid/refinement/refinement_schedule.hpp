// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_schedule.hpp
 * @brief How the parallel dimension-adaptive refinement orders its decisions.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief How a dimension-adaptive refinement interleaves its decisions with the parallel
 * evaluation of tensor grids (see @c DimensionAdaptiveRefinement).
 */
enum class RefinementSchedule {
  /**
   * Every decision waits until the tensor grids requested by the previous one are evaluated, and
   * their results enter in a fixed order: the refined grid, the estimate and the error estimate
   * do not depend on the number of threads or on timing (except a stop by time). Idle threads
   * may evaluate likely future candidates ahead (@c RefinementOptions::speculationWindow).
   */
  Synchronized,
  /**
   * Results enter as they arrive and decisions are made whenever too few tensor grids are queued:
   * no waiting, but the refined grid depends on timing (with one thread it equals the
   * @c Synchronized one).
   */
  Asynchronous
};

}  // namespace combigrid
}  // namespace sgpp
