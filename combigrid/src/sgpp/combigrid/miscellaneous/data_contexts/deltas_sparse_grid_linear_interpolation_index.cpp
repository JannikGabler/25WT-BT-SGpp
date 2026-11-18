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
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/tools/collections/vector_bytes.hpp>
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

/// Appends @p nodes to @p out, sorted ascending.
void appendSorted(const std::span<const double> nodes, std::vector<double>& out) {
  const size_t oldSize = out.size();
  out.insert(out.end(), nodes.begin(), nodes.end());

  const std::span<double> appended = std::span<double>(out).subspan(oldSize);
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
  leafCoeffs_.resize(nLeaves());
  for (size_t leaf = 0; leaf < nLeaves(); leaf++) {
    leafCoeffs_[leaf] = static_cast<double>(coeffs_[trie_.leafItem(leaf)]);
  }

  dimFiberBegin_.assign(1, 0);
  fiberBegin_.assign(1, 0);
  fiberBlocks_.reserve(nDim() * nLeaves());
  for (size_t dim = 0; dim < nDim(); dim++) {
    collectFibers(dim);
    dimFiberBegin_.push_back(fiberShift_.size());
  }
}

/**
 * The delta nodes are the unit nodes of the grid (the corner search runs on the normalized query
 * point; the corners are mapped onto the domain with tools::mapToDomain, as the domain nodes of
 * the grid).
 */
size_t DSGLinInterpIndex::memoryBytes() const noexcept {
  using tools::vectorBytes;
  return sizeof(*this) + trie_.heapBytes() + vectorBytes(rowOffset_) + vectorBytes(nodeOffset) +
         vectorBytes(nodes) + vectorBytes(maxNewCoords_) + vectorBytes(slotOffset_) +
         vectorBytes(ctTGs) + vectorBytes(leafCoeffs_) + vectorBytes(dimFiberBegin_) +
         vectorBytes(fiberBegin_) + vectorBytes(fiberBlocks_) + vectorBytes(fiberShift_);
}

void DSGLinInterpIndex::collectDeltaNodes(const DeltasSparseGrid& deltasSparseGrid) {
  const NodeTable& unitNodeTable = deltasSparseGrid.getUnitNodeTable();

  rowOffset_.assign(1, 0);
  nodeOffset.assign(1, 0);
  for (size_t dim = 0; dim < nDim(); dim++) {
    rowOffset_.push_back(rowOffset_.back() + unitNodeTable.maxLvl(dim) + 1);
    for (LvlType lvl = 0; lvl <= unitNodeTable.maxLvl(dim); lvl++) {
      appendSorted(unitNodeTable.nodeSpan(dim, lvl), nodes);
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
      // A level without new coordinates keeps one slot (the transform of the slot weights stores
      // the intermediate values of every multi-index in the first slot of its block)
      blockSize *= std::max<size_t>(maxNewCoords_[row(dim, mi[dim])], 1);
    }
    slotOffset_.push_back(slotOffset_.back() + blockSize);
  }
}

/**
 * The fibers start at the leaves of level 0 in @p dim (in leaf order); the next leaf of a fiber is
 * found in the trie (d lookups).
 */
void DSGLinInterpIndex::collectFibers(const size_t dim) {
  LvlMI mi(nDim());

  for (size_t leaf = 0; leaf < nLeaves(); leaf++) {
    const LvlMIView leafLevels = leafMI(leaf);
    if (leafLevels[dim] != 0) {
      continue;
    }
    std::copy(leafLevels.begin(), leafLevels.end(), mi.begin());
    fiberShift_.push_back(nMultiDimsBelow(leafLevels, dim));

    for (size_t next = leaf; next != ClosureTrie::NOT_FOUND; next = trie_.findLeaf(mi)) {
      // Dimension 0 is the last one of the trie walk: its fibers are runs of consecutive leaves
      assert(dim != 0 || next == fiberBlocks_.size());
      fiberBlocks_.push_back(slotOffset_[next]);
      mi[dim]++;
    }
    fiberBegin_.push_back(fiberBlocks_.size());
  }
}

uint8_t DSGLinInterpIndex::nMultiDimsBelow(const LvlMIView mi, const size_t dim) const {
  uint8_t count = 0;
  for (size_t j = 0; j < dim; j++) {
    count += (maxNewCoords_[row(j, mi[j])] == 2);
  }
  return count;
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
