// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_state.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_node_storage.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_state.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/sparse_grid_storage_access.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum_tools.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_decision_tools.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_hand_over_tools.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_index_set_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

using sgpp::combigrid::misc::RefinementIndexState;
using sgpp::combigrid::misc::RefinementNodeStorage;
using sgpp::combigrid::misc::RefinementState;
using sgpp::combigrid::misc::SparseGridStorageAccess;

namespace sgpp {
namespace combigrid {
namespace tools {
namespace refinement {

namespace {

/// The multi-indices of the grid of @p state (active and old) in colexicographic order.
std::vector<size_t> gridIndicesInOrder(const RefinementState& state) {
  std::vector<size_t> inGrid;
  for (size_t idx = 0; idx < state.states.size(); idx++) {
    if (state.isInGrid(idx)) inGrid.push_back(idx);
  }
  return sortColexicographically(state.indexSet, std::move(inGrid));
}

}  // namespace

bool synchronizeState(const SparseGridBase& grid, RefinementState& state, const LvlType maxLevel) {
  if (state.gridVersion != 0 &&
      state.gridVersion == SparseGridStorageAccess::storageVersion(grid)) {
    return false;
  }

  const LvlMIVec closure = grid.getLevels().downwardsClosure();
  state = RefinementState(grid.nDim(), state.nValues);
  state.indexSet.reserve(closure.nMI());
  for (size_t miIdx = 0; miIdx < closure.nMI(); miIdx++) {
    state.add(closure[miIdx], RefinementIndexState::Active,
              tensorGridCost(grid.getSetup(), closure[miIdx]));
  }
  state.coefficients = computeCTCoeffs(closure);
  for (size_t idx = 0; idx < state.states.size(); idx++) {
    if (hasAllForwardNeighbours(state.indexSet, idx, maxLevel)) {
      state.states[idx] = RefinementIndexState::Old;
    }
  }
  if (closure.nMI() == 0) {
    const LvlMI root(grid.nDim());
    state.add(root, RefinementIndexState::Pending, tensorGridCost(grid.getSetup(), root));
  }
  return true;
}

LvlType nodeCapacity(const RefinementState& state, const LvlType maxLevel) {
  LvlType capacity = maxLevel;
  for (size_t idx = 0; idx < state.states.size(); idx++) {
    const LvlMIView mi = state.indexSet.levels(idx);
    for (size_t dim = 0; dim < mi.size(); dim++) capacity = std::max(capacity, mi[dim]);
  }
  return capacity;
}

void beginHandOver(SparseGridBase& grid, RefinementNodeStorage& nodes) {
  SparseGridStorageAccess::clearCrossCallOperatorData(grid);
  nodes.adopt(SparseGridStorageAccess::unitNodeTable(grid),
              SparseGridStorageAccess::domainNodeTable(grid));
}

void endHandOver(SparseGridBase& grid, RefinementState& state, RefinementNodeStorage& nodes,
                 const bool changed) {
  if (!changed) {
    nodes.giveBack(SparseGridStorageAccess::unitNodeTable(grid),
                   SparseGridStorageAccess::domainNodeTable(grid));
    state.gridVersion = SparseGridStorageAccess::storageVersion(grid);
    return;
  }

  LvlMIVec levels(grid.nDim(), 0);
  std::vector<CTCoeffType> coefficients;
  std::vector<LvlType> maxLvls(grid.nDim(), 0);
  for (const size_t idx : gridIndicesInOrder(state)) {
    const LvlMIView mi = state.indexSet.levels(idx);
    for (size_t dim = 0; dim < mi.size(); dim++) maxLvls[dim] = std::max(maxLvls[dim], mi[dim]);
    if (state.coefficients[idx] != 0) {
      levels.push_back(mi);
      coefficients.push_back(state.coefficients[idx]);
    }
  }

  auto [unitTable, domainTable] = nodes.release(maxLvls);
  SparseGridStorageAccess::replaceStorage(grid, std::move(levels), std::move(coefficients),
                                          std::move(unitTable), std::move(domainTable));
  state.gridVersion = SparseGridStorageAccess::storageVersion(grid);
}

std::vector<double> combineEstimate(const RefinementState& state, const RefinementTarget& target) {
  std::vector<double> coefficients;
  std::vector<double> values;
  for (const size_t idx : gridIndicesInOrder(state)) {
    if (state.coefficients[idx] == 0) continue;
    if (!state.evaluated[idx]) {
      return std::vector<double>(state.nValues, std::numeric_limits<double>::quiet_NaN());
    }
    coefficients.push_back(static_cast<double>(state.coefficients[idx]));
    const auto tgValues = state.valuesOf(idx);
    values.insert(values.end(), tgValues.begin(), tgValues.end());
  }

  std::vector<double> estimate(state.nValues, 0.0);
  numerics::compensatedDotProducts<double>(coefficients, values, std::span<double>(estimate));
  target.finishCombination(estimate);
  return estimate;
}

}  // namespace refinement
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
