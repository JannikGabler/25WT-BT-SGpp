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
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
/// @brief Generation of the node tables of a sparse grid.
namespace node_table_generation {

/**
 * @brief Generates the node table of the tensor grids of @p levels on the unit interval: per
 * dimension the node lists of the levels 0, ..., (maximum level of the dimension in @p levels), as
 * the node-generation function of the dimension generates them. Dimensions with the same
 * node-generation function and level-to-grid-point-count function share their lists.
 *
 * @param genInstr   Generation instruction (nodes, counts, boundary level offset).
 * @param levels     Level multi-indices of the tensor grids.
 * @param deltaNodes Whether the lists are the delta nodes of the levels (@c DeltasSparseGrid) or
 *                   the regular nodes (@c SparseGrid).
 */
NodeTable genUnitNodeTable(const SGGenInstr& genInstr, const LvlMIVec& levels, bool deltaNodes);

/**
 * @brief Maps the node table @p unitNodeTable (see @ref genUnitNodeTable) onto the domain of
 * @p genInstr with @c tools::mapToDomain. Dimensions with the same node-generation function,
 * level-to-grid-point-count function and domain interval share their lists.
 *
 * @param genInstr      Generation instruction (the same as for @p unitNodeTable).
 * @param unitNodeTable Node table on the unit interval.
 */
NodeTable mapOntoDomain(const SGGenInstr& genInstr, const NodeTable& unitNodeTable);

/**
 * @brief Returns, per dimension, the first dimension with the same node lists (the same
 * node-generation function and level-to-grid-point-count function, and, if @p sameDomain, the
 * same domain interval); the dimension itself if there is none before it.
 *
 * @param genInstr   Generation instruction.
 * @param sameDomain Whether the dimensions must also have the same domain interval.
 */
std::vector<size_t> findDimsWithSameNodes(const SGGenInstr& genInstr, bool sameDomain);

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

}  // namespace node_table_generation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
