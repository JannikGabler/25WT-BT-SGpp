// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_scratch.hpp
 * @brief Per-thread scratch storage reused across quadrature calls.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace for the quadrature operator.
 *
 * Holds the buffers needed to integrate one tensor grid: the concatenated 1D weight vector for
 * every dimension, the per-dimension quadrature-rule pointers, the nodes mapped onto the domain,
 * and the state of the row-wise iteration over the grid points (see
 * @c quadrature_operator::processTensorGrid). Its size does not depend on the number of grid
 * points.
 *
 * The iteration runs over the "iterated dimensions": the row dimension (the first dimension with
 * more than one node, or the last dimension if there is none) followed by the other dimensions
 * with more than one node in ascending order. The dimensions with one node keep their coordinate
 * and contribute a constant factor to the weights.
 *
 * For a vector-valued source function (m outputs), the scratch also holds the buffer of the
 * @c tools::VectorValues of the row loop: the m values at the current grid point, the m sums of
 * the tensor grid and the m sums of the current row.
 */
struct QuadratureScratch {
  /// @brief Accumulator slot of the sums of the tensor grid (@c tools::VectorValues::zeroAcc).
  static constexpr size_t TG_SUM_ACC = 0;
  /// @brief Accumulator slot of the sums of the current row.
  static constexpr size_t ROW_SUM_ACC = 1;
  /// @brief Number of accumulator slots of the row loop.
  static constexpr size_t N_ACCS = 2;

  /**
   * @brief Allocates the buffers.
   * @param nDim                  Spatial dimension.
   * @param maxTGSumOfGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the
   *                              tensor grids that will be integrated; sized
   *                              once to avoid reallocations.
   * @param nOutputs              Number of outputs m of a vector-valued source function, 0 for a
   *                              scalar source function (no @ref valueBuffer).
   */
  QuadratureScratch(const size_t nDim, const size_t maxTGSumOfGPCntsPerDim,
                    const size_t nOutputs = 0)
      : weights1D(maxTGSumOfGPCntsPerDim),
        quadRules(nDim),
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

 public:
  /// @brief Concatenated 1D quadrature weights (per-dimension blocks back-to-back).
  base::DataVector weights1D;
  /// @brief Per-dimension quadrature-rule pointers; non-owning.
  std::vector<QuadRule*> quadRules;
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

  /// @brief Buffer of the @c tools::VectorValues of a vector-valued source function (empty for a
  /// scalar one; accumulator slots @ref TG_SUM_ACC and @ref ROW_SUM_ACC).
  std::vector<double> valueBuffer;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
