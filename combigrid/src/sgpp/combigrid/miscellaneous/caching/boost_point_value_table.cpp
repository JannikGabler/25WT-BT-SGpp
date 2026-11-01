// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstring>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_layout.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_shard.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <stdexcept>

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
#include <mutex>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_probe.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>

// dependencies_defines.hpp included xxhash.h for its version check; with XXH_INLINE_ALL, xxhash.h
// may be included again and then defines XXH3 inline (no xxHash library at link or run time).
#define XXH_INLINE_ALL
#include <xxhash.h>
#endif

using sgpp::combigrid::misc::boost_point_value_table::RecordLayout;
using sgpp::combigrid::misc::boost_point_value_table::Shard;

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

using sgpp::combigrid::misc::boost_point_value_table::Probe;

namespace sgpp {
namespace combigrid {

namespace misc {

BoostPointValueTable::BoostPointValueTable(const RecordLayout layout, const size_t memoryLimit,
                                           const size_t nShards)
    : nShards_(nShards), layout_(layout), shardMemoryLimit_(0) {
  checkShardsCnt(nShards);
  shardMemoryLimit_ = memoryLimit / nShards;
  shards_.reserve(nShards);
  for (size_t s = 0; s < nShards; s++) {
    shards_.push_back(std::make_unique<Shard>(layout));
  }
}

BoostPointValueTable::~BoostPointValueTable() = default;

uint64_t BoostPointValueTable::hash(const double* point, const size_t nDim) {
  return XXH3_64bits(point, sizeof(double) * nDim);
}

bool BoostPointValueTable::find(const double* point, const uint64_t hash, double* values) {
  Shard& s = shard(hash);
  const std::lock_guard<SpinLock> guard(s.lock);

  s.lookups++;
  const auto it = s.index.find(Probe{point, hash});
  if (it == s.index.end()) {
    return false;
  }

  s.hits++;
  std::memcpy(values, s.arena.record(*it) + 1 + layout_.nDim, sizeof(double) * layout_.nOutputs);
  return true;
}

void BoostPointValueTable::insert(const double* point, const uint64_t hash, const double* values) {
  Shard& s = shard(hash);
  const std::lock_guard<SpinLock> guard(s.lock);

  if (s.index.contains(Probe{point, hash})) {
    return;
  }
  if (s.arena.full() || s.bytes() + s.arena.bytesOfNextRecord() > shardMemoryLimit_) {
    s.rejectedInserts++;
    return;
  }

  const uint32_t idx = s.arena.allocate();
  uint64_t* const record = s.arena.record(idx);
  record[0] = hash;
  std::memcpy(record + 1, point, sizeof(double) * layout_.nDim);
  std::memcpy(record + 1 + layout_.nDim, values, sizeof(double) * layout_.nOutputs);
  s.index.insert(idx);
  s.inserts++;
}

CacheStatistics BoostPointValueTable::statistics() const {
  CacheStatistics stats;

  for (const std::unique_ptr<Shard>& s : shards_) {
    const std::lock_guard<SpinLock> guard(s->lock);
    stats.lookups += s->lookups;
    stats.hits += s->hits;
    stats.inserts += s->inserts;
    stats.rejectedInserts += s->rejectedInserts;
    stats.entries += s->index.size();
    stats.bytes += s->bytes();
  }

  return stats;
}

/// @name Private helpers
/// @{

Shard& BoostPointValueTable::shard(const uint64_t hash) const {
  return *shards_[(hash >> 24) & (nShards_ - 1)];
}

/// @}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#else  // SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

namespace sgpp {
namespace combigrid {

namespace misc {

// Without the libraries no table is ever created: SourceFuncCache throws when it is constructed,
// before any evaluation. These definitions only let the library link; the constructor throws
// as well.

BoostPointValueTable::BoostPointValueTable(const RecordLayout layout, const size_t /*memoryLimit*/,
                                           const size_t nShards)
    : nShards_(nShards), layout_(layout), shardMemoryLimit_(0) {
  throw base::dependency_exception(
      "BoostPointValueTable: SG++ was built without the libraries of the source-function cache. "
      SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION);
}

BoostPointValueTable::~BoostPointValueTable() = default;

uint64_t BoostPointValueTable::hash(const double* /*point*/, const size_t /*nDim*/) { return 0; }

bool BoostPointValueTable::find(const double* /*point*/, const uint64_t /*hash*/,
                                double* /*values*/) {
  return false;
}

void BoostPointValueTable::insert(const double* /*point*/, const uint64_t /*hash*/,
                                  const double* /*values*/) {}

CacheStatistics BoostPointValueTable::statistics() const { return CacheStatistics(); }

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

namespace sgpp {
namespace combigrid {
namespace misc {

void BoostPointValueTable::checkShardsCnt(const size_t nShards) {
  if (nShards == 0 || (nShards & (nShards - 1)) != 0) {
    throw std::invalid_argument("The number of shards must be a power of two.");
  } else if (nShards > 64) {
    throw std::invalid_argument("The number of shards can be at most 64.");
  }
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
