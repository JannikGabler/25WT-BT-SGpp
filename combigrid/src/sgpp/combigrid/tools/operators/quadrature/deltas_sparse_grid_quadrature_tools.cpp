// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cassert>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_quadrature_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/operators/cross_call/cross_call_operator_data_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/weighted_sum/deltas_sparse_grid_weighted_sum_tools.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::DeltaWeightedSumScratch;
using sgpp::combigrid::misc::FlatNestedVec;
using sgpp::combigrid::misc::GenericDSGOperatorContext;
using sgpp::combigrid::tools::deltas_sparse_grid_generation::compNodeIdxLvlMapFuncs;
using sgpp::combigrid::tools::deltas_sparse_grid_generation::mapDeltaNodeIdxToRegular;
using sgpp::combigrid::tools::dsg_weighted_sum::processDeltaTensorGrid;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_quadrature {

std::vector<double> estimateDeltaTensorGrids(const GenericDSGOperatorContext& context,
                                             const SourceFunc& sourceFunc) {
  const DeltasSparseGrid& deltasSparseGrid = context.deltasSparseGrid;
  const size_t nTG = deltasSparseGrid.nTG();
  const std::span<const size_t> order = deltasSparseGrid.getTGProcessingOrder();

  std::vector<double> values(nTG);

#pragma omp parallel
  {
    DeltaWeightedSumScratch scratch(deltasSparseGrid.nDim(),
                                    deltasSparseGrid.getMaxTGSumOverGPCntsPerDim());

    // The delta tensor grids differ in size by orders of magnitude -> Dynamic scheduling,
    // largest first
#pragma omp for schedule(dynamic) nowait
    for (size_t orderIdx = 0; orderIdx < nTG; orderIdx++) {
      const size_t tgIdx = order[orderIdx];
      values[tgIdx] = processDeltaTensorGrid(context, sourceFunc,
                                             deltasSparseGrid.getDomainTensorGrid(tgIdx), scratch);
    }
  }

  return values;
}

FlatNestedVec<double, 2> estimateDeltaTensorGrids(const GenericDSGOperatorContext& context,
                                                  const VectorSourceFunc& sourceFunc) {
  const DeltasSparseGrid& deltasSparseGrid = context.deltasSparseGrid;
  const size_t nTG = deltasSparseGrid.nTG();
  const size_t nOutputs = sourceFunc.nOutputs();
  const std::span<const size_t> order = deltasSparseGrid.getTGProcessingOrder();

  FlatNestedVec<double, 2> values(nTG, nOutputs);

#pragma omp parallel
  {
    DeltaWeightedSumScratch scratch(deltasSparseGrid.nDim(),
                                    deltasSparseGrid.getMaxTGSumOverGPCntsPerDim(), nOutputs);

    // The delta tensor grids differ in size by orders of magnitude -> Dynamic scheduling,
    // largest first
#pragma omp for schedule(dynamic) nowait
    for (size_t orderIdx = 0; orderIdx < nTG; orderIdx++) {
      const size_t tgIdx = order[orderIdx];
      processDeltaTensorGrid(context, sourceFunc, deltasSparseGrid.getDomainTensorGrid(tgIdx),
                             scratch, values[tgIdx]);
    }
  }

  return values;
}

// TODO: Functionaly decompose this function if feasible regarding runtime (?)
FlatNestedVec<double, 3> comp1DWeights(const DeltasSparseGrid& deltasSparseGrid) {
  const size_t nDim = deltasSparseGrid.nDim();
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const LvlType boundaryLvlOffset = genInstr.getBoundaryLevelOffset();
  const std::vector<NodeIdxLvlMapFunc> nodeIdxLvlMapFuncs =
      compNodeIdxLvlMapFuncs(deltasSparseGrid);

  // The maximum level per dimension is the one of the node table
  const NodeTable& nodeTable = deltasSparseGrid.getDomainNodeTable();

  FlatNestedVec<double, 3> weights;
  weights.reserveSubVecs(1, nDim);
  base::DataVector sortedWeights;  // Weights of one level, sorted by node position

  for (size_t dim = 0; dim < nDim; dim++) {
    const NodeFamily* const nodeFamily = genInstr.getNodeFamilyForDim(dim);
    const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);

    weights.appendSubVec(1);  // weights[dim]

    for (LvlType lvl = 0; lvl <= nodeTable.maxLvl(dim); lvl++) {
      const bool includeBoundary = lvl >= boundaryLvlOffset;
      const GPCntType nNodes = lvl2GPCntFunc(lvl) + (includeBoundary ? 2 : 0);

      sortedWeights.resize(nNodes);
      nodeFamily->getQuadRule(nNodes, includeBoundary)->genWeightsInplace(sortedWeights);

      weights.appendSubVec(2, nNodes);  // weights[dim][lvl]
      // The sub-vector just appended consists of the last nNodes elements.
      const std::span<double> weightsOfLvl = weights.flat().last(nNodes);
      size_t writeIdx = 0;

      for (LvlType deltaLvl = 0; deltaLvl <= lvl; deltaLvl++) {
        const GPCntType nDeltaNodes =
            tools::getDeltaNodeCnt(deltaLvl, lvl2GPCntFunc, deltaLvl == boundaryLvlOffset);

        for (GPCntType deltaNodeIdx = 0; deltaNodeIdx < nDeltaNodes; deltaNodeIdx++) {
          const GPCntType nodeIdx =
              mapDeltaNodeIdxToRegular(nDeltaNodes, nNodes, deltaLvl, lvl, deltaNodeIdx,
                                       nodeIdxLvlMapFuncs[dim], boundaryLvlOffset);

          assert(nodeIdx < nNodes);
          weightsOfLvl[writeIdx++] = sortedWeights[nodeIdx];
        }
      }

      assert(writeIdx == nNodes);
    }
  }

  return weights;
}

std::shared_ptr<const misc::DSGQuadIndex> getIndex(const DeltasSparseGrid& deltasSparseGrid,
                                                   const bool useCache) {
  static constexpr misc::CrossCallOperatorDataDescriptor DESCRIPTOR{
      .operatorName = "quadrature (DeltasSparseGrid)",
      .description =
          "1D quadrature weights per (dimension, level) in delta order and the trie of "
          "the combination coefficients",
      .key = ""};
  return cross_operator_call::getOperatorData<misc::DSGQuadIndex>(
      deltasSparseGrid, useCache, DESCRIPTOR,
      [&]() { return std::make_unique<misc::DSGQuadIndex>(deltasSparseGrid); });
}

}  // namespace dsg_quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
