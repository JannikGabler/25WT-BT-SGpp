// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_state.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_state.hpp>
#include <sgpp/combigrid/refinement/refinement_indicator.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_decision_tools.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_index_set_tools.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::combigrid::misc::RefinementIndexSet;
using sgpp::combigrid::misc::RefinementIndexState;
using sgpp::combigrid::misc::RefinementState;
using sgpp::combigrid::tools::numerics::CompensatedSum;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace refinement {

size_t tensorGridCost(const SparseGridSetup& setup, const LvlMIView mi) {
  constexpr size_t MAX = std::numeric_limits<size_t>::max();
  size_t cost = 1;
  for (size_t dim = 0; dim < mi.size(); dim++) {
    const size_t nNodes = node_table_generation::compNodeCnt(setup, dim, mi[dim], false);
    cost = (nNodes != 0 && cost > MAX / nNodes) ? MAX : cost * nNodes;
  }
  return cost;
}

double priorityOf(const double gain, const size_t cost, const RefinementIndicator indicator) {
  const double safeGain = std::isnan(gain) ? 0.0 : gain;
  switch (indicator) {
    case RefinementIndicator::Gain:
      return safeGain;
    case RefinementIndicator::Cost:
      return 1.0 / static_cast<double>(cost);
    case RefinementIndicator::Profit:
      return safeGain / static_cast<double>(cost);
  }
  return 0.0;
}

void computeIncrement(const RefinementState& state, const size_t idx,
                      std::vector<CompensatedSum<double>>& sums, std::vector<double>& increment,
                      std::vector<size_t>& support) {
  const size_t nValues = state.nValues;
  sums.assign(nValues, CompensatedSum<double>());

  forEachBackwardBoxMember(state.indexSet, idx, support, [&](const size_t member, const int sign) {
    const std::span<const double> values = state.valuesOf(member);
    for (size_t i = 0; i < nValues; i++) {
      sums[i].add(sign > 0 ? values[i] : -values[i]);
    }
  });

  increment.resize(nValues);
  for (size_t i = 0; i < nValues; i++) {
    increment[i] = sums[i].value();
  }
}

double gainOf(const std::span<const double> increment, const RefinementTarget& target,
              std::vector<double>& scratch) {
  scratch.assign(increment.begin(), increment.end());
  target.finishCombination(scratch);

  double gain = 0.0;
  for (const double value : scratch) {
    if (std::isnan(value)) {
      return value;  // a NaN of the source function stays visible
    }
    gain = std::max(gain, std::abs(value));
  }
  return gain;
}

namespace {

/// Whether every backward neighbour of @p candidate except the one in @p dim is old.
bool otherBackwardNeighboursOld(const RefinementState& state, const LvlMI& candidate,
                                const uint64_t hash, const size_t dim) {
  for (size_t other = 0; other < candidate.size(); other++) {
    if (other == dim || candidate[other] == 0) continue;
    const size_t backward = state.indexSet.findNeighbour(candidate, hash, other, -1);
    if (backward == RefinementIndexSet::NOT_FOUND ||
        state.states[backward] != RefinementIndexState::Old) {
      return false;
    }
  }
  return true;
}

}  // namespace

void collectNewCandidates(const RefinementState& state, const size_t idx, const LvlType maxLevel,
                          LvlMI& neighbour, std::vector<size_t>& dims,
                          std::vector<size_t>& existing) {
  const RefinementIndexSet& set = state.indexSet;
  dims.clear();
  existing.clear();

  for (size_t dim = 0; dim < set.nDim(); dim++) {
    const LvlMIView mi = set.levels(idx);
    if (mi[dim] >= maxLevel) continue;
    const size_t found = set.findNeighbour(mi, set.hash(idx), dim, 1);
    if (found != RefinementIndexSet::NOT_FOUND &&
        state.states[found] != RefinementIndexState::Speculative) {
      continue;  // in the grid or pending already
    }
    neighbour.resize(mi.size());
    std::copy(mi.data(), mi.data() + mi.size(), neighbour.begin());
    neighbour[dim]++;
    const uint64_t hash = set.hash(idx) - RefinementIndexSet::entryHash(dim, mi[dim]) +
                          RefinementIndexSet::entryHash(dim, mi[dim] + 1);
    if (otherBackwardNeighboursOld(state, neighbour, hash, dim)) {
      dims.push_back(dim);
      existing.push_back(found);
    }
  }
}

double errorEstimateOf(const RefinementState& state) {
  CompensatedSum<double> sum;
  for (size_t idx = 0; idx < state.states.size(); idx++) {
    if (state.states[idx] != RefinementIndexState::Active) continue;
    const double gain = state.gains[idx];
    if (std::isnan(gain)) {
      return gain;
    }
    if (gain < 0.0) {  // not computed yet: the grid has not been evaluated
      return std::numeric_limits<double>::quiet_NaN();
    }
    sum.add(gain);
  }
  return sum.value();
}

}  // namespace refinement
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
