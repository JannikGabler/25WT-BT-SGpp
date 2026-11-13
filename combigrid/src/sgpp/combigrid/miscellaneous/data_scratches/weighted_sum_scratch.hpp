// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file weighted_sum_scratch.hpp
 * @brief Per-thread scratch storage of the weighted sum over the grid points of a tensor grid,
 * reused across tensor grids.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace of the weighted sum @f$\sum_p \prod_k w_{k,p_k} f(p)@f$ over the
 * grid points p of one tensor grid (see @c tools::weighted_sum::sumOverRows).
 *
 * The 1D weights @f$w_{k,i}@f$ are the quadrature weights for the quadrature
 * (@c misc::QuadratureScratch) and the 1D basis values at the query point for the global
 * interpolation (@c misc::GlobalInterpScratch). The scratch holds them, the nodes mapped onto the
 * domain, and the state of the row-wise iteration over the grid points. Its size does not depend
 * on the number of grid points.
 *
 * The iteration runs over the "iterated dimensions": the row dimension (the first dimension with
 * more than one node, or the last dimension if there is none) followed by the other dimensions
 * with more than one node in ascending order. The dimensions with one node keep their coordinate
 * and contribute a constant factor to the weights.
 *
 * For a vector-valued source function (m outputs), the scratch also holds the buffer of the
 * @c misc::VectorValues of the row loop: the m values at the current grid point, the m sums of
 * the tensor grid and the m sums of the current row.
 */
struct WeightedSumScratch {
  /// @brief Accumulator slot of the sums of the tensor grid (@c misc::VectorValues::zeroAcc).
  static constexpr size_t TG_SUM_ACC = 0;
  /// @brief Accumulator slot of the sums of the current row.
  static constexpr size_t ROW_SUM_ACC = 1;
  /// @brief Number of accumulator slots of the row loop.
  static constexpr size_t N_ACCS = 2;

  /**
   * @brief Allocates the buffers.
   * @param nDim                   Spatial dimension.
   * @param maxTGSumOfGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the tensor grids that
   *                               will be processed; sized once to avoid reallocations.
   * @param nOutputs               Number of outputs m of a vector-valued source function, 0 for
   *                               a scalar source function (no @ref valueBuffer).
   */
  WeightedSumScratch(const size_t nDim, const size_t maxTGSumOfGPCntsPerDim,
                     const size_t nOutputs = 0)
      : weights1D(maxTGSumOfGPCntsPerDim),
        nodesPerDim(maxTGSumOfGPCntsPerDim),
        nIterDims(0),
        iterDims(nDim),
        nGPOfIterDim(nDim),
        nodesOfIterDim(nDim),
        weightsOfIterDim(nDim),
        gp(nDim),
        gpMI(nDim),
        weightProducts(nDim + 1),
        valueBuffer(misc::VectorValues::bufferSize(nOutputs, N_ACCS)) {}

  /// @brief Concatenated 1D weights, one per node (per-dimension blocks back-to-back, in the
  /// order of the nodes).
  base::DataVector weights1D;
  /// @brief Node positions of the tensor grid, mapped onto the domain (per-dimension blocks
  /// back-to-back).
  base::DataVector nodesPerDim;

  /// @brief Number of iterated dimensions (at least 1).
  size_t nIterDims;
  /// @brief @c iterDims[j]: the @c j-th iterated dimension (@c iterDims[0] is the row dimension).
  std::vector<size_t> iterDims;
  /// @brief @c nGPOfIterDim[j]: number of nodes of the @c j-th iterated dimension.
  GPMI nGPOfIterDim;
  /// @brief @c nodesOfIterDim[j]: start of the nodes of the @c j-th iterated dimension in
  /// @ref nodesPerDim.
  std::vector<const double*> nodesOfIterDim;
  /// @brief @c weightsOfIterDim[j]: start of the weights of the @c j-th iterated dimension in
  /// @ref weights1D.
  std::vector<const double*> weightsOfIterDim;

  /// @brief Current grid point (in domain coordinates, indexed by dimension).
  base::DataVector gp;
  /// @brief @c gpMI[j]: node index of the current grid point in the @c j-th iterated dimension
  /// (entry 0, the row dimension, is unused).
  GPMI gpMI;
  /// @brief @c weightProducts[j] for @c 1 <= j <= nIterDims: product of the 1D weights of the
  /// current grid point in the iterated dimensions @c j, ..., nIterDims - 1 and in the
  /// dimensions with one node (entry 0 is unused).
  std::vector<double> weightProducts;

  /// @brief Buffer of the @c misc::VectorValues of a vector-valued source function (empty for a
  /// scalar one; accumulator slots @ref TG_SUM_ACC and @ref ROW_SUM_ACC).
  std::vector<double> valueBuffer;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
