// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file global_interpolation_options.hpp
 * @brief Defines @c GlobalInterpolationOptions, the additional options for a @c interpolate call on
 * a @c DeltasSparseGrid.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief Additional options for @c interpolate calls on @c DeltasSparseGrid (scalar and
 * vector-valued source functions). @c interpolate on a @c SparseGrid keeps no data across
 * calls and has no options.
 */
struct GlobalInterpolationOptions {
  /**
   * @brief Whether the call reuses the index of the grid from the grid's cross-call operator data
   * cache (and stores it there if it is not present), instead of building it for this call only.
   *
   * Stored data: the trie of the combination coefficients, the regular 1D nodes, interpolation
   * methods and delta orders of every (dimension, level), the grid's tensor grids by size (about
   * one double per 1D node plus a few words per tensor grid; e.g. 0.75 MiB in 2 dimensions with
   * Clenshaw-Curtis nodes of level 12). It depends on the grid only, so scalar and vector-valued
   * calls share it. The results are the same with and without reuse.
   *
   * Default @c true: building the index costs up to the time of a call on small grids.
   */
  bool useCrossCallOperatorDataCache = true;
};

}  // namespace combigrid
}  // namespace sgpp
