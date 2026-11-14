// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file node_table_generation.hpp
 * @brief Generation of the node table (@c NodeTable) of a @c SparseGrid or @c DeltasSparseGrid.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/// @brief Generation of the node table of a sparse grid.
namespace node_table_generation {

/**
 * @brief Generates the node table of the tensor grids of @p levels: per dimension the node lists
 * of the levels 0, ..., (maximum level of the dimension in @p levels), mapped onto the domain with
 * @c tools::mapToDomain (or on the unit interval). Dimensions with the same node-generation
 * function, level-to-grid-point count function and domain interval share their lists.
 *
 * @param genInstr      Generation instruction (nodes, counts, boundary level offset, domain).
 * @param levels        Level multi-indices of the tensor grids.
 * @param deltaNodes    Whether the lists are the delta nodes of the levels (@c DeltasSparseGrid)
 *                      or the regular nodes (@c SparseGrid).
 * @param mapOntoDomain Whether the nodes are mapped onto the domain (the grid's table) or stay on
 *                      the unit interval (@c misc::UnitNodeTable; same layout).
 */
NodeTable genNodeTable(const SGGenInstr& genInstr, const LvlMIVec& levels, bool deltaNodes,
                       bool mapOntoDomain = true);

/**
 * @brief Returns the node table of @p sparseGrid on the unit interval (@c misc::UnitNodeTable),
 * built on the first call and kept in the grid's operator data.
 *
 * @param sparseGrid Sparse grid.
 */
const NodeTable& getUnitNodeTable(const SparseGrid& sparseGrid);

/**
 * @brief Returns, per dimension, the first dimension with the same node lists (the same
 * node-generation function, level-to-grid-point-count function and domain interval); the
 * dimension itself if there is none before it.
 *
 * @param genInstr Generation instruction.
 */
std::vector<size_t> findDimsWithSameNodes(const SGGenInstr& genInstr);

/**
 * @brief Returns the number of nodes of (dimension @p dim, level @p lvl).
 *
 * @param genInstr   Generation instruction.
 * @param dim        Dimension.
 * @param lvl        Level.
 * @param deltaNodes Delta nodes (with the boundary nodes at the boundary level offset) or regular
 *                   nodes (with the boundary nodes from the boundary level offset on).
 */
GPCntType compNodeCnt(const SGGenInstr& genInstr, size_t dim, LvlType lvl, bool deltaNodes);

/**
 * @brief Writes the nodes of (dimension @p dim, level @p lvl) in domain coordinates to row @p row
 * of @p table.
 *
 * @param genInstr   Generation instruction.
 * @param dim        Dimension.
 * @param lvl        Level.
 * @param deltaNodes See @ref compNodeCnt.
 * @param mapOntoDomain See @ref genNodeTable.
 * @param table      Node table (row @p row has @ref compNodeCnt nodes).
 * @param row        Row of (@p dim, @p lvl).
 */
void genRow(const SGGenInstr& genInstr, size_t dim, LvlType lvl, bool deltaNodes,
            bool mapOntoDomain, NodeTable& table, size_t row);

}  // namespace node_table_generation

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
