// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file dsg_lin_interp_index.hpp
 * @brief Defines @c DSGLinInterpIndex, the query-independent data of the linear interpolation
 * operator on a deltas sparse grid (built once per grid, on first use).
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

class DeltasSparseGrid;

namespace misc {

/**
 * @brief Query-independent data of the linear interpolation on a deltas sparse grid: how the
 * corners of all combination tensor grids are identified with slots of a per-call value store.
 *
 * <b>Corner coordinates.</b> The tensor grid of level @f$\vec{\ell}@f$ has the nodes
 * @f$N_j(\ell_j) = \bigcup_{\lambda \leq \ell_j} D_j(\lambda)@f$ in dimension j, where
 * @f$D_j(\lambda)@f$ are the delta nodes of level @f$\lambda@f$ (nested nodes; the deltas sparse
 * grid holds them in its tensor grid of level @f$\lambda \vec{e}_j@f$). For a query coordinate
 * @f$x_j@f$, the bracketing coordinates of the levels @f$0, 1, \ldots@f$ form a chain: each one
 * appears first at some level @f$\lambda@f$, at most @ref maxNewCoords of them per level (2 at
 * level 0; at level @f$\lambda > 0@f$ the number of delta nodes in a gap of
 * @f$N_j(\lambda - 1) \cup \{0, 1\}@f$, at most 2, i.e. 1 for the doubling growth). A corner
 * coordinate is therefore identified by its level of first appearance @f$\lambda_j@f$ and its
 * index @f$s_j@f$ among the coordinates new at that level.
 *
 * <b>Slots.</b> A corner of the tensor grid @f$\vec{\ell}@f$ is identified by
 * @f$(\vec{\lambda}, \vec{s})@f$ with @f$\vec{\lambda} \leq \vec{\ell}@f$, so @f$\vec{\lambda}@f$
 * lies in the downwards closure, i.e. is a multi-index of the deltas sparse grid. The store has
 * one block of slots per multi-index @f$\vec{\lambda}@f$ (in the order of the leaves of
 * @ref trie), with one slot per combination of the @f$s_j@f$ (@f$\prod_j@f$ maxNewCoords(j,
 * @f$\lambda_j@f$) slots; @f$s_0@f$ least significant, dimensions with one new coordinate
 * contribute no bit). The slot of a corner is @c slotOffset(leaf) + sub-index, where the leaf is
 * found by the trie walk over @f$\lambda_{d-1}, \ldots, \lambda_0@f$.
 *
 * <b>Rows.</b> The per-(dimension, level) data are stored in rows: row(j, l) = rowOffset(j) + l
 * for l = 0, ..., maxLvl(j).
 */
class DSGLinInterpIndex {
 public:
  /**
   * @brief Builds the index of @p deltasSparseGrid. The grid must outlive the index.
   * @param deltasSparseGrid Deltas sparse grid (nested nodes, d > 0).
   */
  explicit DSGLinInterpIndex(const DeltasSparseGrid& deltasSparseGrid);

  /// @brief Returns the number of dimensions d.
  size_t nDim() const noexcept { return trie_.nDim(); }

  /// @brief Returns the trie over the multi-indices of the deltas sparse grid (one leaf per
  /// multi-index, one block of slots per leaf).
  const ClosureTrie& trie() const noexcept { return trie_; }

  /// @brief Returns the maximum level of dimension @p dim over the multi-indices.
  LvlType maxLvl(const size_t dim) const noexcept {
    return static_cast<LvlType>(rowOffset_[dim + 1] - rowOffset_[dim] - 1);
  }

  /// @brief Returns the row of (dimension @p dim, level @p lvl).
  size_t row(const size_t dim, const LvlType lvl) const noexcept { return rowOffset_[dim] + lvl; }

  /// @brief Returns the number of rows (sum of maxLvl(j) + 1 over the dimensions).
  size_t nRows() const noexcept { return rowOffset_.back(); }

  /// @brief Returns the delta nodes of the (dimension, level) of row @p row, sorted ascending
  /// (normalized; as in the deltas sparse grid).
  std::span<const double> deltaNodes(const size_t row) const noexcept {
    return std::span<const double>(nodes).subspan(nodeOffset[row],
                                                  nodeOffset[row + 1] - nodeOffset[row]);
  }

  /// @brief Returns the maximum number (0, 1 or 2) of corner coordinates that appear first at the
  /// (dimension, level) of row @p row (see the class description).
  uint8_t maxNewCoords(const size_t row) const noexcept { return maxNewCoords_[row]; }

  /// @brief Returns the slot offsets of the leaves of @ref trie (@ref nLeaves + 1 entries, the
  /// last one is @ref nSlots).
  const size_t* slotOffsets() const noexcept { return slotOffset_.data(); }

  /// @brief Returns the number of slots of the store.
  size_t nSlots() const noexcept { return slotOffset_.back(); }

  /// @brief Returns the number of leaves (multi-indices of the deltas sparse grid).
  size_t nLeaves() const noexcept { return trie_.nLeaves(); }

  /// @brief Returns the multi-index of leaf @p leaf.
  const LvlMI& leafMI(const size_t leaf) const noexcept {
    return tensorGrids[trie_.leafItem(leaf)].mi;
  }

  /// @brief Returns the tensor grids of the deltas sparse grid with a non-zero combination
  /// coefficient (the tensor grids of the combination technique), in the grid's order.
  std::span<const TensorGridCTData* const> combinationTGs() const noexcept { return ctTGs; }

 private:
  /// @brief Sets the rows and copies the sorted delta nodes of every (dimension, level).
  void collectDeltaNodes();

  /// @brief Sets the row offsets (the maximum levels) and returns, per row (dimension j, level
  /// l), the tensor grid of level l e_j, whose nodes in dimension j are the delta nodes of l.
  std::vector<const TensorGrid*> findRowTensorGrids();

  /// @brief Sets @ref maxNewCoords_ of the rows of dimension @p dim.
  void countNewCoords(size_t dim);

  /// @brief Sets the slot offsets of the leaves.
  void computeSlotOffsets();

  std::span<const TensorGridCTData> tensorGrids;  ///< Tensor grids of the deltas sparse grid.
  ClosureTrie trie_;                              ///< Trie over their multi-indices.
  std::vector<size_t> rowOffset_;                 ///< Per dimension (+ end): first row.
  std::vector<size_t> nodeOffset;                 ///< Per row (+ end): offset into nodes.
  std::vector<double> nodes;                      ///< Sorted delta nodes, row after row.
  std::vector<uint8_t> maxNewCoords_;             ///< Per row: see @ref maxNewCoords.
  std::vector<size_t> slotOffset_;                ///< Per leaf (+ end): first slot.
  std::vector<const TensorGridCTData*> ctTGs;     ///< See @ref combinationTGs.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
