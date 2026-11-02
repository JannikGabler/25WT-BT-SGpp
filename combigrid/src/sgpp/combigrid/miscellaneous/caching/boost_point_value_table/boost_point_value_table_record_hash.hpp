// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file boost_point_value_table_record_hash.hpp
 * @brief Hash of the record indices in the index of a @c misc::BoostPointValueTable.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table_probe.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table_record_arena.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace boost_point_value_table {

/// @brief Hash of the record indices of an index: the hash stored in the record (XXH3, so no
/// further mixing by the index).
struct RecordHash {
  using is_transparent = void;
  using is_avalanching = std::true_type;

  const RecordArena* arena;

  size_t operator()(const uint32_t idx) const { return arena->record(idx)[0]; }
  size_t operator()(const Probe& probe) const { return probe.hash; }
};

}  // namespace boost_point_value_table
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
