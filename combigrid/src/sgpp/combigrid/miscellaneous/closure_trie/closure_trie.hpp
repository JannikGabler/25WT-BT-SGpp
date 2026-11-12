// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file closure_trie.hpp
 * @brief Defines @c ClosureTrie, a prefix tree over a downwards-closed set of level multi-indices
 * that maps a multi-index to its rank by one array lookup per dimension.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Prefix tree (trie) over a downwards-closed set of level multi-indices
 * @f$\vec{i} \in \mathbb{N}^d@f$, e.g. the multi-indices of a deltas sparse grid.
 *
 * <b>Structure.</b> The root (depth 0) is the empty prefix; an inner node of depth @f$k@f$ is a
 * prefix @f$(i_{d-1}, \ldots, i_{d-k})@f$ of the multi-indices of the set, i.e. the
 * prefixes fix the dimensions from the last one downwards. The children of a node are the
 * extensions by the level @f$i_{d-k-1}@f$; the children of the nodes of depth @f$d-1@f$ are
 * the leaves, one per multi-index. Since the set is downwards closed, the levels of the children
 * of a node are @f$0, 1, \ldots, n-1@f$, so the child of level @f$i@f$ is
 * @c child(node, \vec{i}) @c = @c firstChild[node] @c + @c \vec{i}: a multi-index is found by
 * @f$d@f$ dependent array lookups, without comparisons.
 *
 * <b>Ranks.</b> The leaves are numbered @f$0, \ldots, n_{\mathrm{leaves}} - 1@f$ in lexicographic
 * order of @f$(i_{d-1}, \ldots, i_0)@f$ (dimension @f$d-1@f$ most significant): the
 * leaf id is a dense rank of the multi-index, usable as an index into per-multi-index data.
 * Inner node ids are separate (the root is @ref ROOT); the child of a node of depth @f$d-1@f$ is a
 * leaf id.
 *
 * <b>Order of the dimensions.</b> Changing @f$i_j@f$ only changes the nodes of depths
 * @f$\geq d-j@f$. A walk over multi-indices in which the low dimensions change most often (e.g.
 * the corners of a tensor grid in index order) therefore recomputes only the last few lookups.
 */
class ClosureTrie {
 public:
  /// @brief Id of the root (the empty prefix).
  static constexpr uint32_t ROOT = 0;

  /// @brief Returned by @ref findLeaf for a multi-index that is not in the set.
  static constexpr size_t NOT_FOUND = static_cast<size_t>(-1);

  /**
   * @brief Builds the trie over the level multi-indices of @p tgData.
   *
   * @param tgData Tensor grids whose level multi-indices form a downwards-closed set of pairwise
   *               distinct multi-indices with @p nDim components (in any order; coefficients and
   *               tensor grids are ignored).
   * @param nDim   Number of dimensions d > 0.
   * @throws std::invalid_argument if the multi-indices are not pairwise distinct or if the levels
   *         of the children of some prefix are not 0, 1, ..., n - 1 (which holds for every
   *         downwards-closed set; the check does not detect every set that is not downwards
   *         closed).
   * @throws std::length_error if the number of nodes does not fit into 32 bits.
   */
  ClosureTrie(std::span<const TensorGridCTData> tgData, size_t nDim);

  /// @brief Returns the number of dimensions d.
  size_t nDim() const noexcept { return nDim_; }

  /// @brief Returns the number of leaves, i.e. of multi-indices.
  size_t nLeaves() const noexcept { return leafItems.size(); }

  /// @brief Returns the number of inner nodes (including the root).
  size_t nInnerNodes() const noexcept { return firstChild_.size(); }

  /**
   * @brief Returns the child of the inner node @p node with level @p lvl (the level of dimension
   * d - 1 - depth of @p node); a leaf id if @p node has depth d - 1.
   * @note @p lvl must be less than @ref nChildren(node).
   */
  uint32_t child(const uint32_t node, const LvlType lvl) const noexcept {
    return firstChild_[node] + lvl;
  }

  /// @brief Returns the number of children of the inner node @p node.
  LvlType nChildren(const uint32_t node) const noexcept { return nChildren_[node]; }

  /// @brief Returns the array of the first children of the inner nodes (for hot loops: one
  /// lookup per dimension, see @ref child).
  const uint32_t* firstChild() const noexcept { return firstChild_.data(); }

  /**
   * @brief Returns the index (in the @c tgData the trie was built from) of the multi-index of
   * leaf @p leaf.
   */
  size_t leafItem(const size_t leaf) const noexcept { return leafItems[leaf]; }

  /**
   * @brief Returns the leaf id of @p mi, or @ref NOT_FOUND if @p mi is not in the set.
   * @param mi Multi-index with d components.
   */
  size_t findLeaf(const LvlMI& mi) const;

 private:
  /// @brief Appends the children of the inner node @p node, whose multi-indices are
  /// @p order[begin, end) (sorted), to @p nextRanges; @p depth is the depth of @p node.
  void addChildren(size_t node, size_t depth, const std::vector<size_t>& order,
                   std::span<const TensorGridCTData> tgData, size_t begin, size_t end,
                   std::vector<size_t>& nextRanges);

  size_t nDim_;                       ///< Number of dimensions.
  std::vector<uint32_t> firstChild_;  ///< Per inner node: id of its first child.
  std::vector<LvlType> nChildren_;    ///< Per inner node: number of children.
  std::vector<size_t> leafItems;      ///< Per leaf: index of its multi-index in the input.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
