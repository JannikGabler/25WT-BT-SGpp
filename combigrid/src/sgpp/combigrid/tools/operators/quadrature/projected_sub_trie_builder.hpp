// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file projected_sub_trie_builder.hpp
 * @brief Defines @c ProjectedSubTrieBuilder, which computes the projected sub-trie of a delta
 * tensor grid for the delta quadrature (see @c delta_quadrature_operator::compSubTrie).
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {

/**
 * @brief Builds the projected sub-trie of the tensor grids @f$\vec{\ell} \geq \vec{m}@f$ of a
 * delta tensor grid of level @f$\vec{m}@f$ (see @c delta_quadrature_operator::compSubTrie) into
 * the scratch of the calling thread.
 *
 * Two steps: a depth-first search over the coefficient trie accumulates the projected coefficients
 * in the slots of @c scratch.projLeafSlots (see @c misc::DeltaQuadratureScratch::projChildBlocks),
 * which merges the tensor grids with equal levels in the projected dimensions; then the slots with
 * a non-zero projected coefficient below them are copied, in level order, to
 * @c scratch.trieNodes.
 *
 * @note Internal to @c libsgppcombigrid (hidden visibility). As exported functions, the member
 * functions were compiled differently than in an anonymous namespace (212 of the 738 instructions
 * of the recursive @ref addChildren differed; @c micro/dsg/sub_tries 1-3 % slower); hidden, they
 * compile to the same instructions. Targets without symbol visibility (e.g. MinGW) ignore it.
 */
class [[gnu::visibility("hidden")]] ProjectedSubTrieBuilder {  // NOLINT(whitespace/braces)
 public:
  /**
   * @param context   Data of the delta quadrature call (coefficient trie and 1D weights).
   * @param deltaTGMI Level @f$\vec{m}@f$ of the delta tensor grid.
   * @param scratch   Scratch of the calling thread; its projected sub-trie and slots are
   *                  overwritten by @ref build.
   */
  ProjectedSubTrieBuilder(const misc::DeltaQuadratureContext& context, const LvlMI& deltaTGMI,
                          misc::DeltaQuadratureScratch& scratch);

  /// @brief Sets @c scratch.projDims and fills @c scratch.trieNodes (see
  /// @c delta_quadrature_operator::compSubTrie).
  void build();

 private:
  /**
   * @brief Unevaluated sum @c hi + @c lo of two doubles (double-double). The projected
   * coefficients are sums of many products @f$c_{\vec{\ell}} \prod_k w_k(\ell_k)@f$ that cancel
   * (the @f$c_{\vec{\ell}}@f$ are large integers of alternating sign); accumulated in double, their
   * round-off exceeds the one of the nested sums along the trie (results up to 9x farther from the
   * exact combination at d = 15). In double-double, they are correct to about one rounding.
   */
  struct DoubleDouble {
    double hi;
    double lo;

    /// @brief Product with @p factor; the rounding error of @c hi * @p factor is exact (FMA).
    DoubleDouble operator*(double factor) const;
  };

  /// @brief State of the depth-first search (@ref addChildren) on the current path, before a
  /// dimension @c dim.
  struct Path {
    std::size_t nextProjDepth;   ///< Number of projected dimensions < @c dim.
    std::size_t parentSlot;      ///< Slot of the deepest of them (in nextProjDepth - 1).
    LvlType projExcess;          ///< Sum of @f$\ell_k - m_k@f$ over them.
    DoubleDouble weightProduct;  ///< Product of the weights of the other dimensions < @c dim.
  };

  /// @brief Adds @p term to @p sum; the rounding error of the sum of the high parts is exact
  /// (TwoSum).
  static void addTo(misc::DeltaQuadratureScratch::ProjLeafSlot& sum, const DoubleDouble& term);

  /**
   * @brief Sets @c scratch.projDims.
   * @return @c false if @c nDim == 0 or a level of @ref deltaTGMI exceeds the maximum level of its
   *         dimension, i.e. if there is no tensor grid @f$\geq \vec{m}@f$.
   */
  bool setProjDims();

  /**
   * @brief Adds the tensor grids @f$\geq \vec{m}@f$ below the nodes [@p childBegin, @p childEnd)
   * of depth @p dim of the coefficient trie (the children of one node of depth @p dim - 1) with a
   * level @f$\geq m_{dim}@f$. Children whose @c maxSuffixLvlSum is below @p lvlSumAfterDim
   * (@f$= m_{dim+1} + \ldots + m_{d-1}@f$) have no such tensor grid below them and are skipped.
   *
   * A child of a projected dimension gets the slot of its level in the block of children of
   * @c path.parentSlot; a child of another dimension stays in @c path.parentSlot and multiplies the
   * weight product by its weight. Leaves add the weight product times their coefficient to their
   * slot.
   */
  void addChildren(std::size_t dim, std::size_t childBegin, std::size_t childEnd,
                   LvlType lvlSumAfterDim, const Path& path);

  /// @brief Returns the start of the block of children of @c path.parentSlot (of the root if
  /// @c path.nextProjDepth == 0) in the projected depth @c path.nextProjDepth; allocates it on
  /// first use.
  std::size_t childBlock(const Path& path);

  /// @brief Appends a block of empty slots of projected depth @p projDepth for the children of a
  /// slot with the excess @p projExcess and returns its start.
  std::size_t allocBlock(std::size_t projDepth, LvlType projExcess);

  /**
   * @brief Returns the number of slots of a block of projected depth @p projDepth for the children
   * of a slot whose projected prefix has the excess @p projExcess (@f$= \sum (\ell_k - m_k)@f$):
   * slot @c i stands for the level @f$m_k + i@f$ of @f$k@f$ = @c projDims[projDepth]. A tensor grid
   * @f$\vec{\ell} \geq \vec{m}@f$ below the slot has @c i @f$\leq@f$ maximum level of the dimension
   * @f$- m_k@f$ and, as @f$|\vec{\ell}|_1 \leq@f$ maximum 1-norm, @c i @f$\leq@f$ @ref slack
   * @f$-@f$ @p projExcess (which is exact for complete sparse grids).
   */
  std::size_t blockWidth(std::size_t projDepth, LvlType projExcess) const;

  /**
   * @brief Appends the slots of the block starting at @p block of projected depth @p projDepth
   * (for the children of a slot with the excess @p projExcess) that have a non-zero projected
   * coefficient below them to @c scratch.trieNodes[projDepth], each followed by its children
   * (recursively).
   * @return Whether a slot was appended.
   */
  bool appendProjectedChildren(std::size_t projDepth, std::size_t block, LvlType projExcess);

  const misc::DeltaQuadratureContext& context;  ///< See the constructor.
  const LvlMI& deltaTGMI;                       ///< See the constructor.
  misc::DeltaQuadratureScratch& scratch;        ///< See the constructor.
  /// @brief Maximum 1-norm of the levels of the tensor grids minus the one of @ref deltaTGMI.
  LvlType slack = 0;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
