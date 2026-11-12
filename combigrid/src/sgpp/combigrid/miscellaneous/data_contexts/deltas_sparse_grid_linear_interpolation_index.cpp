// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

namespace {

/// Maximum number (at most 2) of the values of @p delta (sorted) that are not in @p known
/// (sorted) and lie in the same gap between two consecutive values of @p known. One linear walk
/// over both arrays (the arrays of the high levels are large).
uint8_t maxNewInOneGap(const std::vector<double>& known, const std::span<const double> delta) {
  size_t maxCnt = 0;
  size_t cnt = 0;
  size_t lastGap = known.size() + 1;
  size_t gap = 0;  // Index of the first value of known not less than the current delta value

  for (const double value : delta) {
    while (gap < known.size() && known[gap] < value) {
      gap++;
    }
    if (gap < known.size() && known[gap] == value) {
      continue;
    }
    cnt = (gap == lastGap) ? cnt + 1 : 1;
    lastGap = gap;
    maxCnt = std::max(maxCnt, cnt);
  }

  return static_cast<uint8_t>(std::min<size_t>(maxCnt, 2));
}

/// Appends the nodes of dimension @p dim of @p tg to @p out, sorted ascending.
void appendSortedNodesOfDim(const TensorGrid& tg, const size_t dim, std::vector<double>& out) {
  const GPMI& gpCnts = tg.getGPCntPerDim();
  size_t begin = 0;
  for (size_t k = 0; k < dim; k++) {
    begin += gpCnts[k];
  }

  const auto first = tg.getNodesPerDim().begin() + static_cast<std::ptrdiff_t>(begin);
  const size_t oldSize = out.size();
  out.insert(out.end(), first, first + gpCnts[dim]);
  const auto appended = out.begin() + static_cast<std::ptrdiff_t>(oldSize);
  if (!std::is_sorted(appended, out.end())) {  // The built-in node families generate them sorted
    std::sort(appended, out.end());
  }
}

}  // namespace

DSGLinInterpIndex::DSGLinInterpIndex(const DeltasSparseGrid& deltasSparseGrid)
    : tensorGrids(deltasSparseGrid.getTensorGrids()), trie_(tensorGrids, deltasSparseGrid.nDim()) {
  collectDeltaNodes();
  maxNewCoords_.assign(nRows(), 0);
  for (size_t dim = 0; dim < nDim(); dim++) {
    countNewCoords(dim);
  }
  computeSlotOffsets();

  for (const TensorGridCTData& tgData : tensorGrids) {
    if (tgData.coefficient != 0) {
      ctTGs.push_back(&tgData);
    }
  }
}

void DSGLinInterpIndex::collectDeltaNodes() {
  const std::vector<const TensorGrid*> rowTGs = findRowTensorGrids();
  size_t nNodes = 0;
  for (size_t dim = 0; dim < nDim(); dim++) {
    for (size_t r = rowOffset_[dim]; r < rowOffset_[dim + 1]; r++) {
      nNodes += rowTGs[r]->getGPCntPerDim()[dim];
    }
  }

  nodes.reserve(nNodes);
  nodeOffset.assign(1, 0);
  for (size_t dim = 0; dim < nDim(); dim++) {
    for (size_t r = rowOffset_[dim]; r < rowOffset_[dim + 1]; r++) {
      appendSortedNodesOfDim(*rowTGs[r], dim, nodes);
      nodeOffset.push_back(nodes.size());
    }
  }
}

std::vector<const TensorGrid*> DSGLinInterpIndex::findRowTensorGrids() {
  const size_t nDim = this->nDim();
  std::vector<LvlType> maxLvls(nDim, 0);
  for (const TensorGridCTData& tgData : tensorGrids) {
    for (size_t dim = 0; dim < nDim; dim++) {
      maxLvls[dim] = std::max(maxLvls[dim], tgData.mi[dim]);
    }
  }

  // The delta nodes of level lvl in dimension dim are those of the tensor grid lvl * e_dim
  rowOffset_.assign(1, 0);
  std::vector<const TensorGrid*> rowTGs;
  LvlMI mi(nDim, 0);
  for (size_t dim = 0; dim < nDim; dim++) {
    rowOffset_.push_back(rowOffset_.back() + maxLvls[dim] + 1);
    for (LvlType lvl = 0; lvl <= maxLvls[dim]; lvl++) {
      mi[dim] = lvl;
      const size_t leaf = trie_.findLeaf(mi);
      assert(leaf != ClosureTrie::NOT_FOUND);
      rowTGs.push_back(&tensorGrids[trie_.leafItem(leaf)].tensorGrid);
    }
    mi[dim] = 0;
  }
  return rowTGs;
}

void DSGLinInterpIndex::countNewCoords(const size_t dim) {
  // The nodes of the levels below the current one, together with the coordinates 0 and 1 that
  // bound the brackets of query coordinates outside the nodes
  std::vector<double> known{0.0, 1.0};
  std::vector<double> merged;
  const size_t nNodesOfDim = nodeOffset[row(dim, maxLvl(dim)) + 1] - nodeOffset[row(dim, 0)];
  known.reserve(nNodesOfDim + 2);
  merged.reserve(nNodesOfDim + 2);

  for (LvlType lvl = 0; lvl <= maxLvl(dim); lvl++) {
    const size_t r = row(dim, lvl);
    const std::span<const double> delta = deltaNodes(r);
    maxNewCoords_[r] = (lvl == 0) ? 2 : maxNewInOneGap(known, delta);

    merged.clear();
    std::merge(known.begin(), known.end(), delta.begin(), delta.end(), std::back_inserter(merged));
    merged.erase(std::unique(merged.begin(), merged.end()), merged.end());
    known.swap(merged);
  }
}

void DSGLinInterpIndex::computeSlotOffsets() {
  slotOffset_.assign(1, 0);
  slotOffset_.reserve(nLeaves() + 1);

  for (size_t leaf = 0; leaf < nLeaves(); leaf++) {
    const LvlMI& mi = leafMI(leaf);
    size_t blockSize = 1;
    for (size_t dim = 0; dim < nDim(); dim++) {
      blockSize *= maxNewCoords_[row(dim, mi[dim])];
    }
    slotOffset_.push_back(slotOffset_.back() + blockSize);
  }
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
