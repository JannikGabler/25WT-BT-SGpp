// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_options.hpp
 * @brief Defines @c QuadratureOptions, the additional options for a @c quadrature call on a
 * @c SparseGrid.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief Additional options for @c quadrature calls on a @c SparseGrid (scalar and vector-valued
 * source functions). @c quadrature on a @c DeltasSparseGrid takes
 * @c DeltasSparseGridQuadratureOptions (reuse on by default).
 */
struct QuadratureOptions {
  /**
   * @brief Whether the call reuses the 1D quadrature weights of the grid from the grid's
   * cross-call operator data cache (and stores them there if they are not present), instead of
   * computing them for this call only.
   *
   * Stored data: the weights of every (dimension, level) row of the unit node table (one double
   * per node of the row). They depend on the grid only, so scalar and vector-valued calls share
   * them. The results are the same with and without reuse.
   *
   * Default @c false: one call computes the weights once for all of its tensor grids, which is a
   * small part of the call unless the grid is dominated by a few long 1D rules (e.g. 2 dimensions
   * with Clenshaw-Curtis nodes of level 20: computing the weights takes about a tenth of a call
   * at one thread and 32 MiB).
   */
  bool useCrossCallOperatorDataCache = false;
};

}  // namespace combigrid
}  // namespace sgpp
