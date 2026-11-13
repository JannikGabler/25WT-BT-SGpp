// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_global_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

DSGGlobalInterpIndex::DSGGlobalInterpIndex(const DeltasSparseGrid& deltasSparseGrid)
    : coeffTrie_(deltasSparseGrid.getTensorGrids(), deltasSparseGrid.nDim()) {
  setRowOffsets(deltasSparseGrid);

  weightLayout_.reserveSubVecs(1, nDim());
  for (size_t dim = 0; dim < nDim(); dim++) {
    addRowsOfDim(deltasSparseGrid, dim);
  }

  rowsBySize_.resize(nRows());
  std::iota(rowsBySize_.begin(), rowsBySize_.end(), size_t{0});
  std::ranges::stable_sort(rowsBySize_, [&](const size_t a, const size_t b) {
    return rowNodes[a].size() > rowNodes[b].size();
  });

  for (const TensorGridCTData& tgData : deltasSparseGrid) {
    nGridPoints_ += tgData.tensorGrid.nGP();
  }

  tgsBySize_.resize(deltasSparseGrid.nTG());
  std::iota(tgsBySize_.begin(), tgsBySize_.end(), size_t{0});
  std::ranges::stable_sort(tgsBySize_, [&](const size_t a, const size_t b) {
    return deltasSparseGrid.getTensorGrid(a).tensorGrid.nGP() >
           deltasSparseGrid.getTensorGrid(b).tensorGrid.nGP();
  });
}

void DSGGlobalInterpIndex::setRowOffsets(const DeltasSparseGrid& deltasSparseGrid) {
  const size_t nDim = deltasSparseGrid.nDim();
  std::vector<LvlType> maxLvls(nDim, 0);

  for (const TensorGridCTData& tgData : deltasSparseGrid) {
    for (size_t dim = 0; dim < nDim; dim++) {
      maxLvls[dim] = std::max(maxLvls[dim], tgData.mi[dim]);
    }
  }

  rowOffset_.assign(1, 0);
  for (size_t dim = 0; dim < nDim; dim++) {
    rowOffset_.push_back(rowOffset_.back() + maxLvls[dim] + 1);
  }
}

void DSGGlobalInterpIndex::addRowsOfDim(const DeltasSparseGrid& deltasSparseGrid,
                                        const size_t dim) {
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const LvlType boundaryLvlOffset = genInstr.getBoundaryLevelOffset();
  const NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
  const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
  const NodeIdxLvlMapFunc nodeIdxLvlMapFunc =
      nodeGenFunc->getNodeIdxMapFromDeltaTGLvlToRegularLvl(lvl2GPCntFunc);

  weightLayout_.appendSubVec(1);  // weights[dim]

  for (LvlType lvl = 0; lvl <= maxLvl(dim); lvl++) {
    // The nodes and the method of the tensor grids of level lvl in the SparseGrid operator
    const bool withBoundary = (lvl >= boundaryLvlOffset);
    const GPCntType nNodes = lvl2GPCntFunc(lvl) + (withBoundary ? 2 : 0);
    const base::DataVector nodes = nodeGenFunc->genNodes(nNodes, withBoundary);
    rowNodes.emplace_back(nodes.begin(), nodes.end());
    rowDim_.push_back(dim);
    rowMethods.push_back(nodeGenFunc->getInterpolationMethod(nNodes));
    weightStart_.push_back(deltaOrder_.size());

    for (LvlType deltaLvl = 0; deltaLvl <= lvl; deltaLvl++) {
      const GPCntType nDeltaNodes =
          tools::getDeltaNodeCnt(deltaLvl, lvl2GPCntFunc, deltaLvl == boundaryLvlOffset);

      for (GPCntType deltaNodeIdx = 0; deltaNodeIdx < nDeltaNodes; deltaNodeIdx++) {
        deltaOrder_.push_back(tools::deltas_sparse_grid_generation::mapDeltaNodeIdxToRegular(
            nDeltaNodes, nNodes, deltaLvl, lvl, deltaNodeIdx, nodeIdxLvlMapFunc,
            boundaryLvlOffset));
        assert(deltaOrder_.back() < nNodes);
      }
    }

    assert(deltaOrder_.size() - weightStart_.back() == nNodes);
    weightLayout_.appendSubVec(2, nNodes);  // weights[dim][lvl]
    maxRowSize_ = std::max<size_t>(maxRowSize_, nNodes);
  }
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
