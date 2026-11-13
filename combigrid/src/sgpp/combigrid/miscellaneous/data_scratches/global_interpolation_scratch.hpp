// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file global_interpolation_scratch.hpp
 * @brief Per-thread scratch storage of the global interpolation on a @c SparseGrid, reused
 * across tensor grids.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace of the global interpolation on one tensor grid
 * (@c tools::global_interpolation::processTensorGrid).
 *
 * On a tensor grid, the interpolant at a query point x is
 * @f$\sum_p \prod_k L_{k,p_k}(x_k) f(p)@f$ with the 1D basis values @f$L_{k,i}(x_k)@f$ of the
 * interpolation method of dimension k: the weighted sum over the grid points
 * (@c tools::weighted_sum::sumOverRows) with the basis values as 1D weights. @ref rows is its
 * scratch (the basis values in @c weights1D); its size does not depend on the number of grid
 * points.
 */
struct GlobalInterpScratch {
  /**
   * @brief Allocates the buffers.
   * @param nDim                   Spatial dimension.
   * @param maxTGSumOfGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the tensor grids that
   *                               will be processed.
   * @param nOutputs               Number of outputs m of a vector-valued source function, 0 for
   *                               a scalar source function.
   */
  GlobalInterpScratch(const size_t nDim, const size_t maxTGSumOfGPCntsPerDim,
                      const size_t nOutputs = 0)
      : rows(nDim, maxTGSumOfGPCntsPerDim, nOutputs) {}

  /// @brief Scratch of the row loop: basis values (in @c weights1D), mapped nodes, iteration.
  WeightedSumScratch rows;
  /// @brief The (normalized) nodes of one dimension, as the interpolation methods take them.
  std::vector<double> nodes1D;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
