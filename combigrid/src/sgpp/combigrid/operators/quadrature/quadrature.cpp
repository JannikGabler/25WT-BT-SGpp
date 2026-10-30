// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <omp.h>
#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_utilities.hpp>
#include <sgpp/combigrid/tools/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/tools/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadratureScratch;

namespace sgpp {
namespace combigrid {

namespace {

/**
 * Sets @p result to the sum over the tensor grids of the coefficient times the vector-valued
 * quadrature estimate of the tensor grid (unit hypercube), in parallel: per thread first, then
 * added to @p result, as the reduction of the scalar operator does output by output.
 */
void combineTensorGrids(const QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                        const std::span<double> result) {
  const SparseGrid& sparseGrid = context.sparseGrid;
  const size_t nOutputs = result.size();

  std::ranges::fill(result, 0.0);

#pragma omp parallel
  {
    QuadratureScratch scratch(sparseGrid.nDim(), sparseGrid.getMaxTGSumOverGPCntsPerDim(),
                              nOutputs);
    std::vector<double> tgResult(nOutputs);
    std::vector<double> threadResult(nOutputs, 0.0);

#pragma omp for schedule(guided) nowait
    for (const TensorGridCTData& tgData : sparseGrid) {
      quadrature_operator::processTensorGrid(context, sourceFunc, tgData.tensorGrid, scratch,
                                             tgResult);
      for (size_t k = 0; k < nOutputs; k++) {
        threadResult[k] += tgData.coefficient * tgResult[k];
      }
    }

#pragma omp critical
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += threadResult[k];
    }
  }
}

}  // namespace

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const size_t nDim = sparseGrid.nDim();

  const QuadratureContext context{.sparseGrid = sparseGrid,
                                  .domain = genInstr.getDomain(),
                                  .nodeGenFuncs = genInstr.getNodeGenFuncs()};

  double result = 0;

#pragma omp parallel reduction(+ : result)
  {
    QuadratureScratch scratch(nDim, sparseGrid.getMaxTGSumOverGPCntsPerDim());

#pragma omp for schedule(guided) nowait
    for (const TensorGridCTData& tgData : sparseGrid) {
      result += tgData.coefficient * quadrature_operator::processTensorGrid(
                                         context, sourceFunc, tgData.tensorGrid, scratch);
    }
  }

  return genInstr.getVolumeOfDomain() * result;
}

void quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw base::data_exception("quadrature: the size of the result is not the number of outputs");
  }

  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  const QuadratureContext context{.sparseGrid = sparseGrid,
                                  .domain = genInstr.getDomain(),
                                  .nodeGenFuncs = genInstr.getNodeGenFuncs()};

  combineTensorGrids(context, sourceFunc, result);

  const double volume = genInstr.getVolumeOfDomain();
  for (double& value : result) {
    value *= volume;
  }
}

base::DataVector quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  quadrature(sourceFunc, sparseGrid, result);
  return result;
}

// TODO: Reorder internal methods
namespace quadrature_operator {

namespace {

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
 * Maps the nodes of @p tg onto the domain (once, instead of for every grid point; same
 * arithmetic as tools::denormalizeDataVector) and moves to the first grid point (all node
 * indices 0).
 */
void mapNodesOntoDomain(const QuadratureContext& context, const TensorGrid& tg,
                        QuadratureScratch& scratch) {
  const size_t nDim = tg.nDim();
  const GPMI& nGPPerDim = tg.getGPCntPerDim();
  const base::DataVector& nodesPerDim = tg.getNodesPerDim();

  assert(scratch.nodesPerDim.size() >= nodesPerDim.size());

  size_t offset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = context.domain[dim].first;
    const double width = context.domain[dim].second - context.domain[dim].first;
    double* const nodes = scratch.nodesPerDim.data() + offset;

    for (GPCntType nodeIdx = 0; nodeIdx < nGPPerDim[dim]; nodeIdx++) {
      nodes[nodeIdx] = nodesPerDim[offset + nodeIdx] * width + lower;
    }

    scratch.gp[dim] = nodes[0];
    offset += nGPPerDim[dim];
  }
}

/**
 * Recomputes the weight products of the iterated dimensions @p j, ..., 1 (see QuadratureScratch)
 * after their node indices changed (@c weightProducts[j + 1] must be up to date).
 */
void updateWeightProducts(const double* const* const weightsOfIterDim, const GPCntType* const gpMI,
                          double* const weightProducts, size_t j) {
  for (; j >= 1; j--) {
    weightProducts[j] = weightsOfIterDim[j][gpMI[j]] * weightProducts[j + 1];
  }
}

/**
 * Determines the iterated dimensions of @p tg (see QuadratureScratch) with their nodes (mapped by
 * mapNodesOntoDomain) and weights (filled in by getWeights), all at node index 0, and the weight
 * products of the first grid point.
 */
void setUpIterDims(const TensorGrid& tg, QuadratureScratch& scratch) {
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
 * Sums the weight times the source function over all grid points, row by row, starting at the
 * first grid point (set up by prepareTensorGrid), and returns the accumulator of the sums.
 * Written once for both value types (tools::ScalarValues, tools::VectorValues), which perform
 * the same operations per output.
 *
 * The data of the scratch is copied into locals first: the source function may write to any
 * memory as far as the compiler knows, so accessing the scratch vectors in the loop would reload
 * them for every row. For the same reason, @p values (a small view) is taken by value.
 */
template <typename Values>
typename Values::Acc sumOverRows(const Values values, QuadratureScratch& scratch) {
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

  typename Values::Acc result = values.zeroAcc(QuadratureScratch::TG_SUM_ACC);

  while (true) {
    // Row of grid points: all node indices of the row dimension, the other ones fixed. The
    // weights of the other dimensions are the same for the whole row and multiplied in once.
    typename Values::Acc rowSum = values.zeroAcc(QuadratureScratch::ROW_SUM_ACC);

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

/**
 * Prepares the row loop over @p tg (sumOverRows): 1D weights, nodes on the domain, iterated
 * dimensions.
 */
void prepareTensorGrid(const QuadratureContext& context, const TensorGrid& tg,
                       QuadratureScratch& scratch) {
  getWeights(context, tg, scratch);
  mapNodesOntoDomain(context, tg, scratch);
  setUpIterDims(tg, scratch);
}

}  // namespace

double processTensorGrid(const QuadratureContext& context, const SourceFunc& sourceFunc,
                         const TensorGrid& tg, QuadratureScratch& scratch) {
  assert(tg.nDim() == context.sparseGrid.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  prepareTensorGrid(context, tg, scratch);

  return sumOverRows(tools::ScalarValues(sourceFunc), scratch);
}

void processTensorGrid(const QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                       const TensorGrid& tg, QuadratureScratch& scratch,
                       const std::span<double> out) {
  assert(tg.nDim() == context.sparseGrid.nDim());
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.valueBuffer.size() ==
         tools::VectorValues::bufferSize(sourceFunc.nOutputs(), QuadratureScratch::N_ACCS));

  if (tg.nGP() == 0) {
    std::ranges::fill(out, 0.0);
    return;
  }

  prepareTensorGrid(context, tg, scratch);

  const tools::VectorValues values(sourceFunc, scratch.valueBuffer);
  values.store(sumOverRows(values, scratch), out.data());
}

void getWeights(const QuadratureContext& context, const TensorGrid& tg,
                QuadratureScratch& scratch) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  scratch.weights1D.resize(gpCntPerDim.sumOfElems<size_t>());
  tools::getQuadRulesForTensorGrid(tg, context.nodeGenFuncs, scratch.quadRules);

  size_t writeIdx = 0;

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    assert(scratch.weights1D.size() >= writeIdx + gpCntPerDim[dim]);  // TODO: Delete

    const GPCntType nNodes = gpCntPerDim[dim];
    scratch.quadRules[dim]->genWeightsInplace(
        std::span(scratch.weights1D).subspan(writeIdx, nNodes));
    writeIdx += nNodes;
  }
}

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp
