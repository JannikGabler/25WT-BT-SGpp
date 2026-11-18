// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_quadrature_options.hpp
 * @brief Defines @c DeltasSparseGridQuadratureOptions, the additional options for a @c quadrature
 * call on a @c DeltasSparseGrid.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief Additional options for @c quadrature calls on a @c DeltasSparseGrid (scalar and
 * vector-valued source functions). @c quadrature on a @c SparseGrid takes @c QuadratureOptions.
 */
struct DeltasSparseGridQuadratureOptions {
  /**
   * @brief Whether the call reuses the index of the grid from the grid's cross-call operator data
   * cache (and stores it there if it is not present), instead of building it for this call only.
   *
   * Stored data: the 1D quadrature weights of every (dimension, level) in delta order (one double
   * per node of the 1D rule of the level, as the weight table of @c quadrature on a @c SparseGrid)
   * and the trie of the combination coefficients (a few words per tensor grid; e.g. 2 MiB in 3
   * dimensions at level 14). It depends on the grid only, so scalar and vector-valued calls share
   * it. The results are the same with and without reuse.
   *
   * Default @c true: building the index is serial and takes a noticeable part of a parallel call
   * (measured with a cheap integrand at 16 threads: 4-24 % for indices of 0.4-2 MiB), which
   * repeated calls on the same grid save. The index stays until the grid is destroyed; grids
   * dominated by long 1D rules keep much (e.g. 64 MiB for 2 dimensions with Clenshaw-Curtis nodes
   * of level 20). Set the option to @c false (or call @c clearCrossCallOperatorDataCache()) to
   * release the memory after a call.
   */
  bool useCrossCallOperatorDataCache = true;
};

}  // namespace combigrid
}  // namespace sgpp
