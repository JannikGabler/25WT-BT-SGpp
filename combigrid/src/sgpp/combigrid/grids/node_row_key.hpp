// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file node_row_key.hpp
 * @brief Identifies a 1D node list ("row") of a @c NodeStorage.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Identifies a 1D node list ("row") of a @c NodeStorage: everything its nodes depend on.
 *
 * A row always holds its nodes **with** the boundary nodes 0 and 1 (mapped onto the interval),
 * in the order of the node family; the list without them is the row without its first and last
 * node (@c NodeFamily builds the list with boundary as 0, the list without boundary, 1).
 *
 * - Regular rows (@ref regular): the nodes of a node family with a number of inner nodes, as a
 *   @c SparseGrid uses them.
 * - Delta rows (@ref delta): the nodes a level adds to the previous level of a nested node family
 *   with a level-to-grid-point-count function, as a @c DeltasSparseGrid uses them.
 * - The interval: the nodes mapped from the unit interval with @c tools::mapToDomain; the unit
 *   interval [0, 1] gives the unit nodes themselves (the mapping is the identity there), so a
 *   grid on the unit cube uses the same rows for its unit and its domain nodes.
 *
 * Keys compare the interval bitwise.
 */
struct NodeRowKey {
  /// @brief Returns the key of the regular row of @p nodeFamily with @p nInnerNodes inner nodes on
  /// the unit interval.
  static NodeRowKey regular(const NodeFamily* nodeFamily, GPCntType nInnerNodes) noexcept;

  /// @brief Returns the key of the delta row of level @p lvl of @p nodeFamily with growth
  /// @p lvl2GPCntFunc on the unit interval.
  static NodeRowKey delta(const NodeFamily* nodeFamily, Lvl2GPCntFunc lvl2GPCntFunc,
                          LvlType lvl) noexcept;

  /// @brief Returns this key with the interval [@p lower, @p upper].
  NodeRowKey onInterval(double lower, double upper) const noexcept;

  /// @brief Returns whether the interval is the unit interval [0, 1] (bitwise).
  bool isOnUnitInterval() const noexcept;

  /// @brief Returns a hash of the key.
  size_t hash() const noexcept;

  /// @brief Returns whether both keys denote the same row (intervals compared bitwise).
  bool operator==(const NodeRowKey& other) const noexcept;

  const NodeFamily* nodeFamily = nullptr;  ///< Node family.
  Lvl2GPCntFunc lvl2GPCntFunc = nullptr;   ///< Delta rows: growth function; regular rows: nullptr.
  uint64_t size = 0;                       ///< Regular rows: inner nodes; delta rows: level.
  double lower = 0.0;                      ///< Lower end of the interval.
  double upper = 1.0;                      ///< Upper end of the interval.
};

}  // namespace combigrid
}  // namespace sgpp

/// Hash of @c NodeRowKey for unordered containers.
template <>
struct std::hash<sgpp::combigrid::NodeRowKey> {
  size_t operator()(const sgpp::combigrid::NodeRowKey& key) const noexcept { return key.hash(); }
};
