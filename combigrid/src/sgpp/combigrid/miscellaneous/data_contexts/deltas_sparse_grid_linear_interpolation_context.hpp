// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_linear_interpolation_context.hpp
 * @brief Defines @c DSGLinInterpContext, the read-only data of one call of the linear
 * interpolation operator on a deltas sparse grid: corner ids, 1D weights and coordinates of the
 * query point.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Read-only data of one call of the linear interpolation on a deltas sparse grid, shared
 * by all functions and threads: the query point's brackets and their ids (see
 * @c DSGLinInterpIndex for the identification of the corners with slots). Set before the passes
 * (@c tools::dsg_lin_interpolation::prepareContext), then only read; the values the passes compute
 * are in @c DSGLinInterpSlotScratch.
 *
 * Per row r = row(j, l) of the index, i.e. per (dimension j, level l), and bit b (0: lower,
 * 1: upper neighbor of the query coordinate in the nodes of level l):
 * - @ref cornerIds[2r + b]: the id of the corner coordinate (@ref encodeId: its level of first
 *   appearance λ, its index s among the coordinates new at λ and whether λ has two new
 *   coordinates, i.e. contributes a bit to the sub-index of the slot);
 * - @ref factors[2r + b]: its 1D weight in the tensor grids of level l.
 *
 * @ref coords[2 row(j, λ) + s] is the coordinate (on the domain) of the corner coordinate with
 * id (λ, s) in dimension j.
 */
struct DSGLinInterpContext {
  /**
   * @brief Allocates the data of a call (uninitialized).
   * @param index Index of the deltas sparse grid.
   */
  explicit DSGLinInterpContext(const DSGLinInterpIndex& index);

  /**
   * @brief Encodes a corner-coordinate id.
   * @param firstLvl Level λ at which the coordinate appears first.
   * @param twoNew   Whether two coordinates may appear first at λ (the id contributes one bit to
   *                 the sub-index of the slot).
   * @param s        Index (0 or 1) of the coordinate among those new at λ.
   */
  static constexpr uint32_t encodeId(const LvlType firstLvl, const bool twoNew, const uint32_t s) {
    return (static_cast<uint32_t>(firstLvl) << 2) | (static_cast<uint32_t>(twoNew) << 1) | s;
  }

  /// @brief Returns the level of first appearance λ of an id.
  static constexpr LvlType idLvl(const uint32_t id) { return id >> 2; }

  /// @brief Returns the number of bits (0 or 1) an id contributes to the sub-index of the slot.
  static constexpr uint32_t idShift(const uint32_t id) { return (id >> 1) & 1; }

  /// @brief Returns the index s of an id among the coordinates new at its level.
  static constexpr uint32_t idSub(const uint32_t id) { return id & 1; }

  std::vector<uint32_t> cornerIds;  ///< Per row and bit: corner-coordinate id.
  /// @brief Storage of @ref factors and @ref coords (one allocation).
  std::unique_ptr<double[]> doubles;
  std::span<double> factors;  ///< Per row and bit: 1D weight.
  std::span<double> coords;   ///< Per id: coordinate on the domain.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
