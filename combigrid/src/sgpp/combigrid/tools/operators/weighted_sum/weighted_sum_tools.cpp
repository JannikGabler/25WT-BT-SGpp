// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/weighted_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>

using sgpp::combigrid::misc::WeightedSumScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace weighted_sum {

/**
 * Maps the nodes of @p tg onto the domain (once, instead of for every grid point; with
 * tools::mapToDomain, as all operators, so that they evaluate bitwise identical points) and moves
 * to the first grid point (all node indices 0).
 */
void mapNodesOntoDomain(const HyperCubeArea& domain, const TensorGrid& tg,
                        WeightedSumScratch& scratch) {
  const size_t nDim = tg.nDim();
  const GPMI& nGPPerDim = tg.getGPCntPerDim();
  const base::DataVector& nodesPerDim = tg.getNodesPerDim();

  assert(scratch.nodesPerDim.size() >= nodesPerDim.size());

  size_t offset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = domain[dim].first;
    const double width = domain[dim].second - domain[dim].first;
    double* const nodes = scratch.nodesPerDim.data() + offset;

    for (GPCntType nodeIdx = 0; nodeIdx < nGPPerDim[dim]; nodeIdx++) {
      nodes[nodeIdx] = tools::mapToDomain(nodesPerDim[offset + nodeIdx], lower, width);
    }

    scratch.gp[dim] = nodes[0];
    offset += nGPPerDim[dim];
  }
}

/**
 * Determines the iterated dimensions of @p tg (see WeightedSumScratch) with their nodes (mapped by
 * mapNodesOntoDomain) and weights (written to scratch.weights1D by the caller), all at node index
 * 0, and the weight products of the first grid point.
 */
void setUpIterDims(const TensorGrid& tg, WeightedSumScratch& scratch) {
  const size_t nDim = tg.nDim();
  const GPMI& nGPPerDim = tg.getGPCntPerDim();
  const size_t rowDim = findRowDim(nGPPerDim);

  assert(nDim >= 1);
  assert(scratch.weights1D.size() == tg.getNodesPerDim().size());

  size_t nIterDims = 1;
  double constWeight = 1.0;  // Product of the weights of the dimensions that are not iterated
  size_t offset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const GPCntType nNodes = nGPPerDim[dim];
    const double* const nodes = scratch.nodesPerDim.data() + offset;
    const double* const weights = scratch.weights1D.data() + offset;
    offset += nNodes;

    if (dim != rowDim && nNodes == 1) {
      constWeight *= weights[0];
      continue;
    }

    const size_t j = (dim == rowDim) ? 0 : nIterDims++;
    scratch.iterDims[j] = dim;
    scratch.nGPOfIterDim[j] = nNodes;
    scratch.nodesOfIterDim[j] = nodes;
    scratch.weightsOfIterDim[j] = weights;
    scratch.gpMI[j] = 0;
  }

  scratch.nIterDims = nIterDims;
  scratch.weightProducts[nIterDims] = constWeight;
  updateWeightProducts(scratch.weightsOfIterDim.data(), scratch.gpMI.data(),
                       scratch.weightProducts.data(), nIterDims - 1);
}

/**
 * Returns the row dimension: the first dimension with more than one node (or the last dimension
 * if there is none).
 */
size_t findRowDim(const GPMI& nGPPerDim) {
  size_t rowDim = 0;

  while (rowDim + 1 < nGPPerDim.size() && nGPPerDim[rowDim] == 1) {
    rowDim++;
  }

  return rowDim;
}

/**
 * Recomputes the weight products of the iterated dimensions @p j, ..., 1 (see WeightedSumScratch)
 * after their node indices changed (@c weightProducts[j + 1] must be up to date).
 */
void updateWeightProducts(const double* const* const weightsOfIterDim, const GPCntType* const gpMI,
                          double* const weightProducts, size_t j) {
  for (; j >= 1; j--) {
    weightProducts[j] = weightsOfIterDim[j][gpMI[j]] * weightProducts[j + 1];
  }
}

/**
 * Sums the weight times the source function over all grid points, row by row, starting at the
 * first grid point (set up by mapNodesOntoDomain and setUpIterDims), and returns the accumulator of
 * the sums. Written once for both value types (misc::ScalarValues, misc::VectorValues), which
 * perform the same operations per output.
 *
 * The data of the scratch is copied into locals first: the source function may write to any
 * memory as far as the compiler knows, so accessing the scratch vectors in the loop would reload
 * them for every row. For the same reason, @p values (a small view) is taken by value.
 */
template <typename ValueType>
typename ValueType::Acc sumOverRows(const ValueType values, WeightedSumScratch& scratch) {
  const size_t nIterDims = scratch.nIterDims;
  const size_t* const iterDims = scratch.iterDims.data();
  const GPCntType* const nGPOfIterDim = scratch.nGPOfIterDim.data();
  const double* const* const nodesOfIterDim = scratch.nodesOfIterDim.data();
  const double* const* const weightsOfIterDim = scratch.weightsOfIterDim.data();
  double* const weightProducts = scratch.weightProducts.data();
  GPCntType* const gpMI = scratch.gpMI.data();
  double* const gp = scratch.gp.data();
  const base::DataVector& gpVec = scratch.gp;

  const size_t rowDim = iterDims[0];
  const GPCntType nGPOfRow = nGPOfIterDim[0];
  const double* const rowNodes = nodesOfIterDim[0];
  const double* const rowWeights = weightsOfIterDim[0];

  typename ValueType::Acc result = values.zeroAcc(WeightedSumScratch::TG_SUM_ACC);

  while (true) {
    // Row of grid points: all node indices of the row dimension, the other ones fixed. The
    // weights of the other dimensions are the same for the whole row and multiplied in once.
    typename ValueType::Acc rowSum = values.zeroAcc(WeightedSumScratch::ROW_SUM_ACC);

    for (GPCntType rowIdx = 0; rowIdx < nGPOfRow; rowIdx++) {
      gp[rowDim] = rowNodes[rowIdx];
      values.addScaled(rowSum, rowWeights[rowIdx], values.evaluate(gpVec));
    }

    values.addScaled(result, weightProducts[1], rowSum);

    // Advances to the next row: increments the node index of the first iterated dimension
    // (after the row dimension) that does not wrap around
    size_t j = 1;

    for (; j < nIterDims; j++) {
      const size_t dim = iterDims[j];

      if (++gpMI[j] < nGPOfIterDim[j]) {
        gp[dim] = nodesOfIterDim[j][gpMI[j]];
        break;
      }

      gpMI[j] = 0;
      gp[dim] = nodesOfIterDim[j][0];
    }

    if (j == nIterDims) {  // Wrapped around in every iterated dimension: all rows are done
      return result;
    }

    updateWeightProducts(weightsOfIterDim, gpMI, weightProducts, j);
  }
}

// The quadrature (1D quadrature weights) and the global interpolation (1D basis values at the
// query point) call these instantiations.
template misc::ScalarValues::Acc sumOverRows(misc::ScalarValues values,
                                             WeightedSumScratch& scratch);
template misc::VectorValues::Acc sumOverRows(misc::VectorValues values,
                                             WeightedSumScratch& scratch);

}  // namespace weighted_sum
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
