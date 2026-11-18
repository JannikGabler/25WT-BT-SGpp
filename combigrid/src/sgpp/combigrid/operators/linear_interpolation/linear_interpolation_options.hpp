// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file linear_interpolation_options.hpp
 * @brief Defines @c LinearInterpolationOptions, the additional options for a @c interpolateLinear
 * call on a @c DeltasSparseGrid.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief Additional options for @c interpolateLinear calls on @c DeltasSparseGrid (scalar and
 * vector-valued source functions). @c interpolateLinear on a @c SparseGrid keeps no data across
 * calls and has no options.
 */
struct LinearInterpolationOptions {
  /**
   * @brief Whether the call reuses the index of the grid from the grid's cross-call operator data
   * cache (and stores it there if it is not present), instead of building it for this call only.
   *
   * Stored data: the trie over the multi-indices, the sorted 1D delta nodes, the fibers of every
   * dimension and the combination coefficients (e.g. 2 MiB in 2 dimensions at level 16, 0.7 MiB in
   * 10 dimensions at level 5). It depends on the grid only, so scalar and vector-valued calls share
   * it. The results are the same with and without reuse.
   *
   * Default @c true: building the index costs up to hundreds of times a call (2 dimensions, level
   * 16: about 3 ms vs. 8 microseconds).
   */
  bool useCrossCallOperatorDataCache = true;
};

}  // namespace combigrid
}  // namespace sgpp
