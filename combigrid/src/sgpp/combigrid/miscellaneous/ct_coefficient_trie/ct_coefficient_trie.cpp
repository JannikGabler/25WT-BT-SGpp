// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <numeric>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

namespace {

/**
 * Copies the levels and coefficients of the multi-indices with a non-zero coefficient, in input
 * order. The levels are stored column by column (@c lvls[dim * n + i] is the level of the i-th one
 * in dimension dim, n = @c coeffs.size()), so that partitioning by one dimension reads one
 * contiguous column instead of every nDim-th entry of the row-major input. @p maxLvls[dim] becomes
 * the maximum level in dimension dim.
 */
void copyNonZeroCoeffTGs(const LvlMIVecView levels, const std::span<const CTCoeffType> coefficients,
                         std::vector<LvlType>& lvls, std::vector<double>& coeffs,
                         std::vector<LvlType>& maxLvls) {
  const size_t nDim = levels.nDim();
  const size_t n = static_cast<size_t>(
      std::ranges::count_if(coefficients, [](const CTCoeffType c) { return c != 0; }));
  const LvlType* const input = levels.data();

  lvls.resize(nDim * n);
  coeffs.reserve(n);
  maxLvls.assign(nDim, 0);

  for (size_t i = 0; i < coefficients.size(); i++) {
    if (coefficients[i] != 0) {
      for (size_t dim = 0; dim < nDim; dim++) {
        const LvlType lvl = input[i * nDim + dim];
        lvls[dim * n + coeffs.size()] = lvl;
        maxLvls[dim] = std::max(maxLvls[dim], lvl);
      }

      coeffs.push_back(static_cast<double>(coefficients[i]));
    }
  }
}

/**
 * Appends the nodes of depth @p depth to @p nodes (whose last sub-vector is nodes[depth]) and sets
 * the @c childEnd of the nodes of depth @p depth - 1.
 *
 * On entry, @p order lists the multi-indices sorted lexicographically by their components
 * 0, ..., depth - 1, and the j-th node of depth - 1 (the root for depth 0) contains the ones in
 * [rangeStarts[j], rangeStarts[j + 1]). Each range is partitioned stably by the level in dimension
 * @p depth (counting sort), which yields the children of the node in ascending order of their
 * levels. On exit, @p order and @p rangeStarts describe depth in the same way.
 */
void appendNodesOfDepth(const size_t depth, const std::span<const LvlType> lvlsOfDepth,
                        const LvlType maxLvl, FlatNestedVec<CTCoeffTrie::Node, 2>& nodes,
                        std::vector<size_t>& order, std::vector<size_t>& rangeStarts) {
  std::vector<size_t> nextOrder(order.size());
  std::vector<size_t> nextRangeStarts;
  std::vector<size_t> counts(maxLvl + 1);  // Number, then write position, per level
  const size_t nNodesBefore = nodes.size();

  for (size_t parent = 0; parent + 1 < rangeStarts.size(); parent++) {
    const size_t begin = rangeStarts[parent];
    const size_t end = rangeStarts[parent + 1];

    std::ranges::fill(counts, size_t{0});
    for (size_t i = begin; i < end; i++) counts[lvlsOfDepth[order[i]]]++;

    for (size_t lvl = 0, writeIdx = begin; lvl <= maxLvl; lvl++) {
      if (counts[lvl] > 0) {
        nodes.push_back(CTCoeffTrie::Node{static_cast<LvlType>(lvl), 0, 0});
        nextRangeStarts.push_back(writeIdx);
        writeIdx += std::exchange(counts[lvl], writeIdx);
      }
    }

    for (size_t i = begin; i < end; i++) nextOrder[counts[lvlsOfDepth[order[i]]]++] = order[i];

    if (depth > 0) {
      nodes[depth - 1][parent].childEnd = nodes.size() - nNodesBefore;
    }
  }

  nextRangeStarts.push_back(order.size());
  order = std::move(nextOrder);
  rangeStarts = std::move(nextRangeStarts);
}

/// Sets the @c maxSuffixLvlSum of all nodes, from the leaves (0) upwards.
void compMaxSuffixLvlSums(FlatNestedVec<CTCoeffTrie::Node, 2>& nodes, const size_t nDim) {
  for (size_t depth = nDim - 1; depth-- > 0;) {
    const std::span<const CTCoeffTrie::Node> children = nodes[depth + 1];
    size_t childIdx = 0;

    for (CTCoeffTrie::Node& parent : nodes[depth]) {
      for (; childIdx < parent.childEnd; childIdx++) {
        parent.maxSuffixLvlSum = std::max(
            parent.maxSuffixLvlSum,
            static_cast<LvlType>(children[childIdx].lvl + children[childIdx].maxSuffixLvlSum));
      }
    }
  }
}

}  // namespace

CTCoeffTrie::CTCoeffTrie(const LvlMIVecView levels, const std::span<const CTCoeffType> coefficients)
    : nDim_(levels.nDim()) {
  assert(levels.size() == coefficients.size());

  const size_t nDim = nDim_;
  std::vector<LvlType> lvls;
  std::vector<double> coeffs;
  std::vector<LvlType> maxLvls;
  copyNonZeroCoeffTGs(levels, coefficients, lvls, coeffs, maxLvls);
  const size_t n = coeffs.size();

  // Builds the trie depth by depth like an MSD radix sort, which also sorts the multi-indices
  // lexicographically (dimension 0 most significant): O(n * nDim) instead of O(n log n) comparisons
  // of multi-indices.
  std::vector<size_t> order(n);
  std::iota(order.begin(), order.end(), size_t{0});
  std::vector<size_t> rangeStarts{0};  // The root contains all multi-indices
  if (n > 0) rangeStarts.push_back(n);

  nodes.reserveSubVecs(1, nDim);

  for (size_t depth = 0; depth < nDim; depth++) {
    nodes.appendSubVec(1);  // nodes[depth]
    appendNodesOfDepth(depth, std::span<const LvlType>(lvls).subspan(depth * n, n), maxLvls[depth],
                       nodes, order, rangeStarts);
  }

  assert((nDim == 0 || nodes[nDim - 1].size() == n) &&
         "The level multi-indices have to be pairwise distinct");

  if (nDim > 0) {
    compMaxSuffixLvlSums(nodes, nDim);
  }

  // order is the lexicographic order of the multi-indices, i.e. the one of the leaves
  leafCoeffs_.resize(n);
  for (size_t leaf = 0; leaf < n; leaf++) leafCoeffs_[leaf] = coeffs[order[leaf]];
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
