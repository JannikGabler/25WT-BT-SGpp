// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

ClosureTrie::ClosureTrie(const std::span<const TensorGridCTData> tgData, const size_t nDim)
    : nDim_(nDim) {
  if (nDim == 0) {
    throw std::invalid_argument("ClosureTrie: The number of dimensions must be positive.");
  }
  if (tgData.size() > std::numeric_limits<uint32_t>::max()) {
    throw std::length_error("ClosureTrie: Too many multi-indices.");
  }

  // Lexicographic order with dimension d - 1 most significant: the multi-indices below a prefix
  // are contiguous and sorted by the level of the next dimension.
  std::vector<size_t> order(tgData.size());
  std::iota(order.begin(), order.end(), size_t{0});
  std::ranges::sort(order, [&](const size_t a, const size_t b) {
    const LvlMI& miA = tgData[a].mi;
    const LvlMI& miB = tgData[b].mi;
    for (size_t j = nDim; j-- > 0;) {
      if (miA[j] != miB[j]) {
        return miA[j] < miB[j];
      }
    }
    return false;
  });

  // The nodes of the current depth cover consecutive ranges of order: node i covers
  // [bounds[i], bounds[i + 1]).
  std::vector<size_t> bounds{0, order.size()};
  firstChild_.push_back(0);
  nChildren_.push_back(0);
  size_t depthBegin = ROOT;

  for (size_t depth = 0; depth < nDim; depth++) {
    std::vector<size_t> nextBounds{0};
    const size_t nNodes = bounds.size() - 1;

    for (size_t i = 0; i < nNodes; i++) {
      addChildren(depthBegin + i, depth, order, tgData, bounds[i], bounds[i + 1], nextBounds);
    }

    depthBegin += nNodes;
    bounds = std::move(nextBounds);
  }

  if (firstChild_.size() > std::numeric_limits<uint32_t>::max()) {
    throw std::length_error("ClosureTrie: Too many nodes.");
  }
}

void ClosureTrie::addChildren(const size_t node, const size_t depth,
                              const std::vector<size_t>& order,
                              const std::span<const TensorGridCTData> tgData, const size_t begin,
                              const size_t end, std::vector<size_t>& nextBounds) {
  const size_t dim = nDim_ - 1 - depth;
  const bool leaves = (depth + 1 == nDim_);
  const size_t first = leaves ? leafItems.size() : firstChild_.size();
  LvlType nGroups = 0;

  for (size_t groupBegin = begin; groupBegin < end;) {
    const LvlType lvl = tgData[order[groupBegin]].mi[dim];
    size_t groupEnd = groupBegin + 1;
    while (groupEnd < end && tgData[order[groupEnd]].mi[dim] == lvl) {
      groupEnd++;
    }

    if (lvl != nGroups) {
      throw std::invalid_argument("ClosureTrie: The multi-indices are not downwards closed.");
    }
    if (leaves) {
      if (groupEnd - groupBegin != 1) {
        throw std::invalid_argument("ClosureTrie: The multi-indices are not pairwise distinct.");
      }
      leafItems.push_back(order[groupBegin]);
    } else {
      firstChild_.push_back(0);
      nChildren_.push_back(0);
    }

    nextBounds.push_back(groupEnd);
    nGroups++;
    groupBegin = groupEnd;
  }

  firstChild_[node] = static_cast<uint32_t>(first);
  nChildren_[node] = nGroups;
}

size_t ClosureTrie::findLeaf(const LvlMI& mi) const {
  uint32_t node = ROOT;

  for (size_t dim = nDim_; dim-- > 0;) {
    if (mi[dim] >= nChildren_[node]) {
      return NOT_FOUND;
    }
    node = child(node, mi[dim]);
  }

  return node;
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
