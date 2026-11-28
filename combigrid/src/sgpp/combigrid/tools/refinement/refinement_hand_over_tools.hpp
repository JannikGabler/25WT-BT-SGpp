// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_hand_over_tools.hpp
 * @brief Hand-over of a sparse grid's storage to a dimension-adaptive refinement call and back.
 */
#pragma once

#include <cstdint>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_node_storage.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_state.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {
namespace refinement {

/**
 * @brief Rebuilds @p state from @p grid unless the state was synchronized with the grid's current
 * storage: the index set becomes the downwards closure of the grid's levels (in its order) with
 * the combination coefficients, a multi-index is old if every forward neighbour up to
 * @p maxLevel is in the set and active otherwise; no values are known. An empty grid starts with
 * the zero multi-index as the only (pending) candidate.
 *
 * @param grid     The grid.
 * @param state    State of the refinement (keeps its number of values).
 * @param maxLevel Largest admissible level per dimension.
 * @return Whether the state was rebuilt.
 */
bool synchronizeState(const SparseGridBase& grid, misc::RefinementState& state, LvlType maxLevel);

/**
 * @brief Returns the level capacity the node tables of a call need: @p maxLevel or the highest
 * level of the state's multi-indices, whichever is larger.
 */
LvlType nodeCapacity(const misc::RefinementState& state, LvlType maxLevel);

/**
 * @brief Begins a call: drops the grid's cross-call operator data and copies the rows of its node
 * tables into @p nodes (the grid is not usable until @ref endHandOver).
 */
void beginHandOver(SparseGridBase& grid, misc::RefinementNodeStorage& nodes);

/**
 * @brief Ends a call. If the call changed the grid (@p changed), the grid gets the multi-indices of
 * the state's grid (active and old) with a coefficient other than 0, in colexicographic order (the
 * order of a grid built by @c MIVecSGGenInstr from the same index set), and compact node tables
 * (a new storage version). Otherwise the grid stays as it is.
 * @param grid    The grid.
 * @param state   State of the refinement (records the grid's storage version).
 * @param nodes   Node tables of the call.
 * @param changed Whether the call committed or accepted a multi-index.
 */
void endHandOver(SparseGridBase& grid, misc::RefinementState& state,
                 misc::RefinementNodeStorage& nodes, bool changed);

/**
 * @brief Returns the target's combination on the grid of @p state: the finished
 * @f$\sum_{\vec{\ell}} c_{\vec{\ell}} Q_{\vec{\ell}}@f$ over its multi-indices with coefficient
 * other than 0 in colexicographic order, combined like the operators (compensated dot products);
 * NaN if one of them has no values yet (a call stopped before the grid was evaluated).
 */
std::vector<double> combineEstimate(const misc::RefinementState& state,
                                    const RefinementTarget& target);

}  // namespace refinement
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
