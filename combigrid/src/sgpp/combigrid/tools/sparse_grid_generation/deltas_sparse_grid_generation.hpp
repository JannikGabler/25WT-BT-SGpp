// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_generation.hpp
 * @brief Delta nodes of a @c DeltasSparseGrid: their number per level and the mapping of delta
 * node indices to regular node indices.
 */
#pragma once

#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Returns the number of delta nodes of level @p lvl: the nodes of level @p lvl that are not
 * nodes of level @p lvl - 1 (nested nodes), plus the two boundary nodes if
 * @p includeNodesOnBoundary (the level is the boundary level offset).
 *
 * @param lvl                    Level.
 * @param lvl2GPCntFunc          Level-to-grid-point-count function (number of inner nodes).
 * @param includeNodesOnBoundary Whether the boundary nodes belong to the level.
 */
GPCntType getDeltaNodeCnt(LvlType lvl, Lvl2GPCntFunc lvl2GPCntFunc, bool includeNodesOnBoundary);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the deltas sparse grid: mapping of delta node indices.
 */
namespace deltas_sparse_grid_generation {

/**
 * @brief Returns, per dimension, the function that maps the index of a delta node to the index
 * of the same node in the (inner) nodes of a higher or equal level.
 *
 * @param deltasSparseGrid Deltas sparse grid (its generation instruction provides the
 *                         node-generation and level-to-grid-point-count functions).
 */
std::vector<NodeIdxLvlMapFunc> compNodeIdxLvlMapFuncs(const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Maps the index of a node in a delta tensor grid to the index of the same node in a
 * regular tensor grid of a higher or equal level (in one dimension), taking the boundary nodes
 * into account.
 *
 * @param nNodesInDeltaTG   Number of delta nodes (including boundary nodes) of @p deltaTGLvl.
 * @param nNodesInTargetTG  Number of regular nodes (including boundary nodes) of @p targetTGLvl.
 * @param deltaTGLvl        Level of the delta tensor grid.
 * @param targetTGLvl       Level of the regular tensor grid, at least @p deltaTGLvl.
 * @param gpIdxInDeltaTG    Index of the delta node.
 * @param nodeIdxLvlMapFunc Index map of the inner nodes (see @ref compNodeIdxLvlMapFuncs).
 * @param boundaryLvlOffset Boundary level offset of the sparse grid.
 * @return Index of the node in the regular nodes of @p targetTGLvl.
 */
GPCntType mapDeltaNodeIdxToRegular(GPCntType nNodesInDeltaTG, GPCntType nNodesInTargetTG,
                                   LvlType deltaTGLvl, LvlType targetTGLvl,
                                   GPCntType gpIdxInDeltaTG, NodeIdxLvlMapFunc nodeIdxLvlMapFunc,
                                   LvlType boundaryLvlOffset);

}  // namespace deltas_sparse_grid_generation

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
