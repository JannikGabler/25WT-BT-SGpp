// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file delta_quadrature_scratch.hpp
 * @brief Per-thread scratch storage of the delta quadrature, reused across delta tensor grids.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Pre-allocated workspace of the delta quadrature for the delta tensor grid currently
 * processed by a thread (see @c delta_quadrature_operator::forEachGPWithGlobalWeight).
 *
 * Holds the sub-trie of the @c CTCoeffTrie that contains the tensor grids
 * @f$\vec{\ell} \geq \vec{m}@f$ (where @f$\vec{m}@f$ is the level of the delta tensor grid) with
 * the partial sums of the global weights, the grid-point coordinates and the global weights of
 * the current row of grid points. Its size does not depend on the number of grid points of the
 * sparse grid.
 */
struct DeltaQuadratureScratch {
  /**
   * @brief Node of the sub-trie of the tensor grids @f$\vec{\ell} \geq \vec{m}@f$.
   *
   * For a node of depth @c k with level @f$\ell_k@f$, @c weights[i] is the weight of the
   * @c i-th delta node of level @f$m_k@f$ in the 1D quadrature rule of level @f$\ell_k@f$ (in
   * dimension @c k). The children of a node are a contiguous range of the next depth, like in the
   * @c CTCoeffTrie.
   */
  struct TrieNode {
    const double* weights;  ///< 1D weights of the delta nodes of the node's dimension.
    std::size_t childEnd;   ///< End of the children in the next depth (@c 0 for leaves).
    /**
     * @brief Leaves: the CT coefficient. Inner nodes of depth @c k: the sum of
     * @c weights[gpMI[k+1]] * @c coeffSum over the children, i.e. the global weight with the
     * dimensions @c 0, ..., k factored out.
     */
    double coeffSum;
  };

  /**
   * @brief Allocates the buffers.
   * @param nDim                     Spatial dimension.
   * @param maxTGSumOverGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the delta tensor
   *                                 grids that will be processed.
   */
  DeltaQuadratureScratch(const size_t nDim, const size_t maxTGSumOverGPCntsPerDim)
      : trieNodes(nDim),
        nodesPerDim(maxTGSumOverGPCntsPerDim),
        nodesOfDim(nDim),
        gp(nDim),
        gpMI(nDim),
        rowWeights(maxTGSumOverGPCntsPerDim) {}

  /// @brief @c trieNodes[depth][idx]: the sub-trie (pruned to nodes with a leaf below them).
  std::vector<std::vector<TrieNode>> trieNodes;
  /// @brief Node positions of the delta tensor grid, mapped onto the domain (per dimension
  /// back-to-back).
  base::DataVector nodesPerDim;
  /// @brief @c nodesOfDim[dim]: start of the nodes of dimension @c dim in @ref nodesPerDim.
  std::vector<const double*> nodesOfDim;
  /// @brief Current grid point (in domain coordinates).
  base::DataVector gp;
  /// @brief Node indices of the current grid point (entry 0 is unused).
  GPMI gpMI;
  /// @brief Global weights of the grid points of the current row (all node indices of
  /// dimension 0, the other ones fixed by @ref gpMI).
  base::DataVector rowWeights;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
