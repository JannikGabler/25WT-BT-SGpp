// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
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

double processDeltaTensorGrid(const DeltaQuadratureContext& context,
                              const TensorGridCTData& deltaTGData,
                              DeltaQuadratureScratch& scratch) {
  const SourceFunc& sourceFunc = context.sourceFunc;
  double result = 0.0;

  forEachGPWithGlobalWeight(context, deltaTGData, scratch,
                            [&](const base::DataVector& gp, const double globalWeight) {
                              result += globalWeight * sourceFunc.evaluate(gp);
                            });

  return result;
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

void updateCoeffSums(misc::DeltaQuadratureScratch& scratch, const size_t maxDepth) {
  for (size_t depth = maxDepth; depth >= 1; depth--) {
    const GPCntType nodeIdx = scratch.gpMI[depth];
    const misc::DeltaQuadratureScratch::TrieNode* const children = scratch.trieNodes[depth].data();
    size_t childIdx = 0;

    for (misc::DeltaQuadratureScratch::TrieNode& parent : scratch.trieNodes[depth - 1]) {
      double sum = 0.0;

      for (; childIdx < parent.childEnd; childIdx++) {
        sum += children[childIdx].weights[nodeIdx] * children[childIdx].coeffSum;
      }

      parent.coeffSum = sum;
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
