// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file ct_coefficient_trie.hpp
 * @brief Defines @c CTCoeffTrie, a prefix tree over the level multi-indices of the tensor grids
 * with a non-zero combination coefficient.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Prefix tree (trie) over the level multi-indices @f$\vec{\ell}@f$ of the tensor grids
 * with a non-zero combination coefficient @f$c_{\vec{\ell}}@f$.
 *
 * <b>Structure.</b> The nodes of depth @f$k \in \{0, \ldots, d-1\}@f$ correspond to the distinct
 * prefixes @f$(\ell_0, \ldots, \ell_k)@f$ of the multi-indices; a node stores the last component
 * @f$\ell_k@f$ of its prefix. The nodes of depth @f$d-1@f$ (the leaves) correspond to the
 * multi-indices themselves and carry their coefficients. The children of the (implicit) root are
 * all nodes of depth 0.
 *
 * <b>Storage.</b> The nodes of each depth are stored contiguously, in lexicographic order of their
 * prefixes. Hence the children of a node are a contiguous range of the next depth, sorted by
 * level in ascending order: they start at the @c childEnd of the preceding node of the same depth
 * (at 0 for the first node) and end at the node's own @c childEnd (see @ref childBegin and
 * @ref childEnd).
 *
 * <b>Purpose.</b> The tensor grids @f$\vec{\ell} \geq \vec{m}@f$ (component-wise) are found by a
 * depth-first search that only descends into children with @f$\ell_k \geq m_k@f$, i.e. into
 * suffixes of the sorted child ranges, and with
 * @f$\mathrm{maxSuffixLvlSum} \geq m_{k+1} + \ldots + m_{d-1}@f$. The latter is necessary for a
 * tensor grid @f$\vec{\ell} \geq \vec{m}@f$ below the node and sufficient for complete sparse
 * grids (below a prefix @f$\vec{p}@f$, they contain a multi-index @f$\geq (\vec{p}, \vec{s})@f$
 * for every @f$\vec{s}@f$ with @f$|\vec{p}|_1 + |\vec{s}|_1 \leq L@f$); without it, the search
 * visits every prefix @f$\geq@f$ the prefix of @f$\vec{m}@f$, most of which have no tensor grid
 * @f$\geq \vec{m}@f$ below them if the levels of @f$\vec{m}@f$ lie in the last dimensions. Sums
 * over the tensor grids found of products @f$c_{\vec{\ell}} \prod_k g_k(\ell_k)@f$ can be
 * factorized along the trie (one factor per depth), which the delta quadrature uses to compute the
 * global quadrature weights.
 */
class CTCoeffTrie {
 public:
  /// @brief Node of the trie (see the class description).
  struct Node {
    LvlType lvl;  ///< Last component @f$\ell_k@f$ of the prefix of the node.
    /// Maximum of @f$\ell_{k+1} + \ldots + \ell_{d-1}@f$ over the multi-indices below the node
    /// (@c 0 for leaves). Fills the padding before @c childEnd.
    LvlType maxSuffixLvlSum;
    std::size_t childEnd;  ///< End of the children in the next depth (@c 0 for leaves).

    bool operator==(const Node& other) const = default;
  };

  /// @name Constructors
  /// @{

  /**
   * @brief Builds the trie over the level multi-indices of the tensor grids in @p tgData with a
   * non-zero coefficient. Tensor grids with coefficient 0 are skipped.
   *
   * @param tgData Tensor grids with pairwise distinct level multi-indices of @p nDim
   *               components. Neither their order nor their tensor grids matter.
   * @param nDim   Number of dimensions (the depth of the trie).
   */
  CTCoeffTrie(std::span<const TensorGridCTData> tgData, size_t nDim);

  /// @}

  /// @name Getters
  /// @{

  /// @brief Returns the number of dimensions, i.e. the depth of the trie.
  size_t nDim() const noexcept { return nDim_; }

  /// @brief Returns the number of leaves, i.e. of tensor grids with a non-zero coefficient.
  size_t nLeaves() const noexcept { return leafCoeffs_.size(); }

  /**
   * @brief Returns the nodes of depth @p depth (ordered lexicographically by their prefixes).
   * @param depth Depth in @c [0, nDim()).
   */
  std::span<const Node> nodesOfDepth(const size_t depth) const noexcept { return nodes[depth]; }

  /**
   * @brief Returns the coefficients of the leaves: entry @c i belongs to the leaf
   * @c nodesOfDepth(nDim()-1)[i].
   */
  std::span<const double> leafCoeffs() const noexcept { return leafCoeffs_; }

  /**
   * @brief Returns the index (in the depth @p depth + 1) of the first child of the node
   * @c nodesOfDepth(depth)[idx].
   */
  size_t childBegin(const size_t depth, const size_t idx) const noexcept {
    return idx == 0 ? 0 : nodes[depth][idx - 1].childEnd;
  }

  /**
   * @brief Returns the index (in the depth @p depth + 1) past the last child of the node
   * @c nodesOfDepth(depth)[idx].
   */
  size_t childEnd(const size_t depth, const size_t idx) const noexcept {
    return nodes[depth][idx].childEnd;
  }

  /// @}

 private:
  /// @name Attributes
  /// @{

  size_t nDim_;                   ///< Number of dimensions (depth of the trie).
  FlatNestedVec<Node, 2> nodes;   ///< @c nodes[depth][idx]: nodes of each depth.
  std::vector<double> leafCoeffs_;  ///< Coefficients of the leaves (see @ref leafCoeffs).

  /// @}
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
