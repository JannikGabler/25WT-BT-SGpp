// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#pragma once

#include <cstdint>
#include <cstring>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_probe.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_arena.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace boost_point_value_table {

/// @brief Equality of the record indices of an index: same hash and point.
struct RecordEqual {
  using is_transparent = void;

  const RecordArena* arena;

  bool operator()(const uint32_t a, const uint32_t b) const {
    return std::memcmp(arena->record(a), arena->record(b),
                       sizeof(uint64_t) * (1 + arena->layout().nDim)) == 0;
  }
  bool operator()(const Probe& probe, const uint32_t idx) const {
    return holds(arena->record(idx), arena->layout().nDim, probe);
  }
  bool operator()(const uint32_t idx, const Probe& probe) const {
    return holds(arena->record(idx), arena->layout().nDim, probe);
  }

  /// @brief Whether @p record holds the point of @p probe (bitwise).
  static bool holds(const uint64_t* record, const size_t nDim, const Probe& probe) {
    return record[0] == probe.hash &&
           std::memcmp(record + 1, probe.point, sizeof(double) * nDim) == 0;
  }
};

}  // namespace boost_point_value_table
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
