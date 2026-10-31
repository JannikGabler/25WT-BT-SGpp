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
#include <limits>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
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
 * @f$\vec{\ell} \geq \vec{m}@f$ (where @f$\vec{m}@f$ is the level of the delta tensor grid),
 * projected onto the dimensions in which the delta tensor grid has more than one node (see
 * @c delta_quadrature_operator::compSubTrie), with the partial sums of the global weights, the
 * grid-point coordinates and the global weights of the current row of grid points. Its size does
 * not depend on the number of grid points of the sparse grid.
 *
 * For a vector-valued source function (m outputs), the scratch also holds the buffer of the
 * @c misc::VectorValues of the kernel: the m values at the current grid point and the m sums of
 * the delta tensor grid.
 */
struct DeltaQuadratureScratch {
  /// @brief Accumulator slot of the sums of the delta tensor grid (@c misc::VectorValues::zeroAcc).
  static constexpr std::size_t TG_SUM_ACC = 0;
  /// @brief Number of accumulator slots of the kernel.
  static constexpr std::size_t N_ACCS = 1;

  /**
   * @brief Node of the projected sub-trie.
   *
   * For a node of projected depth @c t with level @f$\ell_k@f$, where @f$k@f$ =
   * @c projDims[t], @c weights[i] is the weight of the @c i-th delta node of level @f$m_k@f$ in
   * the 1D quadrature rule of level @f$\ell_k@f$ (in dimension @f$k@f$). The children of a node
   * are a contiguous range of the next projected depth, like in the @c CTCoeffTrie.
   */
  struct TrieNode {
    const double* weights;  ///< 1D weights of the delta nodes of the node's dimension.
    std::size_t childEnd;   ///< End of the children in the next projected depth (@c 0 for leaves).
    /**
     * @brief Leaves: the projected coefficient (see @c delta_quadrature_operator::compSubTrie).
     * Inner nodes of projected depth @c t: the sum of @c weights[gpMI[t+1]] * @c coeffSum over the
     * children, i.e. the global weight with the projected depths @c 0, ..., t factored out.
     */
    double coeffSum;
  };

  /**
   * @brief Slot of the last projected depth while the projected sub-trie is built (see
   * @ref projChildBlocks): the projected coefficient accumulated so far, as the unevaluated sum
   * @c hi + @c lo (double-double, see @c delta_quadrature_operator::compSubTrie).
   */
  struct ProjLeafSlot {
    double hi;
    double lo;
  };

  /// @brief Entry of @ref projChildBlocks for a slot without children (yet).
  static constexpr std::size_t NO_BLOCK = std::numeric_limits<std::size_t>::max();

  /**
   * @brief Allocates the buffers.
   * @param nDim                     Spatial dimension.
   * @param maxTGSumOverGPCntsPerDim Upper bound on @f$\sum_k n_k@f$ across the delta tensor
   *                                 grids that will be processed.
   * @param nOutputs                 Number of outputs m of a vector-valued source function, 0 for
   *                                 a scalar source function (no @ref valueBuffer).
   */
  DeltaQuadratureScratch(const size_t nDim, const size_t maxTGSumOverGPCntsPerDim,
                         const size_t nOutputs = 0)
      : trieNodes(nDim),
        projChildBlocks(nDim),
        nodesPerDim(maxTGSumOverGPCntsPerDim),
        nodesOfProjDepth(nDim),
        gpCntOfProjDepth(nDim),
        gp(nDim),
        gpMI(nDim),
        rowWeights(maxTGSumOverGPCntsPerDim),
        valueBuffer(misc::VectorValues::bufferSize(nOutputs, N_ACCS)) {
    projDims.reserve(nDim);
  }

  /// @brief @c projDims[t]: dimension of the projected depth @c t (ascending).
  std::vector<std::size_t> projDims;
  /// @brief @c trieNodes[t]: the projected sub-trie (pruned to nodes with a leaf below them). Only
  /// the first @c projDims.size() entries are used.
  std::vector<std::vector<TrieNode>> trieNodes;
  /**
   * @brief Slots of the projected sub-trie while it is built. The slots of projected depth @c t
   * form blocks, one per slot of depth @c t - 1 (one for @c t = 0) for its children: slot @c i of
   * a block stands for the level @f$m_k + i@f$ of the dimension @f$k@f$ = @c projDims[t].
   * @c projChildBlocks[t] (for @c t < projDims.size() - 1) holds, per slot, the start of the
   * block of its children in the next projected depth, or @ref NO_BLOCK.
   */
  std::vector<std::vector<std::size_t>> projChildBlocks;
  /// @brief The slots of the last projected depth (see @ref projChildBlocks).
  std::vector<ProjLeafSlot> projLeafSlots;
  /// @brief Node positions of the delta tensor grid, mapped onto the domain (per dimension
  /// back-to-back).
  base::DataVector nodesPerDim;
  /// @brief @c nodesOfProjDepth[t]: start of the nodes of dimension @c projDims[t] in
  /// @ref nodesPerDim.
  std::vector<const double*> nodesOfProjDepth;
  /// @brief @c gpCntOfProjDepth[t]: number of nodes of dimension @c projDims[t].
  GPMI gpCntOfProjDepth;
  /// @brief Current grid point (in domain coordinates).
  base::DataVector gp;
  /// @brief @c gpMI[t]: node index of the current grid point in dimension @c projDims[t] (entry 0
  /// is unused).
  GPMI gpMI;
  /// @brief Global weights of the grid points of the current row (all node indices of
  /// dimension @c projDims[0], the other ones fixed by @ref gpMI).
  base::DataVector rowWeights;
  /// @brief Buffer of the @c misc::VectorValues of a vector-valued source function (empty for a
  /// scalar one; accumulator slot @ref TG_SUM_ACC).
  std::vector<double> valueBuffer;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
