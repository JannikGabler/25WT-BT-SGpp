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
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
/// @brief Generation of the node tables of a sparse grid.
namespace node_table_generation {

/**
 * @brief Generates the node table of the tensor grids of @p levels on the unit interval: per
 * dimension the node lists of the levels 0, ..., (maximum level of the dimension in @p levels), as
 * the node family of the dimension generates them. Dimensions with the same
 * node family and level-to-grid-point-count function share their lists.
 *
 * @param setup   Setup of the grid (nodes, counts, boundary level offset).
 * @param levels     Level multi-indices of the tensor grids.
 * @param deltaNodes Whether the lists are the delta nodes of the levels (@c DeltasSparseGrid) or
 *                   the regular nodes (@c SparseGrid).
 */
NodeTable genUnitNodeTable(const SparseGridSetup& setup, const LvlMIVec& levels, bool deltaNodes);

/**
 * @brief Maps the node table @p unitNodeTable (see @ref genUnitNodeTable) onto the domain of
 * @p setup with @c tools::mapToDomain. Dimensions with the same node family,
 * level-to-grid-point-count function and domain interval share their lists.
 *
 * @param setup      Setup of the grid (the same as for @p unitNodeTable).
 * @param unitNodeTable Node table on the unit interval.
 */
NodeTable mapOntoDomain(const SparseGridSetup& setup, const NodeTable& unitNodeTable);

/**
 * @brief Returns a node table with the row layout of @ref genUnitNodeTable (@p sameDomain
 * @c false) resp. @ref mapOntoDomain (@p sameDomain @c true) for the maximum levels @p maxLvls,
 * with all rows empty (to be filled with @ref genUnitRow / @ref genDomainRow).
 *
 * @param setup      Setup of the grid.
 * @param maxLvls    Maximum level per dimension.
 * @param sameDomain Whether dimensions share rows only with the same domain interval too.
 */
NodeTable genEmptyNodeTable(const SparseGridSetup& setup, std::vector<LvlType> maxLvls,
                            bool sameDomain);

/**
 * @brief Writes the regular nodes of (dimension @p dim, level @p lvl) on the unit interval to
 * @p out (resized), as in @ref genUnitNodeTable.
 */
void genUnitRow(const SparseGridSetup& setup, size_t dim, LvlType lvl, base::DataVector& out);

/**
 * @brief Writes @p unitNodes of dimension @p dim mapped onto the domain (@c tools::mapToDomain)
 * to @p out (resized), as in @ref mapOntoDomain.
 */
void genDomainRow(const SparseGridSetup& setup, size_t dim, std::span<const double> unitNodes,
                  base::DataVector& out);

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
