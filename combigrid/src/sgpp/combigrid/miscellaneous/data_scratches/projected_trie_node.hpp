// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file projected_trie_node.hpp
 * @brief Defines @c ProjectedTrieNode, the node of the projected sub-trie of a delta tensor grid
 * (see @c misc::DeltaWeightedSumScratch::trieNodes).
 */
#pragma once

#include <cstddef>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Node of the projected sub-trie of a delta tensor grid (see
 * @c tools::dsg_weighted_sum::compSubTrie and @c misc::DeltaWeightedSumScratch::trieNodes).
 *
 * For a node of projected depth @c t with level @f$\ell_k@f$, where @f$k@f$ =
 * @c projDims[t], @c weights[i] is the 1D weight of the @c i-th delta node of level @f$m_k@f$
 * at level @f$\ell_k@f$ (in dimension @f$k@f$; the quadrature weight of the 1D rule or the
 * basis value of the 1D interpolant of level @f$\ell_k@f$). The children of a node
 * are a contiguous range of the next projected depth, like in the @c CTCoeffTrie.
 */
struct ProjectedTrieNode {
  const double* weights;  ///< 1D weights of the delta nodes of the node's dimension.
  std::size_t childEnd;   ///< End of the children in the next projected depth (@c 0 for leaves).
  /**
   * @brief Leaves: the projected coefficient (see @c tools::dsg_weighted_sum::compSubTrie).
   * Inner nodes of projected depth @c t: the sum of @c weights[gpMI[t+1]] * @c coeffSum over the
   * children, i.e. the global weight with the projected depths @c 0, ..., t factored out.
   */
  double coeffSum;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
