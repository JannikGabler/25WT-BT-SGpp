// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file projected_sub_trie_builder.hpp
 * @brief Defines @c ProjectedSubTrieBuilder, which computes the projected sub-trie of a delta
 * tensor grid for the global weights of the deltas sparse grid operators (see
 * @c tools::dsg_weighted_sum::compSubTrie).
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_weighted_sum_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/numerics/double_double.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {

/**
 * @brief Builds the projected sub-trie of the tensor grids @f$\vec{\ell} \geq \vec{m}@f$ of a
 * delta tensor grid of level @f$\vec{m}@f$ (see @c tools::dsg_weighted_sum::compSubTrie) into
 * the scratch of the calling thread.
 *
 * Two steps: a depth-first search over the coefficient trie accumulates the projected coefficients
 * in the slots of @c scratch.projLeafSlots (see @c misc::DeltaWeightedSumScratch::projChildBlocks),
 * which merges the tensor grids with equal levels in the projected dimensions; then the slots with
 * a non-zero projected coefficient below them are copied, in level order, to
 * @c scratch.trieNodes.
 *
 * The projected coefficients are sums of many products @f$c_{\vec{\ell}} \prod_k w_k(\ell_k)@f$
 * that cancel (the @f$c_{\vec{\ell}}@f$ are large integers of alternating sign); accumulated in
 * double, their round-off exceeds the one of the nested sums along the trie. They are therefore
 * accumulated as @c misc::DoubleDouble, which makes them correct to about one rounding.
 *
 * @note Internal to @c libsgppcombigrid (hidden visibility). As exported functions, the member
 * functions were compiled differently than in an anonymous namespace; hidden, they
 * compile to the same instructions. Targets without symbol visibility (e.g. MinGW) ignore it.
 */
class [[gnu::visibility("hidden")]] ProjectedSubTrieBuilder {  // NOLINT(whitespace/braces)
 public:
  /**
   * @param context   Data of the operator call (coefficient trie and 1D weights).
   * @param deltaTGMI Level @f$\vec{m}@f$ of the delta tensor grid.
   * @param scratch   Scratch of the calling thread; its projected sub-trie and slots are
   *                  overwritten by @ref build.
   */
  ProjectedSubTrieBuilder(const misc::GenericDSGOperatorContext& context, const LvlMI& deltaTGMI,
                          misc::DeltaWeightedSumScratch& scratch);

  /// @brief Sets @c scratch.projDims and fills @c scratch.trieNodes (see
  /// @c tools::dsg_weighted_sum::compSubTrie).
  void build();

 private:
  /// @brief 1D weights of one dimension, per level (@c context.weights1D[dim]).
  using Weights1DOfDim = misc::FlatNestedVec<double, 3>::const_entry_type;

  /// @brief State of the depth-first search (@ref addChildren) on the current path, before a
  /// dimension @c dim.
  struct Path {
    std::size_t nextProjDepth;         ///< Number of projected dimensions < @c dim.
    std::size_t parentSlot;            ///< Slot of the deepest of them (in nextProjDepth - 1).
    LvlType projExcess;                ///< Sum of @f$\ell_k - m_k@f$ over them.
    misc::DoubleDouble weightProduct;  ///< Product of the weights of the other dimensions < @c dim.
  };

  /**
   * @brief Returns the index of the first weight of the delta nodes of level @p deltaLvl in
   * @c weights1DOfDim[lvl] (for every @c lvl @f$\geq@f$ @p deltaLvl): they follow the ones of the
   * levels @c 0, ..., @p deltaLvl - 1.
   */
  static std::size_t deltaNodesStart(const Weights1DOfDim& weights1DOfDim, LvlType deltaLvl);

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

  const misc::GenericDSGOperatorContext& context;  ///< See the constructor.
  const LvlMI& deltaTGMI;                          ///< See the constructor.
  misc::DeltaWeightedSumScratch& scratch;          ///< See the constructor.
  /// @brief Maximum 1-norm of the levels of the tensor grids minus the one of @ref deltaTGMI.
  LvlType slack = 0;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
