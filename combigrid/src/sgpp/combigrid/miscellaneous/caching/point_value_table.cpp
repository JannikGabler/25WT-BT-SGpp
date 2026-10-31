// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/point_value_table.hpp>
#include <sgpp/combigrid/optional_libraries.hpp>

#ifdef SGPP_COMBIGRID_HAS_CACHE_LIBRARIES

#include <boost/unordered/unordered_flat_set.hpp>
#include <cstring>
#include <mutex>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>
#include <type_traits>

// optional_libraries.hpp included xxhash.h for its version check; with XXH_INLINE_ALL, xxhash.h
// may be included again and then defines XXH3 inline (no xxHash library at link or run time).
#define XXH_INLINE_ALL
#include <xxhash.h>

namespace sgpp {
namespace combigrid {

namespace misc {

namespace {

constexpr size_t N_SHARDS = constants::source_func_cache::N_SHARDS;

static_assert((N_SHARDS & (N_SHARDS - 1)) == 0 && N_SHARDS <= 64,
              "the shard is selected by up to 6 bits of the hash (bits 24..29)");

/// @brief Key of the heterogeneous lookups in the index: a point and its hash.
struct Probe {
  const double* point;
  uint64_t hash;
};

/// @brief Whether @p record holds the point of @p probe (bitwise).
bool holds(const uint64_t* record, const size_t nDim, const Probe& probe) {
  return record[0] == probe.hash &&
         std::memcmp(record + 1, probe.point, sizeof(double) * nDim) == 0;
}

/// @brief Hash of the record indices of an index: the hash stored in the record (XXH3, so no
/// further mixing by the index).
struct RecordHash {
  using is_transparent = void;
  using is_avalanching = std::true_type;

  const RecordArena* arena;

  size_t operator()(const uint32_t idx) const { return arena->record(idx)[0]; }
  size_t operator()(const Probe& probe) const { return probe.hash; }
};

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
};

}  // namespace

/// @brief A shard: its lock, records, index and counters (all used under the lock). The counters
/// share the cache line of the lock, which every lookup writes anyway.
struct PointValueTable::Shard {
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

PointValueTable::PointValueTable(const RecordLayout layout, const size_t memoryLimit)
    : layout_(layout), shardMemoryLimit_(memoryLimit / N_SHARDS) {
  shards_.reserve(N_SHARDS);
  for (size_t s = 0; s < N_SHARDS; s++) {
    shards_.push_back(std::make_unique<Shard>(layout));
  }
}

PointValueTable::~PointValueTable() = default;

uint64_t PointValueTable::hash(const double* point, const size_t nDim) {
  return XXH3_64bits(point, sizeof(double) * nDim);
}

bool PointValueTable::find(const double* point, const uint64_t hash, double* values) {
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

void PointValueTable::insert(const double* point, const uint64_t hash, const double* values) {
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

CacheStatistics PointValueTable::statistics() const {
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

PointValueTable::Shard& PointValueTable::shard(const uint64_t hash) const {
  return *shards_[(hash >> 24) & (N_SHARDS - 1)];
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#else  // SGPP_COMBIGRID_HAS_CACHE_LIBRARIES

namespace sgpp {
namespace combigrid {
namespace misc {

// Without the libraries no table is ever created: the constructors of SourceFuncCache and of the
// cached functions throw before (SourceFuncCache::requireAvailable). These definitions only let
// the library link; the constructor throws as well.

struct PointValueTable::Shard {};

PointValueTable::PointValueTable(const RecordLayout layout, const size_t memoryLimit)
    : layout_(layout), shardMemoryLimit_(memoryLimit) {
  SourceFuncCache::requireAvailable();
}

PointValueTable::~PointValueTable() = default;

uint64_t PointValueTable::hash(const double* /*point*/, const size_t /*nDim*/) { return 0; }

bool PointValueTable::find(const double* /*point*/, const uint64_t /*hash*/, double* /*values*/) {
  return false;
}

void PointValueTable::insert(const double* /*point*/, const uint64_t /*hash*/,
                             const double* /*values*/) {}

CacheStatistics PointValueTable::statistics() const { return CacheStatistics(); }

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_HAS_CACHE_LIBRARIES
