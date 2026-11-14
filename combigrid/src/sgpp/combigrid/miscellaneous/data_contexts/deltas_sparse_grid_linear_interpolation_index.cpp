// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
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
/// Appends the delta nodes of level @p lvl of dimension @p dim (normalized) to @p out, sorted.
void appendSortedDeltaNodes(const SGGenInstr& genInstr, const size_t dim, const LvlType lvl,
                            std::vector<double>& out) {
  const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
  const bool withBoundary = (lvl == genInstr.getBoundaryLevelOffset());
  const size_t oldSize = out.size();
  out.resize(oldSize + tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, withBoundary));

  const std::span<double> appended = std::span<double>(out).subspan(oldSize);
  genInstr.getNodeGenFuncForDim(dim)->genDeltaNodes(lvl, lvl2GPCntFunc, appended, withBoundary);
  if (!std::ranges::is_sorted(appended)) {  // The built-in node families generate them sorted
    std::ranges::sort(appended);
  }
}

}  // namespace

DSGLinInterpIndex::DSGLinInterpIndex(const DeltasSparseGrid& deltasSparseGrid)
    : levels_(deltasSparseGrid.getLevels().getView()),
      coeffs_(deltasSparseGrid.getCoefficients()),
      trie_(levels_) {
  collectDeltaNodes(deltasSparseGrid);
  maxNewCoords_.assign(nRows(), 0);
  for (size_t dim = 0; dim < nDim(); dim++) {
    countNewCoords(dim);
  }
  computeSlotOffsets();

  for (size_t tgIdx = 0; tgIdx < coeffs_.size(); tgIdx++) {
    if (coeffs_[tgIdx] != 0) {
      ctTGs.push_back(tgIdx);
    }
  }
}

/**
 * The delta nodes are generated on the unit interval as for the deltas sparse grid (the corner
 * search runs on the normalized query point; the corners are mapped onto the domain with
 * tools::mapToDomain, as the node table of the grid).
 */
void DSGLinInterpIndex::collectDeltaNodes(const DeltasSparseGrid& deltasSparseGrid) {
  const SGGenInstr& genInstr = deltasSparseGrid.getGenInstr();
  const NodeTable& nodeTable = deltasSparseGrid.getNodeTable();

  rowOffset_.assign(1, 0);
  nodeOffset.assign(1, 0);
  for (size_t dim = 0; dim < nDim(); dim++) {
    rowOffset_.push_back(rowOffset_.back() + nodeTable.maxLvl(dim) + 1);
    for (LvlType lvl = 0; lvl <= nodeTable.maxLvl(dim); lvl++) {
      appendSortedDeltaNodes(genInstr, dim, lvl, nodes);
      nodeOffset.push_back(nodes.size());
    }
  }
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
    const LvlMIView mi = leafMI(leaf);
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
