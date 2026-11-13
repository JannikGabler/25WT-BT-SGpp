// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_scratch.hpp
 * @brief Per-thread scratch storage of the quadrature on a @c SparseGrid, reused across tensor
 * grids.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace of the quadrature on one tensor grid
 * (@c tools::quadrature::processTensorGrid).
 *
 * On a tensor grid, the quadrature is the weighted sum @f$\sum_p \prod_k w_{k,p_k} f(p)@f$ with
 * the 1D quadrature weights @f$w_{k,i}@f$ (@c tools::weighted_sum::sumOverRows). @ref rows is its
 * scratch (the quadrature weights in @c weights1D); its size does not depend on the number of
 * grid points.
 */
struct QuadratureScratch {
  /**
   * @brief Allocates the buffers.
   * @param nDim                   Spatial dimension.
   * @param maxTGSumOfGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the tensor grids that
   *                               will be integrated; sized once to avoid reallocations.
   * @param nOutputs               Number of outputs m of a vector-valued source function, 0 for
   *                               a scalar source function.
   */
  QuadratureScratch(const size_t nDim, const size_t maxTGSumOfGPCntsPerDim,
                    const size_t nOutputs = 0)
      : rows(nDim, maxTGSumOfGPCntsPerDim, nOutputs), quadRules(nDim) {}

  /// @brief Scratch of the row loop: quadrature weights (in @c weights1D), mapped nodes,
  /// iteration.
  WeightedSumScratch rows;
  /// @brief Per-dimension quadrature-rule pointers; non-owning.
  std::vector<QuadRule*> quadRules;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
