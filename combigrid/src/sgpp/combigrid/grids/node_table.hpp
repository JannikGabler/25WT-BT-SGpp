// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file node_table.hpp
 * @brief The 1D node lists of a sparse grid, one per (dimension, level), shared by all tensor
 * grids (the lists themselves live in a @c NodeStorage).
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

// Forward declaration
class TensorGridView;

/**
 * @brief The 1D node lists of a sparse grid, one per (dimension, level).
 *
 * The nodes of a tensor grid of level @f$\vec{\ell}@f$ in dimension @f$k@f$ depend only on
 * @f$k@f$ and @f$\ell_k@f$ (for a given generation instruction). A sparse grid therefore stores
 * every such list once, in a "row" of this table, and its tensor grids refer to the rows by their
 * levels: the nodes of the tensor grid @f$\vec{\ell}@f$ in dimension @f$k@f$ are
 * @ref nodes "nodes(k, ℓ_k)". A @c SparseGrid stores the regular nodes of every level (including
 * the boundary nodes if the level is at least the boundary level offset), a @c DeltasSparseGrid
 * the delta nodes (see @c DeltasSparseGrid).
 *
 * Every dimension has the rows of the levels @f$0, \ldots,@f$ @ref maxLvl "maxLvl(k)". Dimensions
 * with the same node lists share their rows (see @ref sharesRowsWith).
 *
 * The table does not own node lists: a row is a view into a list of a @c NodeStorage (all nodes,
 * or those without the boundary nodes) together with a pointer that keeps that list alive. Copies
 * of a table share the lists; rows never move.
 *
 * A sparse grid has two tables with the same levels and node counts (see @c SparseGridBase): the
 * nodes on the unit interval, as the node families generate them, and the nodes in
 * domain coordinates (mapped with @c tools::mapToDomain, i.e. bitwise the coordinates at which the
 * operators evaluate the source function). On the unit interval, dimensions with the same
 * node family and level-to-grid-point-count function share their rows; on the
 * domain, only those with the same domain interval too (see
 * @c tools::node_table_generation::findDimsWithSameNodes).
 */
class NodeTable {
 public:
  /// @name Constructors
  /// @{

  /// @brief Constructs the empty table of a 0-dimensional grid.
  NodeTable() noexcept = default;

  /**
   * @brief Constructs a table with the given layout and empty rows, which are filled in through
   * @ref setRow.
   *
   * @param firstRowOfDim Per dimension: the row of level 0 (dimensions sharing their rows have the
   *                      same entry).
   * @param maxLvlOfDim   Per dimension: the maximum level (rows of the levels 0, ..., maxLvl).
   * @param nRows         Number of rows.
   */
  NodeTable(std::vector<size_t> firstRowOfDim, std::vector<LvlType> maxLvlOfDim, size_t nRows);

  /// @}

  /// @name Getters
  /// @{

  /// @brief Returns the number of dimensions.
  size_t nDim() const noexcept { return firstRowOfDim_.size(); }

  /// @brief Returns the maximum level of dimension @p dim (the highest level with a row).
  LvlType maxLvl(const size_t dim) const noexcept {
    assert(dim < nDim());
    return maxLvlOfDim_[dim];
  }

  /// @brief Returns the row of (dimension @p dim, level @p lvl).
  size_t row(const size_t dim, const LvlType lvl) const noexcept {
    assert(dim < nDim() && lvl <= maxLvlOfDim_[dim]);
    return firstRowOfDim_[dim] + lvl;
  }

  /// @brief Returns the number of nodes of (dimension @p dim, level @p lvl).
  GPCntType nNodes(const size_t dim, const LvlType lvl) const noexcept {
    return static_cast<GPCntType>(rows_[row(dim, lvl)].size());
  }

  /// @brief Returns the nodes of row @p row.
  std::span<const double> rowSpan(const size_t row) const noexcept {
    assert(row < nRows());
    return rows_[row];
  }

  /// @brief Returns the node list that row @p row views (null for a row not set).
  const std::shared_ptr<const base::DataVector>& rowOwner(const size_t row) const noexcept {
    assert(row < nRows());
    return owners_[row];
  }

  /// @brief Returns the first of the @ref nNodes "nNodes(dim, lvl)" nodes of (dimension @p dim,
  /// level @p lvl).
  const double* nodes(const size_t dim, const LvlType lvl) const noexcept {
    return rows_[row(dim, lvl)].data();
  }

  /// @brief Returns the nodes of (dimension @p dim, level @p lvl).
  std::span<const double> nodeSpan(const size_t dim, const LvlType lvl) const noexcept {
    return {nodes(dim, lvl), nNodes(dim, lvl)};
  }

  /// @brief Returns whether dimensions @p dim and @p otherDim share their rows (same node lists).
  bool sharesRowsWith(const size_t dim, const size_t otherDim) const noexcept {
    return firstRowOfDim_[dim] == firstRowOfDim_[otherDim];
  }

  /// @brief Returns the number of rows (distinct node lists).
  size_t nRows() const noexcept { return rows_.size(); }

  /// @brief Returns the number of nodes of all rows (a row shared by dimensions counted once).
  size_t nStoredNodes() const noexcept;

  /// @}

  /// @name Setters
  /// @{

  /**
   * @brief Sets row @p row to @p nodes, a contiguous part of the node list @p owner (which the
   * table keeps alive).
   * @param row   Row index in @c [0, nRows()).
   * @param owner Node list (e.g. a row of a @c NodeStorage).
   * @param nodes Nodes of the row, within @p owner.
   */
  void setRow(size_t row, std::shared_ptr<const base::DataVector> owner,
              std::span<const double> nodes) noexcept;

  /// @brief Sets row @p row to all nodes of @p owner (see the other overload).
  void setRow(size_t row, std::shared_ptr<const base::DataVector> owner) noexcept;

  /// @}

  /// @name Operators
  /// @{

  /**
   * @brief Tests for equality: the same layout and bitwise the same nodes (wherever they are
   * stored).
   * @param other Node table to compare against.
   */
  bool operator==(const NodeTable& other) const;

  /// @}

 private:
  /// The views read the rows directly (without the indirection through the vectors).
  friend class TensorGridView;

  std::vector<size_t> firstRowOfDim_;          ///< Per dimension: row of level 0.
  std::vector<LvlType> maxLvlOfDim_;           ///< Per dimension: maximum level.
  std::vector<std::span<const double>> rows_;  ///< Per row: its nodes.
  /// Per row: the node list its nodes belong to (keeps them alive).
  std::vector<std::shared_ptr<const base::DataVector>> owners_;
};

}  // namespace combigrid
}  // namespace sgpp
