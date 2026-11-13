// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_generation.hpp
 * @brief Helpers that assemble a @c DeltasSparseGrid from a generation instruction, and the
 * mapping of delta node indices to regular node indices.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/vector_map/vector_map.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * TODO: Document
 */
GPCntType getDeltaNodeCnt(LvlType lvl, Lvl2GPCntFunc lvl2GPCntFunc, bool includeNodesOnBoundary);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the deltas-sparse-grid populator.
 */
namespace deltas_sparse_grid_generation {

/**
 * TODO: Document
 */
std::vector<TensorGridCTData> genTGData(const SGGenInstr& genInstr, const LvlMIVecView mis,
                                        const std::span<const CTCoeffType> coeffs);

/**
 * @brief Constructs a single tensor grid for the given level multi-index.
 *
 * @param mi       Level multi-index.
 * @param genInstr Generation instruction.
 * @return Tensor grid with the proper per-dim node counts and node positions.
 */
TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr);

/**
 * @brief Translates a level multi-index into the per-dimension grid-point
 * counts of the corresponding tensor grid.
 *
 * Applies the per-dim @ref Lvl2GPCntFunc and the boundary level offset
 * provided by @p genInstr.
 *
 * @param mi       Level multi-index.
 * @param genInstr Generation instruction.
 */
GPMI getGPCntPerDim(const LvlMI& mi, const SGGenInstr& genInstr);

// TODO: Delete (?)
/**
 * @brief Returns the discrete bounding box used to iterate over all grid
 * points of a tensor grid with the given per-dim counts.
 *
 * @param gpCntPerDim Per-dim grid-point counts.
 */
misc::DiscRectBB<GPCntType> getBBForIteration(GPMI gpCntPerDim);

/**
 * @brief Concatenates the 1D node positions for every dimension of the
 * tensor grid identified by @p mi.
 *
 * @param mi          Level multi-index.
 * @param genInstr    Generation instruction.
 * @param gpCntPerDim Per-dim grid-point counts (must already match @p mi).
 * @return Flat vector of size @f$\sum_k n_k@f$ holding the per-dim node
 *         positions back-to-back.
 */
base::DataVector getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                     const GPMI& gpCntPerDim);

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
