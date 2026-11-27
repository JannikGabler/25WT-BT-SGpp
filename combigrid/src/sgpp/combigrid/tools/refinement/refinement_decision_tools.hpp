// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_decision_tools.hpp
 * @brief Steps of the decisions of the dimension-adaptive refinement: costs, increments, gains,
 * priorities, new candidates, error estimate.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_state.hpp>
#include <sgpp/combigrid/refinement/refinement_indicator.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace refinement {

/**
 * @brief Returns the number of grid points of the tensor grid of @p mi (the product of the node
 * counts of the growth functions; saturates at the largest @c size_t).
 */
size_t tensorGridCost(const SparseGridSetup& setup, LvlMIView mi);

/**
 * @brief Returns the priority of a multi-index with @p gain and @p cost under @p indicator
 * (larger first; a gain that is not a number counts as 0).
 */
double priorityOf(double gain, size_t cost, RefinementIndicator indicator);

/**
 * @brief Computes the hierarchical increment of the evaluated multi-index @p idx,
 * @f$\Delta_{\vec{\ell}} = \sum_{\vec{z}} (-1)^{|\vec{z}|} Q_{\vec{\ell} - \vec{z}}@f$, from
 * the values of its backward box (all evaluated), with compensated summation in a fixed order.
 *
 * @param state     Refinement state.
 * @param idx       Multi-index.
 * @param sums      Scratch (one accumulator per value).
 * @param increment Output: the @c state.nValues values of the increment (not finished).
 * @param support   Scratch.
 */
void computeIncrement(const misc::RefinementState& state, size_t idx,
                      std::vector<numerics::CompensatedSum<double>>& sums,
                      std::vector<double>& increment, std::vector<size_t>& support);

/**
 * @brief Returns the gain of an increment: the max-norm of the increment finished by the target
 * (@c RefinementTarget::finishCombination).
 * @param increment Increment (not finished).
 * @param target    Refinement target.
 * @param scratch   Scratch for the finished increment.
 */
double gainOf(std::span<const double> increment, const RefinementTarget& target,
              std::vector<double>& scratch);

/**
 * @brief Collects the forward neighbours @f$\vec{\ell} + \vec{e}_k@f$ of the active multi-index
 * @p idx that become admissible once @p idx is old (every other backward neighbour old, level at
 * most @p maxLevel) and are neither in the grid nor pending.
 *
 * @param state     Refinement state.
 * @param idx       Active multi-index.
 * @param maxLevel  Largest admissible level per dimension.
 * @param neighbour Scratch multi-index.
 * @param dims      Output: the dimensions @f$k@f$ of the candidates.
 * @param existing  Output: per candidate its index in @c state, or
 *                  @c misc::RefinementIndexSet::NOT_FOUND if it is not in the set yet.
 */
void collectNewCandidates(const misc::RefinementState& state, size_t idx, LvlType maxLevel,
                          LvlMI& neighbour, std::vector<size_t>& dims,
                          std::vector<size_t>& existing);

/**
 * @brief Returns the error estimate: the sum of the gains of the active multi-indices
 * (compensated, in index order); NaN if a gain is not known yet (the grid has not been evaluated)
 * or not a number.
 */
double errorEstimateOf(const misc::RefinementState& state);

}  // namespace refinement
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
