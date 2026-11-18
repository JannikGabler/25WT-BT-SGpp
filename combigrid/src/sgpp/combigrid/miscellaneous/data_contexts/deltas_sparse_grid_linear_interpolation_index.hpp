// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_linear_interpolation_index.hpp
 * @brief Defines @c DSGLinInterpIndex, the query-independent data of the linear interpolation
 * operator on a deltas sparse grid (built once per grid, on first use).
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/miscellaneous/closure_trie/closure_trie.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

// Forward declaration
class DeltasSparseGrid;

namespace misc {

/**
 * @brief Query-independent data of the linear interpolation on a deltas sparse grid: how the
 * corners of all combination tensor grids are identified with slots of a per-call value store.
 *
 * <b>Corner coordinates.</b> The tensor grid of level @f$\vec{\ell}@f$ has the nodes
 * @f$N_j(\ell_j) = \bigcup_{\lambda \leq \ell_j} D_j(\lambda)@f$ in dimension j, where
 * @f$D_j(\lambda)@f$ are the delta nodes of level @f$\lambda@f$ (nested nodes; the index takes
 * them from the unit node table of the grid). For a query coordinate
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
 * contribute no bit; a level without new coordinates, possible for growth functions that repeat a
 * node count, gets one unused slot). The slot of a corner is @c slotOffset(leaf) + sub-index,
 * where the leaf is found by the trie walk over @f$\lambda_{d-1}, \ldots, \lambda_0@f$.
 *
 * <b>Fibers.</b> The fiber of dimension k through a multi-index is the sequence of the leaves that
 * differ from it in @f$\lambda_k@f$ only, ordered by @f$\lambda_k = 0, 1, \ldots@f$ (by downwards
 * closure, the levels of a fiber start at 0 and have no gaps). The fibers of a dimension partition
 * the leaves. The operator computes the weights of the slots by a transform along the fibers of
 * one dimension after another (see deltas_sparse_grid_linear_interpolation_tools.hpp); the
 * dimensions below k with two new coordinates at their levels are the same for all leaves of a
 * fiber, so their number (@ref fiberShift) is the bit position of dimension k in the sub-index.
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

  /// @brief Returns the delta nodes of the (dimension, level) of row @p row on the unit interval,
  /// sorted ascending.
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
  LvlMIView leafMI(const size_t leaf) const noexcept { return levels_[trie_.leafItem(leaf)]; }

  /// @brief Returns the indices of the tensor grids of the deltas sparse grid with a non-zero
  /// combination coefficient (the tensor grids of the combination technique), in the grid's order.
  std::span<const size_t> combinationTGs() const noexcept { return ctTGs; }

  /// @brief Returns the level multi-index of the tensor grid @p tgIdx of the deltas sparse grid.
  LvlMIView mi(const size_t tgIdx) const noexcept { return levels_[tgIdx]; }

  /// @brief Returns the combination coefficient of the tensor grid @p tgIdx.
  CTCoeffType coefficient(const size_t tgIdx) const noexcept { return coeffs_[tgIdx]; }

  /// @brief Returns the combination coefficient of the multi-index of leaf @p leaf as a double.
  double leafCoefficient(const size_t leaf) const noexcept { return leafCoeffs_[leaf]; }

  /// @brief Returns the first fiber of dimension @p dim; the fibers of @p dim are
  /// firstFiber(dim), ..., firstFiber(dim + 1) - 1 (firstFiber(nDim()) is the number of fibers).
  size_t firstFiber(const size_t dim) const noexcept { return dimFiberBegin_[dim]; }

  /// @brief Returns the first slots of the blocks of the leaves of fiber @p fiber, ordered by
  /// their level in the fiber's dimension (entry l: the leaf of level l).
  std::span<const size_t> fiberBlocks(const size_t fiber) const noexcept {
    return std::span<const size_t>(fiberBlocks_)
        .subspan(fiberBegin_[fiber], fiberBegin_[fiber + 1] - fiberBegin_[fiber]);
  }

  /// @brief Returns the first leaf of fiber @p fiber of dimension 0: the fibers of dimension 0
  /// are runs of consecutive leaves, in leaf order.
  size_t firstLeafOfDim0Fiber(const size_t fiber) const noexcept { return fiberBegin_[fiber]; }

  /// @brief Returns the number of dimensions below the fiber's dimension k that have two new
  /// coordinates at the levels of the leaves of fiber @p fiber, i.e. the bit of k in the sub-index
  /// of their slots (if it has one) and the number of bits below it.
  uint8_t fiberShift(const size_t fiber) const noexcept { return fiberShift_[fiber]; }

  /// @brief Returns the memory of the index in bytes (the object and its heap memory).
  size_t memoryBytes() const noexcept;

 private:
  /// @brief Sets the rows and copies the sorted unit delta nodes of every (dimension, level) of
  /// @p deltasSparseGrid.
  void collectDeltaNodes(const DeltasSparseGrid& deltasSparseGrid);

  /// @brief Sets @ref maxNewCoords_ of the rows of dimension @p dim.
  void countNewCoords(size_t dim);

  /// @brief Sets the slot offsets of the leaves.
  void computeSlotOffsets();

  /// @brief Sets the fibers of dimension @p dim (appends them to the fiber arrays).
  void collectFibers(size_t dim);

  /// @brief Returns the number of dimensions below @p dim with two new coordinates at the levels
  /// of @p mi.
  uint8_t nMultiDimsBelow(LvlMIView mi, size_t dim) const;

  LvlMIVecView levels_;                  ///< Level multi-indices of the deltas sparse grid.
  std::span<const CTCoeffType> coeffs_;  ///< Their combination coefficients.
  ClosureTrie trie_;                     ///< Trie over the multi-indices.
  std::vector<size_t> rowOffset_;        ///< Per dimension (+ end): first row.
  std::vector<size_t> nodeOffset;        ///< Per row (+ end): offset into nodes.
  std::vector<double> nodes;             ///< Sorted delta nodes, row after row.
  std::vector<uint8_t> maxNewCoords_;    ///< Per row: see @ref maxNewCoords.
  std::vector<size_t> slotOffset_;       ///< Per leaf (+ end): first slot.
  std::vector<size_t> ctTGs;             ///< See @ref combinationTGs.
  std::vector<double> leafCoeffs_;       ///< Per leaf: combination coefficient.
  std::vector<size_t> dimFiberBegin_;    ///< Per dimension (+ end): first fiber.
  std::vector<size_t> fiberBegin_;       ///< Per fiber (+ end): offset into fiberBlocks_.
  std::vector<size_t> fiberBlocks_;      ///< See @ref fiberBlocks, fiber after fiber.
  std::vector<uint8_t> fiberShift_;      ///< Per fiber: see @ref fiberShift.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
