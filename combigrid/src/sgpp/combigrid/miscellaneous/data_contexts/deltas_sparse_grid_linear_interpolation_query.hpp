// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file dsg_lin_interp_query.hpp
 * @brief Defines @c DSGLinInterpQuery, the per-call data of the linear interpolation operator on
 * a deltas sparse grid: corner ids, 1D weights and coordinates of the query point and the value
 * store.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Per-call data of the linear interpolation on a deltas sparse grid (see
 * @c DSGLinInterpIndex for the identification of the corners with slots).
 *
 * Per row r = row(j, l) of the index, i.e. per (dimension j, level l), and bit b (0: lower,
 * 1: upper neighbor of the query coordinate in the nodes of level l):
 * - @ref cornerIds[2r + b]: the id of the corner coordinate (@ref encodeId: its level of first
 *   appearance λ, its index s among the coordinates new at λ and whether λ has two new
 *   coordinates, i.e. contributes a bit to the sub-index of the slot);
 * - @ref factors[2r + b]: its 1D weight in the tensor grids of level l.
 *
 * @ref coords[2 row(j, λ) + s] is the coordinate (on the domain) of the corner coordinate with
 * id (λ, s) in dimension j. @ref marks and @ref values are the value store: one mark (nonzero: a
 * corner of some tensor grid) and m values per slot.
 */
struct DSGLinInterpQuery {
  /**
   * @brief Allocates the data of a call (the marks are zero, the values uninitialized).
   * @param index    Index of the deltas sparse grid.
   * @param nOutputs Number of values per slot m.
   */
  DSGLinInterpQuery(const DSGLinInterpIndex& index, size_t nOutputs);

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

  /// @brief Returns the level of first appearance λ of an id (the child level in the trie).
  static constexpr LvlType idLvl(const uint32_t id) { return id >> 2; }

  /// @brief Returns the number of bits (0 or 1) an id contributes to the sub-index of the slot.
  static constexpr uint32_t idShift(const uint32_t id) { return (id >> 1) & 1; }

  /// @brief Returns the index s of an id among the coordinates new at its level.
  static constexpr uint32_t idSub(const uint32_t id) { return id & 1; }

  std::vector<uint32_t> cornerIds;   ///< Per row and bit: corner-coordinate id.
  std::vector<double> factors;       ///< Per row and bit: 1D weight.
  std::vector<double> coords;        ///< Per (dimension, λ) and s: coordinate on the domain.
  std::vector<uint8_t> marks;        ///< Per slot: nonzero if a corner uses it.
  std::unique_ptr<double[]> values;  ///< Per slot: m function values (written if marked).
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
