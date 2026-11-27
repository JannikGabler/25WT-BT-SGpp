// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_index_state.hpp
 * @brief State of a multi-index of the dimension-adaptive refinement.
 */
#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief State of a multi-index of a dimension-adaptive refinement (Gerstner-Griebel: old /
 * active sets). Only active and old multi-indices belong to the refined grid.
 */
enum class RefinementIndexState : uint8_t {
  /// Admissible candidate (all backward neighbours old), not in the grid until it is evaluated.
  Pending,
  /// Evaluated (or being evaluated) ahead of the decisions; not admissible yet, not in the grid.
  Speculative,
  /// In the grid, not refined yet: a candidate of the next decisions.
  Active,
  /// In the grid and refined: its admissible forward neighbours have been added.
  Old
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
