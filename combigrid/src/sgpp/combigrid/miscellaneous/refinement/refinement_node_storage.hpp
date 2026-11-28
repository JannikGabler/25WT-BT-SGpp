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
 * level up to a fixed capacity, so that rows can be set while other threads read existing ones
 * (the table entries never move; a tensor-grid view stays valid).
 *
 * At the start of a call the rows of the grid's tables are copied in (@ref adopt; the node lists
 * are shared, see @c NodeTable), missing rows are taken from the grid's @c NodeStorage when a
 * tensor grid needs them (@ref ensureRows, serial), and at the end compact tables for the grid
 * are made (@ref release). The rows come from the same storage requests as those of a grid built
 * from the same index set, so they are the same node lists.
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
   * @brief Copies the rows of a grid's tables in (their levels must not exceed the capacity).
   * @param unitTable   The grid's unit node table.
   * @param domainTable The grid's domain node table.
   */
  void adopt(const NodeTable& unitTable, const NodeTable& domainTable);

  /// @brief Sets the missing rows of (dimension, @p levels[dimension]), unit and domain.
  void ensureRows(LvlMIView levels);

  /**
   * @brief Returns compact tables with the layout of a grid's tables holding the rows of the
   * levels 0 ... @p maxLvls[dim] (all of them must have been set or adopted).
   * @return The unit and the domain node table.
   */
  std::pair<NodeTable, NodeTable> release(const std::vector<LvlType>& maxLvls) const;

 private:
  /**
   * @brief Sets every row of @p to whose (dimension, level) @p from has to that row of @p from
   * (the levels both tables have).
   */
  static void copyRows(const NodeTable& from, NodeTable& to);

  /// @brief Sets the unset rows of (dimension, @p levels[dimension]) of @p table.
  void setMissingRows(LvlMIView levels, bool onDomain, NodeTable& table) const;

  SparseGridSetup setup_;  ///< Setup of the grid.
  LvlType capacity_;       ///< Highest level of the rows.
  NodeTable unit_;         ///< Unit nodes, every level up to the capacity.
  NodeTable domain_;       ///< Domain nodes, every level up to the capacity.
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
