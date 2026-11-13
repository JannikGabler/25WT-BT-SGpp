// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <cassert>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/weighted_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadratureScratch;
using sgpp::combigrid::misc::WeightedSumScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace quadrature {

double processTensorGrid(const QuadratureContext& context, const SourceFunc& sourceFunc,
                         const TensorGrid& tg, QuadratureScratch& scratch) {
  assert(tg.nDim() == context.sparseGrid.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  prepareTensorGrid(context, tg, scratch);

  return weighted_sum::sumOverRows(misc::ScalarValues(sourceFunc), scratch.rows);
}

void processTensorGrid(const QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                       const TensorGrid& tg, QuadratureScratch& scratch,
                       const std::span<double> out) {
  assert(tg.nDim() == context.sparseGrid.nDim());
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.rows.valueBuffer.size() ==
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), WeightedSumScratch::N_ACCS));

  if (tg.nGP() == 0) {
    std::ranges::fill(out, 0.0);
    return;
  }

  prepareTensorGrid(context, tg, scratch);

  const misc::VectorValues values(sourceFunc, scratch.rows.valueBuffer);
  values.store(weighted_sum::sumOverRows(values, scratch.rows), out.data());
}

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
      processTensorGrid(context, sourceFunc, tgData.tensorGrid, scratch, tgResult);
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

/**
 * Prepares the row loop over @p tg (weighted_sum::sumOverRows): 1D quadrature weights, nodes on
 * the domain, iterated dimensions.
 */
void prepareTensorGrid(const QuadratureContext& context, const TensorGrid& tg,
                       QuadratureScratch& scratch) {
  getWeights(context, tg, scratch);
  weighted_sum::mapNodesOntoDomain(context.domain, tg, scratch.rows);
  weighted_sum::setUpIterDims(tg, scratch.rows);
}

void getWeights(const QuadratureContext& context, const TensorGrid& tg,
                QuadratureScratch& scratch) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  base::DataVector& weights1D = scratch.rows.weights1D;
  weights1D.resize(gpCntPerDim.sumOfElems<size_t>());
  getQuadRulesForTensorGrid(tg, context.nodeGenFuncs, scratch.quadRules);

  size_t writeIdx = 0;

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    const GPCntType nNodes = gpCntPerDim[dim];
    scratch.quadRules[dim]->genWeightsInplace(std::span(weights1D).subspan(writeIdx, nNodes));
    writeIdx += nNodes;
  }
}

std::vector<QuadRule*> getQuadRulesForTensorGrid(const TensorGrid& tg,
                                                 const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  std::vector<QuadRule*> result(tg.nDim());

  getQuadRulesForTensorGrid(tg, nodeGenFuncs, result);

  return result;
}

void getQuadRulesForTensorGrid(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                               std::vector<QuadRule*>& out) {
  assert(tg.nDim() == nodeGenFuncs.size());
  assert(tg.nDim() == out.size());

  getQuadRulesForAllDims(tg.getGPCntPerDim(), nodeGenFuncs, out);
}

std::vector<QuadRule*> getQuadRulesForAllDims(const std::span<const GPCntType> gpCntPerDim,
                                              const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  std::vector<QuadRule*> result(gpCntPerDim.size());

  getQuadRulesForAllDims(gpCntPerDim, nodeGenFuncs, result);

  return result;
}

void getQuadRulesForAllDims(const std::span<const GPCntType> gpCntPerDim,
                            const std::vector<NodeGenFunc*>& nodeGenFuncs,
                            std::vector<QuadRule*>& out) {
  assert(gpCntPerDim.size() == nodeGenFuncs.size());
  assert(gpCntPerDim.size() == out.size());

  const size_t nDim = gpCntPerDim.size();

  for (size_t dim = 0; dim < nDim; dim++) {
    out[dim] = nodeGenFuncs[dim]->getQuadRule(gpCntPerDim[dim]);
  }
}

}  // namespace quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
