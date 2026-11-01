// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file boost_point_value_table_shard.hpp
 * @brief Shard of the @c misc::BoostPointValueTable (lock, counters, record arena, Boost index).
 */
#pragma once

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
#include <boost/unordered/unordered_flat_set.hpp>
#endif
#include <cstddef>
#include <cstdint>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_arena.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_equal.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_hash.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_layout.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace boost_point_value_table {

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

/// @brief A shard: its lock, records, index and counters (all used under the lock). The counters
/// share the cache line of the lock, which every lookup writes anyway.
struct Shard {
  explicit Shard(const RecordLayout layout)
      : arena(layout), index(0, RecordHash{&arena}, RecordEqual{&arena}) {}

  /// @brief Bytes of the records and the index (groups of 15 slots of 4 bytes + 16 bytes of
  /// metadata).
  size_t bytes() const {
    const size_t nGroups = (index.bucket_count() + 1) / 15;
    return arena.allocatedBytes() + nGroups * (15 * sizeof(uint32_t) + 16);
  }

  alignas(64) SpinLock lock;
  size_t lookups = 0;
  size_t hits = 0;
  size_t inserts = 0;
  size_t rejectedInserts = 0;
  RecordArena arena;
  boost::unordered_flat_set<uint32_t, RecordHash, RecordEqual> index;
};

#else

/// @brief Without the libraries the table is never created (complete type, so that the table
/// still compiles).
struct Shard {};

#endif

}  // namespace boost_point_value_table
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
