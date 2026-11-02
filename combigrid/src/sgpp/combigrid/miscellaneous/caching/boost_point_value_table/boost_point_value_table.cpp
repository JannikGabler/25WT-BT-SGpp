// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table_shard.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

#include <mutex>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table_probe.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table_record_equal.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>
#include <stdexcept>
#include <vector>

// dependencies_defines.hpp included xxhash.h for its version check; with XXH_INLINE_ALL, xxhash.h
// may be included again and then defines XXH3 inline (no xxHash library at link or run time).
#define XXH_INLINE_ALL
#include <xxhash.h>

using sgpp::combigrid::misc::boost_point_value_table::Probe;
using sgpp::combigrid::misc::boost_point_value_table::Shard;
using sgpp::combigrid::misc::source_func_caching::RecordLayout;

namespace sgpp {
namespace combigrid {

namespace misc {

BoostPointValueTable::BoostPointValueTable(const RecordLayout layout, const size_t memoryLimit,
                                           const size_t nShards, LMDBPointValueStore* store,
                                           const bool storeAllRecords)
    : nShards_(nShards),
      layout_(layout),
      shardMemoryLimit_(0),
      store_(store),
      storeAllRecords_(storeAllRecords) {
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

bool BoostPointValueTable::insert(const double* point, const uint64_t hash, const double* values,
                                  const bool allowSpill) {
  Shard& s = shard(hash);
  {
    const std::lock_guard<SpinLock> guard(s.lock);

    if (s.index.contains(Probe{point, hash})) {
      return true;
    }
    if (s.arena.full() || s.bytes() + s.arena.bytesOfNextRecord() > shardMemoryLimit_) {
      [[unlikely]] if (!allowSpill || store_.load(std::memory_order_acquire) == nullptr) {
        s.rejectedInserts++;
        return false;
      }
    } else {
      const uint32_t idx = s.arena.allocate();
      uint64_t* const record = s.arena.record(idx);
      record[0] = hash;
      std::memcpy(record + 1, point, sizeof(double) * layout_.nDim);
      std::memcpy(record + 1 + layout_.nDim, values, sizeof(double) * layout_.nOutputs);
      s.index.insert(idx);
      s.inserts++;
      return true;
    }
  }

  // Memory is full and there is a disk store (out of line: the memory path stays as without it).
  return spill(s, point, hash, values);
}

bool BoostPointValueTable::spill(Shard& s, const double* point, const uint64_t hash,
                                 const double* values) {
  std::vector<uint64_t> batch;
  {
    const std::lock_guard<SpinLock> guard(s.lock);
    if (store_.load(std::memory_order_acquire)->full()) {
      s.rejectedInserts++;
      return false;
    }
    s.inserts++;
    if (!appendPending(s, point, hash, values)) {
      return true;
    }
    batch.swap(s.pending);
  }

  // Outside the lock: the batch is invisible until it is committed (a concurrent miss of one of
  // its points evaluates it again, the store keeps one value).
  writePending(batch);
  return true;
}

bool BoostPointValueTable::findOnDisk(const double* point, const uint64_t hash, double* values) {
  {
    // The shard's batch of records beyond the memory limit that are not written yet.
    Shard& s = shard(hash);
    const std::lock_guard<SpinLock> guard(s.lock);
    if (!s.pending.empty() && findPending(s, point, hash, values)) {
      s.hits++;
      return true;
    }
  }
  LMDBPointValueStore* const store = store_.load(std::memory_order_acquire);
  return store != nullptr && store->find(shardIndex(hash), layout_, hash, point, values);
}

bool BoostPointValueTable::insertLoaded(const uint64_t* record) {
  const double* const point = reinterpret_cast<const double*>(record + 1);
  Shard& s = shard(record[0]);
  const std::lock_guard<SpinLock> guard(s.lock);

  if (s.index.contains(Probe{point, record[0]})) {
    return true;
  }
  if (s.arena.full() || s.bytes() + s.arena.bytesOfNextRecord() > shardMemoryLimit_) {
    return false;
  }
  const uint32_t idx = s.arena.allocate();
  std::memcpy(s.arena.record(idx), record, layout_.bytes());
  s.index.insert(idx);
  return true;
}

void BoostPointValueTable::markFlushed() {
  for (const std::unique_ptr<Shard>& s : shards_) {
    const std::lock_guard<SpinLock> guard(s->lock);
    s->flushed = s->arena.size();
  }
}

void BoostPointValueTable::collectUnflushed(std::vector<const uint64_t*>& records,
                                            std::vector<uint64_t>& pendingRecords) {
  for (const std::unique_ptr<Shard>& s : shards_) {
    const std::lock_guard<SpinLock> guard(s->lock);
    if (storeAllRecords_) {
      // Records are immutable once inserted and their chunks never move.
      for (size_t idx = s->flushed; idx < s->arena.size(); idx++) {
        records.push_back(s->arena.record(static_cast<uint32_t>(idx)));
      }
      s->flushed = s->arena.size();
    }
    pendingRecords.insert(pendingRecords.end(), s->pending.begin(), s->pending.end());
    s->pending.clear();
  }

  for (size_t offset = 0; offset < pendingRecords.size(); offset += layout_.stride()) {
    records.push_back(pendingRecords.data() + offset);
  }
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

Shard& BoostPointValueTable::shard(const uint64_t hash) const { return *shards_[shardIndex(hash)]; }

bool BoostPointValueTable::appendPending(Shard& s, const double* point, const uint64_t hash,
                                         const double* values) {
  if (s.pending.empty()) {
    s.pending.reserve(layout_.stride() * constants::source_func_cache::SPILL_BATCH_RECORDS);
    markOnDisk();
  }
  const size_t offset = s.pending.size();
  s.pending.resize(offset + layout_.stride());
  uint64_t* const record = s.pending.data() + offset;
  record[0] = hash;
  std::memcpy(record + 1, point, sizeof(double) * layout_.nDim);
  std::memcpy(record + 1 + layout_.nDim, values, sizeof(double) * layout_.nOutputs);
  return s.pending.size() >= layout_.stride() * constants::source_func_cache::SPILL_BATCH_RECORDS;
}

bool BoostPointValueTable::findPending(const Shard& s, const double* point, const uint64_t hash,
                                       double* values) const {
  for (size_t offset = 0; offset < s.pending.size(); offset += layout_.stride()) {
    const uint64_t* const record = s.pending.data() + offset;
    if (boost_point_value_table::RecordEqual::holds(record, layout_.nDim, Probe{point, hash})) {
      std::memcpy(values, record + 1 + layout_.nDim, sizeof(double) * layout_.nOutputs);
      return true;
    }
  }
  return false;
}

void BoostPointValueTable::writePending(const std::vector<uint64_t>& records) {
  std::vector<const uint64_t*> pointers;
  pointers.reserve(records.size() / layout_.stride());
  for (size_t offset = 0; offset < records.size(); offset += layout_.stride()) {
    pointers.push_back(records.data() + offset);
  }
  store_.load(std::memory_order_acquire)->write(pointers, layout_);
}

/// @}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp

#else  // SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE

#include <sgpp/base/exception/dependency_exception.hpp>

using sgpp::combigrid::misc::source_func_caching::RecordLayout;

namespace sgpp {
namespace combigrid {

namespace misc {

// Without the libraries no table is ever created: SourceFuncCache throws when it is constructed,
// before any evaluation. These definitions only let the library link; the constructor throws
// as well.

BoostPointValueTable::BoostPointValueTable(const RecordLayout layout, const size_t /*memoryLimit*/,
                                           const size_t nShards, LMDBPointValueStore* store,
                                           const bool storeAllRecords)
    : nShards_(nShards),
      layout_(layout),
      shardMemoryLimit_(0),
      store_(store),
      storeAllRecords_(storeAllRecords) {
  throw base::dependency_exception(
      "BoostPointValueTable: SG++ was built without the libraries of the source-function "
      "cache. " SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION);
}

BoostPointValueTable::~BoostPointValueTable() = default;

uint64_t BoostPointValueTable::hash(const double* /*point*/, const size_t /*nDim*/) { return 0; }

bool BoostPointValueTable::find(const double* /*point*/, const uint64_t /*hash*/,
                                double* /*values*/) {
  return false;
}

bool BoostPointValueTable::insert(const double* /*point*/, const uint64_t /*hash*/,
                                  const double* /*values*/, const bool /*allowSpill*/) {
  return false;
}

bool BoostPointValueTable::findOnDisk(const double* /*point*/, const uint64_t /*hash*/,
                                      double* /*values*/) {
  return false;
}

bool BoostPointValueTable::insertLoaded(const uint64_t* /*record*/) { return false; }

void BoostPointValueTable::markFlushed() {}

void BoostPointValueTable::collectUnflushed(std::vector<const uint64_t*>& /*records*/,
                                            std::vector<uint64_t>& /*pendingRecords*/) {}

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
