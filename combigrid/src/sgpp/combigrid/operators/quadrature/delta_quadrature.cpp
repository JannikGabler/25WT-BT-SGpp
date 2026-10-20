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
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/delta_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::CTCoeffTrie;
using sgpp::combigrid::misc::DeltaQuadratureContext;
using sgpp::combigrid::misc::DeltaQuadratureScratch;
using sgpp::combigrid::misc::FlatNestedVec;

namespace sgpp {
namespace combigrid {

double quadratureDelta(const SourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid) {
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const size_t nDim = deltasSparseGrid.nDim();

  const FlatNestedVec<double, 3> weights1D =
      delta_quadrature_operator::comp1DWeights(deltasSparseGrid);
  const CTCoeffTrie coeffTrie(deltasSparseGrid.getTensorGrids(), nDim);

  const DeltaQuadratureContext context{.deltasSparseGrid = deltasSparseGrid,
                                       .sourceFunc = sourceFunc,
                                       .domain = genInstr.getDomain(),
                                       .weights1D = weights1D,
                                       .coeffTrie = coeffTrie};

  double result = 0.0;

#pragma omp parallel reduction(+ : result)
  {
    DeltaQuadratureScratch scratch(nDim, deltasSparseGrid.getMaxTGSumOverGPCntsPerDim());

    // The delta tensor grids differ in size by orders of magnitude. Dynamic scheduling was up to
    // 20% faster than guided scheduling (which hands out large chunks first) for large sparse
    // grids; processing them largest-first gave no further gain.
#pragma omp for schedule(dynamic) nowait
    for (const TensorGridCTData& tgData : deltasSparseGrid) {
      result += delta_quadrature_operator::processDeltaTensorGrid(context, tgData, scratch);
    }
  }

  return genInstr.getVolumeOfDomain() * result;
}

/******************
Internal operations
******************/

namespace delta_quadrature_operator {

namespace {

/**
 * Appends the children of a node of depth dim - 1 (the nodes [childBegin, childEnd) of depth dim
 * of the coefficient trie) with a level >= deltaTGMI[dim] to scratch.trieNodes[dim], each followed
 * by its own sub-trie (recursively). Children without a leaf below them are removed again.
 * Returns whether a child was appended.
 */
bool appendSubTrieChildren(const DeltaQuadratureContext& context, const LvlMI& deltaTGMI,
                           DeltaQuadratureScratch& scratch, const size_t dim,
                           const size_t childBegin, const size_t childEnd) {
  const CTCoeffTrie& coeffTrie = context.coeffTrie;
  const std::span<const CTCoeffTrie::Node> children =
      coeffTrie.nodesOfDepth(dim).subspan(childBegin, childEnd - childBegin);
  const bool isLeafDepth = (dim + 1 == coeffTrie.nDim());

  // The children are sorted by level, so the ones with a level >= deltaTGMI[dim] are a suffix
  const LvlType deltaLvl = deltaTGMI[dim];
  const auto firstChild =
      std::ranges::lower_bound(children, deltaLvl, {}, &CTCoeffTrie::Node::lvl);

  const auto weights1DOfDim = context.weights1D[dim];
  // The delta nodes of level deltaLvl follow the ones of the levels 0, ..., deltaLvl - 1
  const size_t deltaNodesStart = (deltaLvl == 0) ? 0 : weights1DOfDim[deltaLvl - 1].size();

  std::vector<DeltaQuadratureScratch::TrieNode>& subTrieNodes = scratch.trieNodes[dim];
  bool appended = false;

  for (auto child = firstChild; child != children.end(); ++child) {
    const size_t childIdx = childBegin + static_cast<size_t>(child - children.begin());
    const double* const weights = weights1DOfDim[child->lvl].data() + deltaNodesStart;

    if (isLeafDepth) {
      subTrieNodes.push_back({weights, 0, coeffTrie.leafCoeffs()[childIdx]});
      appended = true;
    } else {
      subTrieNodes.push_back({weights, 0, 0.0});

      if (appendSubTrieChildren(context, deltaTGMI, scratch, dim + 1,
                                coeffTrie.childBegin(dim, childIdx), child->childEnd)) {
        subTrieNodes.back().childEnd = scratch.trieNodes[dim + 1].size();
        appended = true;
      } else {  // No tensor grid >= deltaTGMI below the child
        subTrieNodes.pop_back();
      }
    }
  }

  return appended;
}

}  // namespace

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
          const GPCntType nodeIdx =
              mapNodeIdx(nDeltaNodes, nNodes, deltaLvl, lvl, deltaNodeIdx,
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

  for (std::vector<DeltaQuadratureScratch::TrieNode>& nodes : scratch.trieNodes) {
    nodes.clear();
  }

  if (context.coeffTrie.nDim() > 0) {
    appendSubTrieChildren(context, deltaTGMI, scratch, 0, 0,
                          context.coeffTrie.nodesOfDepth(0).size());
  }
}

void prepareDeltaTensorGrid(const DeltaQuadratureContext& context,
                            const TensorGridCTData& deltaTGData,
                            DeltaQuadratureScratch& scratch) {
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

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = context.domain[dim].first;
    const double width = context.domain[dim].second - context.domain[dim].first;
    double* const nodes = scratch.nodesPerDim.data() + offset;

    for (GPCntType nodeIdx = 0; nodeIdx < nGPPerDim[dim]; nodeIdx++) {
      nodes[nodeIdx] = nodesPerDim[offset + nodeIdx] * width + lower;
    }

    scratch.nodesOfDim[dim] = nodes;
    scratch.gpMI[dim] = 0;
    scratch.gp[dim] = nodes[0];
    offset += nGPPerDim[dim];
  }
}

}  // namespace delta_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp
