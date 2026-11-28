// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file node_table_generation.hpp
 * @brief Generation of the node tables (@c NodeTable) of a @c SparseGrid or @c DeltasSparseGrid.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/node_row_key.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
/// @brief Generation of the node tables of a sparse grid.
namespace node_table_generation {

/**
 * @brief Generates the node tables of the tensor grids of @p levels: per dimension the node lists
 * of the levels 0, ..., (maximum level of the dimension in @p levels), on the unit interval and on
 * the domain, taken from the node storage of @p setup (generated there if missing). On the unit
 * interval, dimensions with the same node family and level-to-grid-point-count function share
 * their rows; on the domain, only those with the same interval too.
 *
 * @param setup      Setup of the grid (nodes, counts, boundary level offset, domain, storage).
 * @param levels     Level multi-indices of the tensor grids.
 * @param deltaNodes Whether the lists are the delta nodes of the levels (@c DeltasSparseGrid) or
 *                   the regular nodes (@c SparseGrid).
 * @return The unit and the domain node table.
 */
std::pair<NodeTable, NodeTable> genNodeTables(const SparseGridSetup& setup, const LvlMIVec& levels,
                                              bool deltaNodes);

/**
 * @brief Returns a node table with the row layout of @ref genNodeTables (unit table for
 * @p sameDomain @c false, domain table for @c true) for the maximum levels @p maxLvls, with all
 * rows unset (to be set with @ref setRows).
 *
 * @param setup      Setup of the grid.
 * @param maxLvls    Maximum level per dimension.
 * @param sameDomain Whether dimensions share rows only with the same domain interval too.
 */
NodeTable genEmptyNodeTable(const SparseGridSetup& setup, std::vector<LvlType> maxLvls,
                            bool sameDomain);

/**
 * @brief Sets the rows @p tableRows of @p table to the node lists of (dimension, level)
 * @p rowLevels (one pair per row) from the node storage of @p setup, as in @ref genNodeTables.
 *
 * @param setup      Setup of the grid.
 * @param deltaNodes Delta nodes or regular nodes.
 * @param onDomain   Nodes on the domain or on the unit interval.
 * @param tableRows  Rows of @p table to set.
 * @param rowLevels  Per row: its dimension and level.
 * @param table      Node table (unit or domain layout, matching @p onDomain).
 */
void setRows(const SparseGridSetup& setup, bool deltaNodes, bool onDomain,
             std::span<const size_t> tableRows,
             std::span<const std::pair<size_t, LvlType>> rowLevels, NodeTable& table);

/**
 * @brief Returns the key of the node list of (dimension @p dim, level @p lvl) in a
 * @c NodeStorage (see @c NodeRowKey; on the unit interval unless @p onDomain).
 */
NodeRowKey rowKey(const SparseGridSetup& setup, size_t dim, LvlType lvl, bool deltaNodes,
                  bool onDomain);

/**
 * @brief Returns whether the nodes of level @p lvl include the boundary nodes: regular nodes
 * from the boundary level offset on, delta nodes at the offset.
 */
bool includesBoundary(const SparseGridSetup& setup, LvlType lvl, bool deltaNodes);

/**
 * @brief Returns, per dimension, the first dimension with the same node lists (the same
 * node family and level-to-grid-point-count function, and, if @p sameDomain, the
 * same domain interval); the dimension itself if there is none before it.
 *
 * @param setup   Setup of the grid.
 * @param sameDomain Whether the dimensions must also have the same domain interval.
 */
std::vector<size_t> findDimsWithSameNodes(const SparseGridSetup& setup, bool sameDomain);

/**
 * @brief Returns the number of nodes of (dimension @p dim, level @p lvl).
 *
 * @param setup   Setup of the grid.
 * @param dim        Dimension.
 * @param lvl        Level.
 * @param deltaNodes Delta nodes (with the boundary nodes at the boundary level offset) or regular
 *                   nodes (with the boundary nodes from the boundary level offset on).
 */
GPCntType compNodeCnt(const SparseGridSetup& setup, size_t dim, LvlType lvl, bool deltaNodes);

}  // namespace node_table_generation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
