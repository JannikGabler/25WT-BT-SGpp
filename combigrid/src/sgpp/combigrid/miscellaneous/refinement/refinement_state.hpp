// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_state.hpp
 * @brief What a dimension-adaptive refinement keeps between its calls.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_state.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief What a dimension-adaptive refinement keeps between its calls: the index set (the grid's
 * downwards-closed set, the pending candidates and the multi-indices evaluated ahead) and per
 * multi-index its state, combination coefficient, target values, gain and cost.
 *
 * The values of a multi-index are the @ref nValues values the target computed for its tensor
 * grid; the gain is the max-norm of its finished increment (negative until it is computed).
 */
struct RefinementState {
  /// @brief Constructs the empty state for @p nDim dimensions and @p nValues values per tensor
  /// grid.
  RefinementState(size_t nDim, size_t nValues);

  /**
   * @brief Appends multi-index @p mi (must not be in the set) with @p state, coefficient 0, not
   * evaluated; returns its index.
   */
  size_t add(LvlMIView mi, RefinementIndexState state, size_t cost);

  /// @brief Returns the values of multi-index @p idx (valid until the next @ref add).
  std::span<double> valuesOf(const size_t idx) noexcept {
    return {values.data() + idx * nValues, nValues};
  }

  /// @brief Returns the values of multi-index @p idx (valid until the next @ref add).
  std::span<const double> valuesOf(const size_t idx) const noexcept {
    return {values.data() + idx * nValues, nValues};
  }

  /// @brief Returns whether multi-index @p idx belongs to the grid (active or old).
  bool isInGrid(const size_t idx) const noexcept {
    return states[idx] == RefinementIndexState::Active || states[idx] == RefinementIndexState::Old;
  }

  size_t nValues;                            ///< Values per tensor grid (of the target).
  RefinementIndexSet indexSet;               ///< All multi-indices, in insertion order.
  std::vector<RefinementIndexState> states;  ///< Per multi-index: its state.
  std::vector<uint8_t> evaluated;            ///< Per multi-index: whether its values are known.
  std::vector<CTCoeffType> coefficients;     ///< Per multi-index: coefficient in the grid.
  std::vector<double> values;                ///< @ref nValues values per multi-index.
  std::vector<double> gains;                 ///< Per multi-index: gain (negative: not computed).
  std::vector<size_t> costs;                 ///< Per multi-index: grid points of its tensor grid.
  /// Storage version of the grid after the last call (0: never synchronized with a grid).
  uint64_t gridVersion = 0;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
