// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/projected_sub_trie_builder.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::DeltaQuadratureContext;
using sgpp::combigrid::misc::DeltaQuadratureScratch;
using sgpp::combigrid::misc::FlatNestedVec;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace dsg_quadrature {

std::vector<NodeIdxLvlMapFunc> compNodeIdxLvlMapFuncs(const DeltasSparseGrid& deltasSparseGrid) {
  const size_t nDim = deltasSparseGrid.nDim();
  const std::vector<NodeGenFunc*>& nodeGenFuncs = deltasSparseGrid.getGenInstr().getNodeGenFuncs();
  const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs =
      deltasSparseGrid.getGenInstr().getLvl2GPCntFuncs();

  std::vector<NodeIdxLvlMapFunc> nodeIdxLvlMapFuncs(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    nodeIdxLvlMapFuncs[dim] =
        nodeGenFuncs[dim]->getNodeIdxMapFromDeltaTGLvlToRegularLvl(lvl2GPCntFuncs[dim]);
  }

  return nodeIdxLvlMapFuncs;
}

GPCntType mapNodeIdx(const GPCntType nNodesInDeltaTG, const GPCntType nNodesInTargetTG,
                     const LvlType deltaTGLvl, const LvlType targetTGLvl,
                     const GPCntType gpIdxInDeltaTG, const NodeIdxLvlMapFunc nodeIdxLvlMapFunc,
                     const LvlType boundaryLvlOffset) {
  if (deltaTGLvl == boundaryLvlOffset) {  // Has the delta tensor grid boundary nodes?
    if (gpIdxInDeltaTG == 0) {            // left boundary node
      return 0;
    } else if (gpIdxInDeltaTG == nNodesInDeltaTG - 1) {  // right boundary node
      return nNodesInTargetTG - 1;
    } else {
      // Has the target tensor grid boundary nodes?
      return nodeIdxLvlMapFunc(gpIdxInDeltaTG - 1, deltaTGLvl, targetTGLvl) +
             (targetTGLvl >= boundaryLvlOffset ? 1 : 0);
    }
  } else {
    // Has the target tensor grid boundary nodes?
    return nodeIdxLvlMapFunc(gpIdxInDeltaTG, deltaTGLvl, targetTGLvl) +
           (targetTGLvl >= boundaryLvlOffset ? 1 : 0);
  }
}

FlatNestedVec<double, 3> comp1DWeights(const DeltasSparseGrid& deltasSparseGrid) {
  const size_t nDim = deltasSparseGrid.nDim();
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const LvlType boundaryLvlOffset = genInstr.getBoundaryLevelOffset();
  const std::vector<NodeIdxLvlMapFunc> nodeIdxLvlMapFuncs =
      compNodeIdxLvlMapFuncs(deltasSparseGrid);

  // Maximum level per dimension
  LvlMI maxLvls(nDim, LvlType{0});

  for (const TensorGridCTData& tgData : deltasSparseGrid) {
    for (size_t dim = 0; dim < nDim; dim++) {
      maxLvls[dim] = std::max(maxLvls[dim], tgData.mi[dim]);
    }
  }

  FlatNestedVec<double, 3> weights;
  weights.reserveSubVecs(1, nDim);
  base::DataVector sortedWeights;  // Weights of one level, sorted by node position

  for (size_t dim = 0; dim < nDim; dim++) {
    const NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
    const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);

    weights.appendSubVec(1);  // weights[dim]

    for (LvlType lvl = 0; lvl <= maxLvls[dim]; lvl++) {
      const GPCntType nNodes = lvl2GPCntFunc(lvl) + (lvl >= boundaryLvlOffset ? 2 : 0);

      sortedWeights.resize(nNodes);
      nodeGenFunc->getQuadRule(nNodes)->genWeightsInplace(sortedWeights);

      weights.appendSubVec(2, nNodes);  // weights[dim][lvl]
      // The sub-vector just appended consists of the last nNodes elements.
      const std::span<double> weightsOfLvl = weights.flat().last(nNodes);
      size_t writeIdx = 0;

      for (LvlType deltaLvl = 0; deltaLvl <= lvl; deltaLvl++) {
        const GPCntType nDeltaNodes =
            tools::getDeltaNodeCnt(deltaLvl, lvl2GPCntFunc, deltaLvl == boundaryLvlOffset);

        for (GPCntType deltaNodeIdx = 0; deltaNodeIdx < nDeltaNodes; deltaNodeIdx++) {
          const GPCntType nodeIdx = mapNodeIdx(nDeltaNodes, nNodes, deltaLvl, lvl, deltaNodeIdx,
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

namespace {

/**
 * Sums the global weight times the source function over the grid points of @p deltaTGData and
 * returns the accumulator of the sums. Written once for both value types (misc::ScalarValues,
 * misc::VectorValues), which perform the same operations per output; the global weight of a grid
 * point is computed once for all outputs. @p values (a small view) is captured by value, so that
 * the row loop reaches the source function through the closure directly (as the scalar kernel
 * did before the value types).
 */
template <typename ValueType>
typename ValueType::Acc sumWithGlobalWeights(const DeltaQuadratureContext& context,
                                             const ValueType values,
                                             const TensorGridCTData& deltaTGData,
                                             DeltaQuadratureScratch& scratch) {
  typename ValueType::Acc result = values.zeroAcc(DeltaQuadratureScratch::TG_SUM_ACC);

  forEachGPWithGlobalWeight(
      context, deltaTGData, scratch,
      [&result, values](const base::DataVector& gp, const double globalWeight) {
        values.addScaled(result, globalWeight, values.evaluate(gp));
      });

  return result;
}

}  // namespace

double processDeltaTensorGrid(const DeltaQuadratureContext& context, const SourceFunc& sourceFunc,
                              const TensorGridCTData& deltaTGData,
                              DeltaQuadratureScratch& scratch) {
  return sumWithGlobalWeights(context, misc::ScalarValues(sourceFunc), deltaTGData, scratch);
}

void processDeltaTensorGrid(const DeltaQuadratureContext& context,
                            const VectorSourceFunc& sourceFunc, const TensorGridCTData& deltaTGData,
                            DeltaQuadratureScratch& scratch, const std::span<double> out) {
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.valueBuffer.size() ==
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), DeltaQuadratureScratch::N_ACCS));

  const misc::VectorValues values(sourceFunc, scratch.valueBuffer);
  values.store(sumWithGlobalWeights(context, values, deltaTGData, scratch), out.data());
}

/**
 * Per thread first, then added to @p result, as the reduction of the scalar operator does output
 * by output.
 */
void combineDeltaTensorGrids(const DeltaQuadratureContext& context,
                             const VectorSourceFunc& sourceFunc, const std::span<double> result) {
  const DeltasSparseGrid& deltasSparseGrid = context.deltasSparseGrid;
  const size_t nOutputs = result.size();

  std::ranges::fill(result, 0.0);

#pragma omp parallel
  {
    DeltaQuadratureScratch scratch(deltasSparseGrid.nDim(),
                                   deltasSparseGrid.getMaxTGSumOverGPCntsPerDim(), nOutputs);
    std::vector<double> tgResult(nOutputs);
    std::vector<double> threadResult(nOutputs, 0.0);

    // Dynamic scheduling as in the scalar operator (delta tensor grids differ in size by orders
    // of magnitude)
#pragma omp for schedule(dynamic) nowait
    for (const TensorGridCTData& tgData : deltasSparseGrid) {
      processDeltaTensorGrid(context, sourceFunc, tgData, scratch, tgResult);
      for (size_t k = 0; k < nOutputs; k++) {
        threadResult[k] += tgResult[k];
      }
    }

#pragma omp critical
    for (size_t k = 0; k < nOutputs; k++) {
      result[k] += threadResult[k];
    }
  }
}

void compSubTrie(const DeltaQuadratureContext& context, const LvlMI& deltaTGMI,
                 DeltaQuadratureScratch& scratch) {
  assert(deltaTGMI.size() == context.coeffTrie.nDim());

  tools::ProjectedSubTrieBuilder(context, deltaTGMI, scratch).build();
}

void prepareDeltaTensorGrid(const DeltaQuadratureContext& context,
                            const TensorGridCTData& deltaTGData, DeltaQuadratureScratch& scratch) {
  const TensorGrid& deltaTG = deltaTGData.tensorGrid;
  const size_t nDim = deltaTG.nDim();
  const GPMI& nGPPerDim = deltaTG.getGPCntPerDim();
  const base::DataVector& nodesPerDim = deltaTG.getNodesPerDim();

  assert(nDim >= 1);
  assert(scratch.nodesPerDim.size() >= nodesPerDim.size());

  compSubTrie(context, deltaTGData.mi, scratch);

  // Maps the nodes onto the domain once, instead of every grid point (same arithmetic as
  // tools::denormalizeDataVector)
  size_t offset = 0;
  size_t projDepth = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = context.domain[dim].first;
    const double width = context.domain[dim].second - context.domain[dim].first;
    double* const nodes = scratch.nodesPerDim.data() + offset;

    for (GPCntType nodeIdx = 0; nodeIdx < nGPPerDim[dim]; nodeIdx++) {
      nodes[nodeIdx] = nodesPerDim[offset + nodeIdx] * width + lower;
    }

    scratch.gp[dim] = nodes[0];
    offset += nGPPerDim[dim];

    if (projDepth < scratch.projDims.size() && scratch.projDims[projDepth] == dim) {
      scratch.nodesOfProjDepth[projDepth] = nodes;
      scratch.gpCntOfProjDepth[projDepth] = nGPPerDim[dim];
      scratch.gpMI[projDepth] = 0;
      projDepth++;
    }
  }
}

void compRowWeights(misc::DeltaQuadratureScratch& scratch, const GPCntType nGP0) {
  double* const rowWeights = scratch.rowWeights.data();
  std::fill_n(rowWeights, nGP0, 0.0);

  for (const misc::DeltaQuadratureScratch::TrieNode& node : scratch.trieNodes[0]) {
    const double coeffSum = node.coeffSum;
    const double* const weights = node.weights;

    for (GPCntType idx0 = 0; idx0 < nGP0; idx0++) {
      rowWeights[idx0] += coeffSum * weights[idx0];
    }
  }
}

}  // namespace dsg_quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
