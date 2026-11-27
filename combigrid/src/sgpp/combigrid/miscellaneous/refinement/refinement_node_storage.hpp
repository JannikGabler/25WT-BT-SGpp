// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_node_storage.hpp
 * @brief Node tables of a sparse grid during a dimension-adaptive refinement call: rows are
 * added without moving existing ones.
 */
#pragma once

#include <cstdint>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief The unit and domain node tables of a grid during a refinement call, laid out for every
 * level up to a fixed capacity, so that rows can be generated while other threads read existing
 * ones (no row object moves; a tensor-grid view stays valid).
 *
 * At the start of a call the rows of the grid's tables are moved in (@ref adopt), new rows are
 * generated when a tensor grid needs them (@ref ensureRows, serial), and at the end compact
 * tables are handed back to the grid (@ref release). Rows are generated with the functions that
 * build a grid's tables, so they equal the rows of a grid built from the same index set bitwise.
 */
class RefinementNodeStorage {
 public:
  /**
   * @brief Constructs empty tables with rows for the levels 0 ... @p capacity of every dimension.
   * @param setup    Setup of the grid (copied).
   * @param capacity Highest level a row can be generated for.
   */
  RefinementNodeStorage(const SparseGridSetup& setup, LvlType capacity);

  /// @brief Returns the highest level a row can be generated for.
  LvlType capacity() const noexcept { return capacity_; }

  /// @brief Returns the table of the nodes on the unit interval.
  const NodeTable& unitTable() const noexcept { return unit_; }

  /// @brief Returns the table of the nodes on the domain.
  const NodeTable& domainTable() const noexcept { return domain_; }

  /**
   * @brief Moves the rows of a grid's tables in (their levels must not exceed the capacity).
   * @param unitTable   The grid's unit node table (its rows are left empty).
   * @param domainTable The grid's domain node table (its rows are left empty).
   */
  void adopt(NodeTable& unitTable, NodeTable& domainTable);

  /**
   * @brief Moves the rows back into a grid's tables (the inverse of @ref adopt; their levels must
   * have rows here). The tables here are empty afterwards.
   */
  void giveBack(NodeTable& unitTable, NodeTable& domainTable);

  /// @brief Generates the missing rows of (dimension, @p levels[dimension]), unit and domain.
  void ensureRows(LvlMIView levels);

  /**
   * @brief Moves the rows of the levels 0 ... @p maxLvls[dim] out into compact tables with the
   * layout of a grid's tables (all of them must have been generated or adopted).
   * @return The unit and the domain node table.
   */
  std::pair<NodeTable, NodeTable> release(const std::vector<LvlType>& maxLvls);

 private:
  /**
   * @brief Swaps every row of @p to whose (dimension, level) @p from has with that row of
   * @p from (rows shared by dimensions once).
   */
  static void moveRows(NodeTable& from, NodeTable& to);

  /// @brief Marks the rows of this storage's tables as absent.
  void clearPresence();

  SparseGridSetup setup_;               ///< Setup of the grid.
  LvlType capacity_;                    ///< Highest level of the rows.
  NodeTable unit_;                      ///< Unit nodes, every level up to the capacity.
  NodeTable domain_;                    ///< Domain nodes, every level up to the capacity.
  std::vector<uint8_t> unitPresent_;    ///< Per row of unit_: generated or adopted.
  std::vector<uint8_t> domainPresent_;  ///< Per row of domain_: generated or adopted.
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
