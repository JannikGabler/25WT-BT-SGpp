// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_cache_proto.hpp
 * @brief WP-25 prototypes of a source-function cache (comparison benchmark, not production code).
 *
 * Everything lives in this header so that all designs are compiled alike (judged flags, LTO):
 * - keys: the bit patterns of the user-domain point (the @c base::DataVector the operators pass to
 *   the user function), hashed with @ref hashPoint (wyhash-style products of independent word
 *   pairs) or with the removed prototype's @c misc::DataVectorHash;
 * - records: fixed size, @c [hash | d key doubles | m value doubles] (m = 1 for @c SourceFunc,
 *   m outputs for a @c VectorSourceFunc, WP-24), stored in chunked arenas; the hash tables only
 *   hold 4-byte record indices (heterogeneous lookup with a @ref Probe), so the bytes per entry
 *   are 8 (d + m + 1) + index;
 * - tables (all thread-safe for the operators' OpenMP loops): @ref OldProtoTable (the prototype
 *   removed in 6dfcf971a: 256 mutex shards of @c std::unordered_map<DataVector, double>),
 *   @ref ShardedTable (S shards, each a lock + arena + @c boost::unordered_flat_set),
 *   @ref BoostConcurrentTable (@c boost::concurrent_flat_set + concurrent arena),
 *   @ref PhmapTable (@c phmap::parallel_flat_hash_set, only if installed), @ref SolTable
 *   (fixed-capacity lock-free open addressing: speed-of-light reference, no growth),
 *   @ref PerThreadTable (one lock-free table per thread, no sharing: lower bound of a
 *   per-thread-and-merge design within one operator call);
 * - @ref makeCachedSourceFunc: a plain @c SourceFunc whose callable looks the point up before
 *   calling the user function (the "wrapper" design: today's @c SourceFunc and operators stay
 *   unchanged, policy "none" costs nothing), with a run-time bypass mode (adaptive policy
 *   deciding not to cache);
 * - @ref CostedFunc: an e2e integrand plus a calibrated dependent FMA chain of configurable
 *   duration; the chain is scaled by 1e-300 and does not change the value (bitwise);
 * - @ref FlatFile: the disk tier as an append-only file of records behind a header.
 */
#pragma once

#include <fcntl.h>
#include <malloc.h>
#include <omp.h>
#include <sys/stat.h>
#include <unistd.h>

// Boost's concurrent containers include <execution>, whose libstdc++ implementation uses TBB
// (and needs -ltbb) as soon as TBB headers are installed: not needed here.
#define BOOST_UNORDERED_DISABLE_PARALLEL_ALGORITHMS

#include <algorithm>
#include <atomic>
#include <bit>
#include <boost/unordered/concurrent_flat_set.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/hashing/data_vector_hash.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#if __has_include(<parallel_hashmap/phmap.h>)
#include <parallel_hashmap/phmap.h>
#define PERF_CACHE_HAVE_PHMAP 1
#endif

#if __has_include(<xxhash.h>)
#define XXH_INLINE_ALL
#include <xxhash.h>
#define PERF_CACHE_HAVE_XXH3 1
#endif

#include "perf_harness.hpp"
#include "perf_problems.hpp"

namespace sgpp {
namespace combigrid {
namespace perf {
namespace cache_proto {

/*******************************************************************************
Hashing
*******************************************************************************/

inline uint64_t bitsOf(const double x) { return std::bit_cast<uint64_t>(x); }

/// @brief 64 x 64 -> 128 bit product, folded (wyhash's "mum").
__extension__ typedef unsigned __int128 Uint128;

inline uint64_t mum(const uint64_t a, const uint64_t b) {
  const Uint128 r = static_cast<Uint128>(a) * b;
  return static_cast<uint64_t>(r) ^ static_cast<uint64_t>(r >> 64);
}

constexpr uint64_t SECRET0 = 0xa0761d6478bd642full;
constexpr uint64_t SECRET1 = 0xe7037ed1a0b428dbull;
constexpr uint64_t SECRET2 = 0x8ebc6af09c88c6e3ull;
constexpr uint64_t SECRET3 = 0x589965cc75374cc3ull;

/**
 * @brief Hash of the bit patterns of @p x[0..d): one product per pair of words, the products are
 * independent (no dependency chain over d) and summed; position-dependent secrets make the hash
 * order-sensitive. Bitwise key: -0.0 and 0.0 differ.
 */
inline uint64_t hashPoint(const double* x, const size_t d) {
  uint64_t acc = SECRET0 ^ (static_cast<uint64_t>(d) * SECRET1);
  size_t k = 0;
  for (; k + 2 <= d; k += 2) {
    acc += mum(bitsOf(x[k]) ^ (SECRET2 + k * SECRET3), bitsOf(x[k + 1]) ^ SECRET1);
  }
  const uint64_t last = (k < d) ? bitsOf(x[k]) : 0;
  return mum(acc ^ SECRET3, last ^ SECRET2);
}

/// @brief The removed prototype's hash (@c misc::DataVectorHash, FNV-style combine).
inline uint64_t hashPointFnv(const base::DataVector& x) { return misc::DataVectorHash()(x); }

#ifdef PERF_CACHE_HAVE_XXH3
inline uint64_t hashPointXxh3(const double* x, const size_t d) {
  return XXH3_64bits(x, d * sizeof(double));
}
#endif

/*******************************************************************************
Records and arenas
*******************************************************************************/

/// @brief Layout of a record: @c [hash | key (d doubles) | value (m doubles)], in 8-byte words.
struct RecordLayout {
  size_t d;
  size_t m;
  size_t stride() const { return 1 + d + m; }
  size_t bytes() const { return 8 * stride(); }
};

/// @brief Chunked arena of records for one owner (no synchronization); indices are stable.
class ChunkedArena {
 public:
  // 128 records per chunk: a sharded table has one arena per shard, so small caches must not
  // pay a large chunk per shard (64 shards x 4096 records = 2.5 KB per entry at n = 1e4).
  static constexpr size_t CHUNK_BITS = 7;

  explicit ChunkedArena(const RecordLayout layout) : layout_(layout) {}

  uint32_t allocate() {
    if ((n_ >> CHUNK_BITS) == chunks_.size()) {
      chunks_.emplace_back(new uint64_t[layout_.stride() << CHUNK_BITS]);
    }
    return static_cast<uint32_t>(n_++);
  }

  uint64_t* record(const uint32_t idx) const {
    return chunks_[idx >> CHUNK_BITS].get() + (idx & ((1u << CHUNK_BITS) - 1)) * layout_.stride();
  }

  size_t size() const { return n_; }
  size_t allocatedBytes() const { return chunks_.size() * (layout_.bytes() << CHUNK_BITS); }

 private:
  RecordLayout layout_;
  std::vector<std::unique_ptr<uint64_t[]>> chunks_;
  size_t n_ = 0;
};

/**
 * @brief Arena shared by all threads: one @c fetch_add per record, chunks allocated on first
 * touch under a mutex. A record is written before its index is published in a table, whose
 * synchronization orders the write before every read.
 */
class ConcurrentArena {
 public:
  static constexpr size_t CHUNK_BITS = 12;
  static constexpr size_t MAX_CHUNKS = size_t{1} << 16;  // 2.7e8 records

  explicit ConcurrentArena(const RecordLayout layout)
      : layout_(layout), chunks_(new std::atomic<uint64_t*>[MAX_CHUNKS]) {}  // value-initialized

  ~ConcurrentArena() {
    for (size_t c = 0; c < MAX_CHUNKS; c++) delete[] chunks_[c].load(std::memory_order_relaxed);
  }

  uint32_t allocate() {
    const uint64_t idx = next_.fetch_add(1, std::memory_order_relaxed);
    const size_t c = idx >> CHUNK_BITS;
    if (chunks_[c].load(std::memory_order_acquire) == nullptr) {
      std::lock_guard<std::mutex> guard(chunkMutex_);
      if (chunks_[c].load(std::memory_order_relaxed) == nullptr) {
        chunks_[c].store(new uint64_t[layout_.stride() << CHUNK_BITS], std::memory_order_release);
        nChunks_.fetch_add(1, std::memory_order_relaxed);
      }
    }
    return static_cast<uint32_t>(idx);
  }

  uint64_t* record(const uint32_t idx) const {
    return chunks_[idx >> CHUNK_BITS].load(std::memory_order_acquire) +
           (idx & ((1u << CHUNK_BITS) - 1)) * layout_.stride();
  }

  size_t size() const { return next_.load(std::memory_order_relaxed); }
  size_t allocatedBytes() const {
    return nChunks_.load(std::memory_order_relaxed) * (layout_.bytes() << CHUNK_BITS);
  }

 private:
  RecordLayout layout_;
  std::unique_ptr<std::atomic<uint64_t*>[]> chunks_;
  std::atomic<uint64_t> next_{0};
  std::atomic<size_t> nChunks_{0};
  std::mutex chunkMutex_;
};

/// @brief Lookup key of the heterogeneous table lookups: the point and its hash.
struct Probe {
  const double* x;
  uint64_t hash;
};

inline void writeRecord(uint64_t* record, const RecordLayout layout, const double* x,
                        const uint64_t hash, const double* value) {
  record[0] = hash;
  std::memcpy(record + 1, x, 8 * layout.d);
  std::memcpy(record + 1 + layout.d, value, 8 * layout.m);
}

inline bool recordMatches(const uint64_t* record, const RecordLayout layout, const Probe& p) {
  return record[0] == p.hash && std::memcmp(record + 1, p.x, 8 * layout.d) == 0;
}

/// @brief Hash / equality of record indices in an arena (heterogeneous with @ref Probe).
template <typename Arena>
struct IdxHash {
  using is_transparent = void;
  using is_avalanching = std::true_type;
  const Arena* arena;
  size_t operator()(const uint32_t idx) const { return arena->record(idx)[0]; }
  size_t operator()(const Probe& p) const { return p.hash; }
};

template <typename Arena>
struct IdxEq {
  using is_transparent = void;
  const Arena* arena;
  RecordLayout layout;
  bool operator()(const uint32_t a, const uint32_t b) const {
    const uint64_t* ra = arena->record(a);
    return std::memcmp(ra, arena->record(b), 8 * (1 + layout.d)) == 0;
  }
  bool operator()(const Probe& p, const uint32_t b) const {
    return recordMatches(arena->record(b), layout, p);
  }
  bool operator()(const uint32_t a, const Probe& p) const {
    return recordMatches(arena->record(a), layout, p);
  }
};

/*******************************************************************************
Locks
*******************************************************************************/

/// @brief Test-and-test-and-set spin lock (critical sections of the tables are ~10-100 ns).
class SpinLock {
 public:
  void lock() {
    while (flag_.exchange(true, std::memory_order_acquire)) {
      while (flag_.load(std::memory_order_relaxed)) {
#if defined(__x86_64__)
        __builtin_ia32_pause();
#endif
      }
    }
  }
  void unlock() { flag_.store(false, std::memory_order_release); }

 private:
  std::atomic<bool> flag_{false};
};

/*******************************************************************************
Tables. Interface: find(x, hash, out) -> bool, insert(x, hash, value) -> bool (false if present
or rejected by the memory limit), size(), bytes() (own accounting).
*******************************************************************************/

/// @brief The prototype removed in 6dfcf971a (256 mutex shards, node-based map, DataVector keys).
class OldProtoTable {
 public:
  static constexpr bool OWN_HASH = true;  // hashes the DataVector itself (as the prototype)

  OldProtoTable(const RecordLayout layout, size_t /*capacityHint*/, size_t memLimit)
      : layout_(layout), shards_(256), memLimit_(memLimit) {}

  bool find(const base::DataVector& x, uint64_t /*hash*/, double* out) {
    Shard& s = shards_[misc::DataVectorHash()(x) % shards_.size()];
    std::lock_guard<std::mutex> guard(s.mutex);
    const auto it = s.map.find(x);
    if (it == s.map.end()) return false;
    out[0] = it->second;
    return true;
  }

  bool insert(const base::DataVector& x, uint64_t /*hash*/, const double* value) {
    Shard& s = shards_[misc::DataVectorHash()(x) % shards_.size()];
    std::lock_guard<std::mutex> guard(s.mutex);
    // ~ node (64-byte aligned by base's operator new) + key heap block + bucket pointer
    const size_t entryBytes = 128 + 8 * layout_.d + 8;
    if (s.bytes + entryBytes > memLimit_ / shards_.size()) return false;
    if (!s.map.emplace(x, value[0]).second) return false;
    s.bytes += entryBytes;
    return true;
  }

  size_t size() const {
    size_t n = 0;
    for (const Shard& s : shards_) n += s.map.size();
    return n;
  }

  /// @brief Largest shard / mean shard size (1 = even distribution of the keys).
  double shardImbalance() const {
    size_t largest = 0;
    for (const Shard& s : shards_) largest = std::max(largest, s.map.size());
    return static_cast<double>(largest * shards_.size()) / static_cast<double>(size());
  }

 private:
  struct Shard {
    std::mutex mutex;
    std::unordered_map<base::DataVector, double, misc::DataVectorHash> map;
    size_t bytes = 0;
  };
  RecordLayout layout_;
  std::vector<Shard> shards_;
  size_t memLimit_;
};

/**
 * @brief S shards (selected by hash bits 24..), each a lock, an own arena and a
 * @c boost::unordered_flat_set of record indices. The memory limit is split evenly.
 */
template <typename Lock>
class ShardedTable {
 public:
  ShardedTable(const RecordLayout layout, const size_t capacityHint, const size_t memLimit,
               const size_t nShards = 64)
      : layout_(layout), nShards_(nShards), shardMemLimit_(memLimit / nShards) {
    shards_ = std::make_unique<std::optional<Shard>[]>(nShards);
    for (size_t s = 0; s < nShards; s++) {
      shards_[s].emplace(layout);
      if (capacityHint > 0) shards_[s]->set.reserve(capacityHint / nShards + 1);
    }
  }

  bool find(const base::DataVector& x, const uint64_t hash, double* out) {
    Shard& s = shard(hash);
    std::lock_guard<Lock> guard(s.lock);
    const auto it = s.set.find(Probe{x.data(), hash});
    if (it == s.set.end()) return false;
    std::memcpy(out, s.arena.record(*it) + 1 + layout_.d, 8 * layout_.m);
    return true;
  }

  bool insert(const base::DataVector& x, const uint64_t hash, const double* value) {
    Shard& s = shard(hash);
    std::lock_guard<Lock> guard(s.lock);
    if (s.set.find(Probe{x.data(), hash}) != s.set.end()) return false;
    if (s.bytes() + layout_.bytes() > shardMemLimit_) return false;
    const uint32_t idx = s.arena.allocate();
    writeRecord(s.arena.record(idx), layout_, x.data(), hash, value);
    s.set.insert(idx);
    return true;
  }

  size_t size() const {
    size_t n = 0;
    for (size_t s = 0; s < nShards_; s++) n += shards_[s]->arena.size();
    return n;
  }

  size_t bytes() const {
    size_t n = 0;
    for (size_t s = 0; s < nShards_; s++) n += shards_[s]->bytes();
    return n;
  }

 private:
  struct Shard {
    explicit Shard(const RecordLayout layout)
        : arena(layout),
          set(0, IdxHash<ChunkedArena>{&arena}, IdxEq<ChunkedArena>{&arena, layout}) {}
    // Records (in whole chunks) + slots (4 B) + group metadata (16 B per 15 slots)
    size_t bytes() const {
      return arena.allocatedBytes() +
             static_cast<size_t>(static_cast<double>(set.bucket_count()) * (4 + 16.0 / 15));
    }

    alignas(64) Lock lock;
    ChunkedArena arena;
    boost::unordered_flat_set<uint32_t, IdxHash<ChunkedArena>, IdxEq<ChunkedArena>> set;
  };

  Shard& shard(const uint64_t hash) { return *shards_[(hash >> 24) & (nShards_ - 1)]; }

  RecordLayout layout_;
  size_t nShards_;
  size_t shardMemLimit_;
  std::unique_ptr<std::optional<Shard>[]> shards_;
};

/// @brief @c boost::concurrent_flat_set of record indices + a concurrent arena.
class BoostConcurrentTable {
 public:
  BoostConcurrentTable(const RecordLayout layout, const size_t capacityHint, const size_t memLimit)
      : layout_(layout),
        arena_(layout),
        set_(capacityHint, IdxHash<ConcurrentArena>{&arena_},
             IdxEq<ConcurrentArena>{&arena_, layout}),
        memLimit_(memLimit) {}

  bool find(const base::DataVector& x, const uint64_t hash, double* out) {
    const uint64_t* record = nullptr;
    set_.visit(Probe{x.data(), hash}, [&](const uint32_t idx) { record = arena_.record(idx); });
    if (record == nullptr) return false;
    std::memcpy(out, record + 1 + layout_.d, 8 * layout_.m);
    return true;
  }

  bool insert(const base::DataVector& x, const uint64_t hash, const double* value) {
    if (set_.contains(Probe{x.data(), hash})) return false;
    if (bytes() + layout_.bytes() > memLimit_) return false;
    const uint32_t idx = arena_.allocate();
    writeRecord(arena_.record(idx), layout_, x.data(), hash, value);
    if (!set_.insert(idx)) {
      wasted_.fetch_add(1, std::memory_order_relaxed);  // inserted concurrently by another thread
      return false;
    }
    return true;
  }

  size_t size() const { return set_.size(); }
  size_t bytes() const {
    return arena_.allocatedBytes() +
           static_cast<size_t>(static_cast<double>(set_.bucket_count()) * (4 + 16.0 / 15));
  }
  size_t wasted() const { return wasted_.load(); }

 private:
  RecordLayout layout_;
  ConcurrentArena arena_;
  boost::concurrent_flat_set<uint32_t, IdxHash<ConcurrentArena>, IdxEq<ConcurrentArena>> set_;
  size_t memLimit_;
  std::atomic<size_t> wasted_{0};
};

#ifdef PERF_CACHE_HAVE_PHMAP
/// @brief @c phmap::parallel_flat_hash_set (2^6 submaps, each with a @c std::mutex) of indices.
class PhmapTable {
 public:
  PhmapTable(const RecordLayout layout, const size_t capacityHint, const size_t memLimit)
      : layout_(layout),
        arena_(layout),
        set_(capacityHint, IdxHash<ConcurrentArena>{&arena_},
             IdxEq<ConcurrentArena>{&arena_, layout}),
        memLimit_(memLimit) {}

  bool find(const base::DataVector& x, const uint64_t hash, double* out) {
    const uint64_t* record = nullptr;
    set_.if_contains(Probe{x.data(), hash},
                     [&](const uint32_t idx) { record = arena_.record(idx); });
    if (record == nullptr) return false;
    std::memcpy(out, record + 1 + layout_.d, 8 * layout_.m);
    return true;
  }

  bool insert(const base::DataVector& x, const uint64_t hash, const double* value) {
    if (set_.contains(Probe{x.data(), hash})) return false;
    if (arena_.allocatedBytes() + layout_.bytes() > memLimit_) return false;
    const uint32_t idx = arena_.allocate();
    writeRecord(arena_.record(idx), layout_, x.data(), hash, value);
    return set_.insert(idx).second;
  }

  size_t size() const { return set_.size(); }
  size_t bytes() const { return arena_.allocatedBytes() + set_.capacity() * 5; }

 private:
  RecordLayout layout_;
  ConcurrentArena arena_;
  phmap::parallel_flat_hash_set<uint32_t, IdxHash<ConcurrentArena>, IdxEq<ConcurrentArena>,
                                std::allocator<uint32_t>, 6, std::mutex>
      set_;
  size_t memLimit_;
};
#endif

/**
 * @brief Speed-of-light reference: fixed capacity (power of two >= 2 * capacityHint), lock-free
 * linear probing over 8-byte slots @c (fingerprint << 32 | index + 1) published with one CAS.
 * No growth: inserts beyond the capacity are rejected.
 */
class SolTable {
 public:
  static constexpr bool FIXED_CAPACITY = true;

  SolTable(const RecordLayout layout, const size_t capacityHint, size_t /*memLimit*/)
      : layout_(layout), arena_(layout) {
    capacity_ = std::bit_ceil(std::max<size_t>(2 * capacityHint, 1024));
    slots_.reset(new std::atomic<uint64_t>[capacity_]);
    for (size_t i = 0; i < capacity_; i++) slots_[i].store(0, std::memory_order_relaxed);
  }

  bool find(const base::DataVector& x, const uint64_t hash, double* out) {
    const Probe p{x.data(), hash};
    const uint64_t fp = fingerprint(hash);
    for (size_t i = hash & (capacity_ - 1);; i = (i + 1) & (capacity_ - 1)) {
      const uint64_t slot = slots_[i].load(std::memory_order_acquire);
      if (slot == 0) return false;
      if ((slot >> 32) == fp) {
        const uint64_t* record = arena_.record(static_cast<uint32_t>(slot) - 1);
        if (recordMatches(record, layout_, p)) {
          std::memcpy(out, record + 1 + layout_.d, 8 * layout_.m);
          return true;
        }
      }
    }
  }

  bool insert(const base::DataVector& x, const uint64_t hash, const double* value) {
    if (arena_.size() >= capacity_ / 2) return false;
    const Probe p{x.data(), hash};
    const uint64_t fp = fingerprint(hash);
    const uint32_t idx = arena_.allocate();
    writeRecord(arena_.record(idx), layout_, x.data(), hash, value);
    const uint64_t desired = (fp << 32) | (static_cast<uint64_t>(idx) + 1);
    for (size_t i = hash & (capacity_ - 1);; i = (i + 1) & (capacity_ - 1)) {
      uint64_t slot = slots_[i].load(std::memory_order_acquire);
      if (slot == 0 && slots_[i].compare_exchange_strong(slot, desired, std::memory_order_acq_rel,
                                                         std::memory_order_acquire)) {
        return true;
      }
      if ((slot >> 32) == fp &&
          recordMatches(arena_.record(static_cast<uint32_t>(slot) - 1), layout_, p)) {
        return false;  // inserted concurrently: the record is wasted
      }
    }
  }

  size_t size() const { return arena_.size(); }
  size_t bytes() const { return arena_.allocatedBytes() + 8 * capacity_; }

 private:
  static uint64_t fingerprint(const uint64_t hash) { return (hash >> 32) | 1; }

  RecordLayout layout_;
  ConcurrentArena arena_;
  size_t capacity_;
  std::unique_ptr<std::atomic<uint64_t>[]> slots_;
};

/// @brief One unsynchronized table per OpenMP thread: hits only on points the same thread saw.
class PerThreadTable {
 public:
  PerThreadTable(const RecordLayout layout, const size_t capacityHint, const size_t memLimit)
      : layout_(layout) {
    const size_t nThreads = static_cast<size_t>(omp_get_max_threads());
    tables_.reserve(nThreads);
    for (size_t t = 0; t < nThreads; t++) {
      tables_.push_back(std::make_unique<ShardedTable<NoLock>>(layout, capacityHint / nThreads,
                                                               memLimit / nThreads, 1));
    }
  }

  bool find(const base::DataVector& x, const uint64_t hash, double* out) {
    return tables_[omp_get_thread_num()]->find(x, hash, out);
  }
  bool insert(const base::DataVector& x, const uint64_t hash, const double* value) {
    return tables_[omp_get_thread_num()]->insert(x, hash, value);
  }
  size_t size() const {
    size_t n = 0;
    for (const auto& t : tables_) n += t->size();
    return n;
  }

 private:
  struct NoLock {
    void lock() {}
    void unlock() {}
  };
  RecordLayout layout_;
  std::vector<std::unique_ptr<ShardedTable<NoLock>>> tables_;
};

/*******************************************************************************
Cached source function (wrapper design)
*******************************************************************************/

/// @brief Hit / miss statistics of a cached source function (padded per-thread counters).
struct CacheStats {
  struct alignas(64) PerThread {
    uint64_t lookups = 0;
    uint64_t hits = 0;
  };
  std::vector<PerThread> perThread = std::vector<PerThread>(omp_get_max_threads());

  uint64_t lookups() const {
    uint64_t n = 0;
    for (const PerThread& t : perThread) n += t.lookups;
    return n;
  }
  uint64_t hits() const {
    uint64_t n = 0;
    for (const PerThread& t : perThread) n += t.hits;
    return n;
  }
};

/**
 * @brief Wraps @p func into a plain @c SourceFunc that looks every point up in @p table first.
 * @p bypass (adaptive policy decided not to cache) is read per evaluation and skips the lookup.
 */
/// @brief Hash passed to the tables (0 for tables that hash the key themselves).
template <typename Table>
uint64_t keyHash(const base::DataVector& x) {
  if constexpr (requires { Table::OWN_HASH; }) {
    return 0;
  } else {
    return hashPoint(x.data(), x.size());
  }
}

template <typename Table>
SourceFunc makeCachedSourceFunc(std::function<double(const base::DataVector&)> func,
                                std::shared_ptr<Table> table, std::shared_ptr<CacheStats> stats,
                                std::shared_ptr<std::atomic<bool>> bypass = nullptr) {
  return SourceFunc([func = std::move(func), table = std::move(table), stats = std::move(stats),
                     bypass = std::move(bypass)](const base::DataVector& x) {
    if (bypass && bypass->load(std::memory_order_relaxed)) return func(x);
    CacheStats::PerThread& st = stats->perThread[omp_get_thread_num()];
    st.lookups++;
    const uint64_t hash = keyHash<Table>(x);
    double value;
    if (table->find(x, hash, &value)) {
      st.hits++;
      return value;
    }
    value = func(x);
    table->insert(x, hash, &value);
    return value;
  });
}

/*******************************************************************************
Integrand with configurable cost
*******************************************************************************/

/// @brief Dependent FMA chain (latency-bound, 4 cycles per iteration on Zen 3).
[[gnu::noinline]] inline double spin(double x, const uint64_t iterations) {
  for (uint64_t i = 0; i < iterations; i++) x = x * 0.9999999 + 1e-9;
  return x;
}

/// @brief Iterations of @ref spin per nanosecond on this machine (measured once).
inline double spinIterationsPerNs() {
  static const double perNs = []() {
    const uint64_t n = 20'000'000;
    double best = 1e300;
    for (int r = 0; r < 3; r++) {
      const auto start = std::chrono::steady_clock::now();
      doNotOptimize(spin(0.5, n));
      best = std::min(best, std::chrono::duration<double, std::nano>(
                                std::chrono::steady_clock::now() - start)
                                .count());
    }
    return static_cast<double>(n) / best;
  }();
  return perNs;
}

/// @brief @c base(x) + 1e-300 * spin(...): costs about @p costNs more, value bitwise unchanged.
struct CostedFunc {
  double (*baseFunc)(const base::DataVector&);
  uint64_t iterations;

  CostedFunc(double (*func)(const base::DataVector&), const double costNs)
      : baseFunc(func),
        iterations(static_cast<uint64_t>(std::llround(costNs * spinIterationsPerNs()))) {}

  double operator()(const base::DataVector& x) const {
    const double v = baseFunc(x);
    return iterations == 0 ? v : v + 1e-300 * spin(x[0], iterations);
  }
};

/*******************************************************************************
Disk tier: append-only flat file of records behind a header
*******************************************************************************/

/// @brief File header (validated on load): version, key scheme, d, m, function id, record count.
struct FlatFileHeader {
  char magic[8] = {'S', 'G', 'C', 'A', 'C', 'H', 'E', '1'};
  uint32_t version = 1;
  uint32_t keyScheme = 1;  // 1 = bitwise user-domain coordinates
  uint64_t nDim = 0;
  uint64_t nOutputs = 0;
  uint64_t nRecords = 0;  // records fully written (updated after the data, crash-safe tail)
  char functionId[64] = {};
};

/// @brief Minimal POSIX flat-file tier (the format the summary proposes).
class FlatFile {
 public:
  FlatFile(const std::string& path, const RecordLayout layout) : path_(path), layout_(layout) {}

  /// @brief Writes header + @p n records from @p records (stride words each), optionally synced.
  double write(const uint64_t* records, const size_t n, const bool sync) const {
    const auto start = std::chrono::steady_clock::now();
    const int fd = ::open(path_.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
    FlatFileHeader header;
    header.nDim = layout_.d;
    header.nOutputs = layout_.m;
    writeAll(fd, &header, sizeof(header));
    writeAll(fd, records, n * layout_.bytes());
    if (sync) ::fdatasync(fd);
    header.nRecords = n;
    if (::pwrite(fd, &header, sizeof(header), 0) != static_cast<ssize_t>(sizeof(header))) {
      std::abort();
    }
    if (sync) ::fdatasync(fd);
    ::close(fd);
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
  }

  /// @brief Reads all records into @p out (after header validation); returns the record count.
  size_t read(std::vector<uint64_t>& out) const {
    const int fd = ::open(path_.c_str(), O_RDONLY);
    FlatFileHeader header;
    if (::read(fd, &header, sizeof(header)) != static_cast<ssize_t>(sizeof(header)) ||
        header.nDim != layout_.d || header.nOutputs != layout_.m) {
      ::close(fd);
      return 0;
    }
    out.resize(header.nRecords * layout_.stride());
    readAll(fd, out.data(), out.size() * 8);
    ::close(fd);
    return header.nRecords;
  }

  /// @brief Reads record @p idx with one @c pread (spill-tier hit).
  void readRecord(const int fd, const size_t idx, uint64_t* out) const {
    const off_t offset = static_cast<off_t>(sizeof(FlatFileHeader) + idx * layout_.bytes());
    if (::pread(fd, out, layout_.bytes(), offset) != static_cast<ssize_t>(layout_.bytes())) {
      std::abort();
    }
  }

  const std::string& path() const { return path_; }

 private:
  static void writeAll(const int fd, const void* data, size_t bytes) {
    const char* p = static_cast<const char*>(data);
    while (bytes > 0) {
      const ssize_t w = ::write(fd, p, bytes);
      if (w <= 0) std::abort();
      p += w;
      bytes -= static_cast<size_t>(w);
    }
  }
  static void readAll(const int fd, void* data, size_t bytes) {
    char* p = static_cast<char*>(data);
    while (bytes > 0) {
      const ssize_t r = ::read(fd, p, bytes);
      if (r <= 0) std::abort();
      p += r;
      bytes -= static_cast<size_t>(r);
    }
  }

  std::string path_;
  RecordLayout layout_;
};

/// @brief Directory for disk-tier measurements (on the SSD, not tmpfs).
inline std::string diskDir() {
  const char* env = std::getenv("PERF_CACHE_DIR");
  return env ? env : "/var/tmp";
}

/*******************************************************************************
Test data: distinct sparse-grid points (structured keys, as the operators produce them)
*******************************************************************************/

/// @brief The first @p n distinct points (row-major, d doubles each) of a deltas sparse grid
/// (equidistant, boundary offset 1, unit domain) of the smallest level with >= n points.
inline std::vector<double> distinctGridPoints(const size_t nDim, const size_t n) {
  for (LvlType lvl = 0;; lvl++) {
    CompleteSGGenInstr genInstr(lvl, nDim);
    configure(genInstr, "equi", 1, unitDomain(nDim));
    const DeltasSparseGrid grid(genInstr);
    size_t total = 0;
    for (const auto& tgData : grid) total += tgData.tensorGrid.nGP();
    if (total < n) continue;

    std::vector<double> points;
    points.reserve(n * nDim);
    base::DataVector gp(nDim);
    for (const auto& tgData : grid) {
      for (size_t i = 0; i < tgData.tensorGrid.nGP() && points.size() < n * nDim; i++) {
        tgData.tensorGrid.getGridPoint(i, gp);
        points.insert(points.end(), gp.begin(), gp.end());
      }
    }
    return points;
  }
}

/// @brief Point @p i of a row-major point list.
inline base::DataVector pointAt(const std::vector<double>& flat, const size_t nDim,
                                const size_t i) {
  base::DataVector point(nDim);
  std::copy_n(flat.begin() + static_cast<std::ptrdiff_t>(i * nDim), nDim, point.begin());
  return point;
}

/// @brief Heap bytes in use (glibc: arenas + mmapped chunks).
inline size_t heapBytesInUse() {
  const struct mallinfo2 mi = mallinfo2();
  return mi.uordblks + mi.hblkhd;
}

/*******************************************************************************
Micro cases: hashing, lookup, insert
*******************************************************************************/

inline std::string dn(const size_t nDim, const size_t n) {
  return "d" + std::to_string(nDim) + "/n" + std::to_string(n);
}

inline void addHashCases(Suite& suite, const size_t nDim) {
  const size_t nKeys = 4096;
  auto keys = std::make_shared<std::vector<base::DataVector>>();
  auto addCase = [&](const std::string& name, auto hashFunc) {
    suite.addMicro("micro/cache/hash/" + name + "/d" + std::to_string(nDim),
                   [=](MicroRun& run) {
                     if (keys->empty()) {
                       const std::vector<double> flat = distinctGridPoints(nDim, nKeys);
                       for (size_t i = 0; i < nKeys; i++) {
                         keys->push_back(pointAt(flat, nDim, i));
                       }
                     }
                     run.setItems(static_cast<double>(nKeys));
                     run.measure([&]() {
                       uint64_t acc = 0;
                       for (const base::DataVector& k : *keys) acc += hashFunc(k);
                       doNotOptimize(acc);
                     });
                   });
  };
  addCase("wy", [](const base::DataVector& k) { return hashPoint(k.data(), k.size()); });
  addCase("fnv_proto", [](const base::DataVector& k) { return hashPointFnv(k); });
#ifdef PERF_CACHE_HAVE_XXH3
  addCase("xxh3", [](const base::DataVector& k) { return hashPointXxh3(k.data(), k.size()); });
#endif
}

/// @brief Shared input of the lookup / insert cases of one (d, n): keys and their values.
struct KeySet {
  size_t nDim = 0;
  std::vector<base::DataVector> keys;
  std::vector<uint32_t> queryOrder;  // random indices (lookup cases)
  std::vector<double> queries;       // the queried keys, contiguous in query order

  /// @brief The key set of (d, n); only the most recent one is kept alive between cases.
  static std::shared_ptr<KeySet> get(const size_t nDim, const size_t n) {
    static std::shared_ptr<KeySet> current;
    if (!current || current->nDim != nDim || current->keys.size() != n) {
      current.reset();
      current = make(nDim, n);
    }
    return current;
  }

  static std::shared_ptr<KeySet> make(const size_t nDim, const size_t n) {
    auto ks = std::make_shared<KeySet>();
    ks->nDim = nDim;
    const std::vector<double> flat = distinctGridPoints(nDim, n);
    ks->keys.reserve(n);
    for (size_t i = 0; i < n; i++) ks->keys.push_back(pointAt(flat, nDim, i));
    const size_t nQueries = std::min<size_t>(n, 1 << 16);
    Rng rng(42 + n);
    ks->queryOrder.resize(nQueries);
    for (uint32_t& q : ks->queryOrder) q = static_cast<uint32_t>(rng.index(n));
    ks->queries.reserve(nQueries * nDim);
    for (const uint32_t q : ks->queryOrder) {
      ks->queries.insert(ks->queries.end(), ks->keys[q].begin(), ks->keys[q].end());
    }
    return ks;
  }
};

template <typename Table>
void fillTable(Table& table, const KeySet& ks) {
#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < ks.keys.size(); i++) {
    const double value = static_cast<double>(i);
    table.insert(ks.keys[i], keyHash<Table>(ks.keys[i]), &value);
  }
}

/// @brief Capacity hint for a table built without knowing its size (0: growth measured); the
/// fixed-capacity reference must be sized.
template <typename Table>
size_t unknownSizeHint(const size_t n) {
  if constexpr (requires { Table::FIXED_CAPACITY; }) {
    return n;
  } else {
    return 0;
  }
}

constexpr size_t NO_LIMIT = size_t{1} << 62;

template <typename Table>
void addTableCases(Suite& suite, const std::string& tableName, const size_t nDim,
                   const size_t n) {
  const RecordLayout layout{nDim, 1};

  suite.addMicro("micro/cache/lookup/" + tableName + "/" + dn(nDim, n), [=](MicroRun& run) {
    const std::shared_ptr<KeySet> ks = KeySet::get(nDim, n);
    const double before = static_cast<double>(heapBytesInUse());
    Table table(layout, unknownSizeHint<Table>(n), NO_LIMIT);  // grown as without a hint
    fillTable(table, *ks);
    run.setCounter("bytes_per_entry", (static_cast<double>(heapBytesInUse()) - before) /
                                          static_cast<double>(n));
    run.setCounter("entries", static_cast<double>(table.size()));
    run.setCounter("threads", omp_get_max_threads());
    if constexpr (requires { table.shardImbalance(); }) {
      run.setCounter("shard_imbalance", table.shardImbalance());
    }
    // The queried point is hot as in the operators (their scratch point): the keys are read
    // sequentially from a contiguous copy into a per-thread DataVector.
    const size_t nQueries = ks->queryOrder.size();
    const double* const queries = ks->queries.data();
    run.setItems(static_cast<double>(nQueries));
    size_t found = 0;
    run.measure([&]() {
      double sum = 0;
      size_t hits = 0;
      base::DataVector key(nDim);
#pragma omp parallel for schedule(static) reduction(+ : sum, hits) firstprivate(key)
      for (size_t q = 0; q < nQueries; q++) {
        std::copy_n(queries + q * nDim, nDim, key.begin());
        double value;
        if (table.find(key, keyHash<Table>(key), &value)) {
          sum += value;
          hits++;
        }
      }
      found = hits;
      doNotOptimize(sum);
    });
    run.setCounter("hit_fraction", static_cast<double>(found) / static_cast<double>(nQueries));
  });

  suite.addMicro("micro/cache/insert/" + tableName + "/" + dn(nDim, n), [=](MicroRun& run) {
    const std::shared_ptr<KeySet> ks = KeySet::get(nDim, n);
    run.setItems(static_cast<double>(n));
    run.setCounter("threads", omp_get_max_threads());
    run.measure([&]() {
      Table table(layout, unknownSizeHint<Table>(n), NO_LIMIT);  // growth included
      fillTable(table, *ks);
      doNotOptimize(table.size());
    });
  });
}

/// @brief All tables for one (d, n), so that consecutive cases share the key set.
inline void addTableCaseSet(Suite& suite, const size_t nDim, const size_t n) {
  addTableCases<OldProtoTable>(suite, "oldproto", nDim, n);
  addTableCases<ShardedTable<std::mutex>>(suite, "sharded_mutex", nDim, n);
  addTableCases<ShardedTable<SpinLock>>(suite, "sharded_spin", nDim, n);
  addTableCases<BoostConcurrentTable>(suite, "boost_cfs", nDim, n);
#ifdef PERF_CACHE_HAVE_PHMAP
  addTableCases<PhmapTable>(suite, "phmap", nDim, n);
#endif
  addTableCases<SolTable>(suite, "sol", nDim, n);
}

/*******************************************************************************
Micro cases: whole operators with a cached source function
*******************************************************************************/

/// @brief An operator configuration (as the e2e cases, smaller).
struct OpConfig {
  std::string op;  // quad (SG), dsgquad, interp, interplin
  std::string nodes;
  double (*integrand)(const base::DataVector&);
  LvlType boundary;
  size_t nDim;
  LvlType maxLvl;
  size_t nPoints;  // interpolation points (interp / interplin)

  std::string name() const {
    const std::string grid = (op == "dsgquad") ? "quad/dsg/" : (op + "/sg/");
    return grid + nodes + "/b" + std::to_string(boundary) + "/d" + std::to_string(nDim) + "/L" +
           std::to_string(maxLvl) + (nPoints > 0 ? "/p" + std::to_string(nPoints) : "");
  }
  bool shifted() const { return op == "interp" || op == "interplin"; }
};

/// @brief Grid, points and the operator of one configuration.
struct OpInstance {
  OpConfig cfg;
  std::optional<CompleteSGGenInstr> genInstr;
  std::optional<SparseGrid> sg;
  std::optional<DeltasSparseGrid> dsg;
  std::vector<base::DataVector> points;

  explicit OpInstance(const OpConfig& c) : cfg(c) {
    genInstr.emplace(c.maxLvl, c.nDim);
    const HyperCubeArea domain = c.shifted() ? shiftedDomain(c.nDim) : unitDomain(c.nDim);
    configure(*genInstr, c.nodes, c.boundary, domain);
    if (c.op == "dsgquad") {
      dsg.emplace(*genInstr);
    } else {
      sg.emplace(*genInstr);
    }
    if (c.nPoints > 0) points = randomPoints(domain, c.nPoints, 1000 + c.nDim);
  }

  double run(const SourceFunc& f) const {
    if (cfg.op == "quad") return quadrature(f, *sg);
    if (cfg.op == "dsgquad") return quadrature(f, *dsg);
    double sum = 0;
    for (const base::DataVector& p : points) {
      sum += (cfg.op == "interp") ? interpolate(f, p, *sg) : interpolateLinear(f, p, *sg);
    }
    return sum;
  }

  /// @brief Quadrature on the same grid (the cross-operator warm-up of interp / interplin).
  double runQuadrature(const SourceFunc& f) const {
    return dsg ? quadrature(f, *dsg) : quadrature(f, *sg);
  }
};

/**
 * @brief Variants: @c none (plain SourceFunc), @c always (fresh cache per operator call),
 * @c warm (cache filled before: by the same operator for quadrature = second run / persistent
 * load, by a quadrature on the same grid for interp / interplin = cross-operator reuse),
 * @c bypass (wrapper in bypass mode: adaptive policy that decided not to cache).
 */
template <typename Table>
void addOpCase(Suite& suite, const OpConfig& cfg, const std::string& variant,
               const std::string& tableName, const double costNs) {
  const std::string name = "micro/cache/op/" + cfg.name() + "/" + variant +
                           (variant == "none" ? "" : "_" + tableName) + "/c" +
                           std::to_string(static_cast<int64_t>(costNs));
  suite.addMicro(name, [=](MicroRun& run) {
    const OpInstance inst(cfg);
    const CostedFunc costed(cfg.integrand, costNs);
    const SourceFunc plain(costed);
    const RecordLayout layout{cfg.nDim, 1};

    // Counts of one call: evaluations and distinct points (cache without limit, 1 table)
    auto countStats = std::make_shared<CacheStats>();
    auto countTable = std::make_shared<ShardedTable<SpinLock>>(layout, 0, NO_LIMIT);
    const double cachedResult = inst.run(makeCachedSourceFunc(costed, countTable, countStats));
    const double plainResult = inst.run(plain);
    run.setCounter("evals_per_call", static_cast<double>(countStats->lookups()));
    run.setCounter("distinct", static_cast<double>(countTable->size()));
    run.setCounter("bitwise_equal_or_1thread_only",
                   (omp_get_max_threads() > 1) ? -1.0 : (cachedResult == plainResult ? 1.0 : 0.0));
    run.setCounter("threads", omp_get_max_threads());
    run.setItems(static_cast<double>(countStats->lookups()));

    std::shared_ptr<Table> warmTable;
    if (variant == "warm") {
      warmTable = std::make_shared<Table>(layout, countTable->size(), NO_LIMIT);
      auto st = std::make_shared<CacheStats>();
      if (cfg.op == "interp" || cfg.op == "interplin") {
        inst.runQuadrature(makeCachedSourceFunc(costed, warmTable, st));
      } else {
        inst.run(makeCachedSourceFunc(costed, warmTable, st));
      }
    }

    auto stats = std::make_shared<CacheStats>();
    auto bypass = std::make_shared<std::atomic<bool>>(true);
    run.measure([&]() {
      if (variant == "none") {
        doNotOptimize(inst.run(plain));
      } else if (variant == "bypass") {
        auto table = std::make_shared<Table>(layout, 0, NO_LIMIT);
        doNotOptimize(inst.run(makeCachedSourceFunc(costed, table, stats, bypass)));
      } else if (variant == "always") {
        auto table = std::make_shared<Table>(layout, 0, NO_LIMIT);
        doNotOptimize(inst.run(makeCachedSourceFunc(costed, table, stats)));
      } else {
        doNotOptimize(inst.run(makeCachedSourceFunc(costed, warmTable, stats)));
      }
    });
    if (stats->lookups() > 0) {
      run.setCounter("hit_rate",
                     static_cast<double>(stats->hits()) / static_cast<double>(stats->lookups()));
    }
  });
}

inline std::vector<OpConfig> opConfigs() {
  // ~1e5 evaluations per call (a 1 us integrand then takes ~0.1 s per call at 1 thread)
  return {
      {"quad", "cc", uqIntegrand, 0, 5, 4, 0},     {"quad", "equi", expSum, 1, 20, 3, 0},
      {"quad", "equi", expSum, 0, 2, 11, 0},       {"quad", "equi", expSum, 1, 10, 4, 0},
      {"dsgquad", "cc", uqIntegrand, 0, 5, 5, 0},  {"interp", "cc", cosProd, 0, 5, 3, 8},
      {"interplin", "equi", cosProd, 1, 8, 3, 8},
  };
}

template <typename Table>
void addOpCases(Suite& suite, const std::string& tableName, const bool withNone) {
  for (const OpConfig& cfg : opConfigs()) {
    for (const double cost : {0.0, 100.0, 1000.0}) {
      if (withNone) addOpCase<Table>(suite, cfg, "none", tableName, cost);
      addOpCase<Table>(suite, cfg, "always", tableName, cost);
      addOpCase<Table>(suite, cfg, "warm", tableName, cost);
    }
    if (withNone) addOpCase<Table>(suite, cfg, "bypass", tableName, 0.0);
  }
}

/*******************************************************************************
Micro cases: disk tier (flat file)
*******************************************************************************/

inline void addDiskCases(Suite& suite, const size_t nDim, const size_t n) {
  const RecordLayout layout{nDim, 1};
  auto records = std::make_shared<std::vector<uint64_t>>();
  auto getRecords = [=]() {
    if (records->empty()) {
      const std::vector<double> flat = distinctGridPoints(nDim, n);
      records->resize(n * layout.stride());
      for (size_t i = 0; i < n; i++) {
        const double value = static_cast<double>(i);
        writeRecord(records->data() + i * layout.stride(), layout, flat.data() + i * nDim,
                    hashPoint(flat.data() + i * nDim, nDim), &value);
      }
    }
    return records;
  };
  const std::string path = diskDir() + "/sgpp_perf_cache_" + std::to_string(nDim) + "_" +
                           std::to_string(n) + ".bin";

  for (const bool sync : {false, true}) {
    suite.addMicro(std::string("micro/cache/disk/flat_write") + (sync ? "_sync/" : "/") +
                       dn(nDim, n),
                   [=](MicroRun& run) {
                     const auto recs = getRecords();
                     const FlatFile file(path, layout);
                     run.setItems(static_cast<double>(n));
                     run.setCounter("bytes_per_record", static_cast<double>(layout.bytes()));
                     run.measure([&]() { doNotOptimize(file.write(recs->data(), n, sync)); });
                   });
  }

  suite.addMicro("micro/cache/disk/flat_load/" + dn(nDim, n), [=](MicroRun& run) {
    const auto recs = getRecords();
    const FlatFile file(path, layout);
    file.write(recs->data(), n, false);
    run.setItems(static_cast<double>(n));
    std::vector<uint64_t> buffer;
    run.measure([&]() {
      // Load = read the file + insert every record into the in-memory table (as on creation)
      const size_t nRead = file.read(buffer);
      ShardedTable<SpinLock> table(layout, nRead, NO_LIMIT);
      base::DataVector x(nDim);
#pragma omp parallel for schedule(static) firstprivate(x)
      for (size_t i = 0; i < nRead; i++) {
        const uint64_t* r = buffer.data() + i * layout.stride();
        double value;
        std::memcpy(x.data(), r + 1, 8 * nDim);
        std::memcpy(&value, r + 1 + nDim, 8);
        table.insert(x, r[0], &value);
      }
      doNotOptimize(table.size());
    });
  });

  suite.addMicro("micro/cache/disk/pread_hit/" + dn(nDim, n), [=](MicroRun& run) {
    const auto recs = getRecords();
    const FlatFile file(path, layout);
    file.write(recs->data(), n, false);
    const int fd = ::open(path.c_str(), O_RDONLY);
    Rng rng(7);
    std::vector<size_t> order(4096);
    for (size_t& o : order) o = rng.index(n);
    run.setItems(static_cast<double>(order.size()));
    std::vector<uint64_t> rec(layout.stride());
    run.measure([&]() {
      uint64_t acc = 0;
      for (const size_t o : order) {
        file.readRecord(fd, o, rec.data());
        acc += rec[0];
      }
      doNotOptimize(acc);
    });
    ::close(fd);
  });
}

/*******************************************************************************
Registration
*******************************************************************************/

inline void addCacheProtoCases(Suite& suite) {
  for (const size_t nDim : {2, 5, 10, 20}) addHashCases(suite, nDim);

  for (const size_t nDim : {2, 10, 20}) {
    for (const size_t n : {10'000, 1'000'000}) addTableCaseSet(suite, nDim, n);
  }

  addOpCases<ShardedTable<SpinLock>>(suite, "sharded_spin", true);
  addOpCases<BoostConcurrentTable>(suite, "boost_cfs", false);
  addOpCases<OldProtoTable>(suite, "oldproto", false);
  addOpCases<PerThreadTable>(suite, "perthread", false);

  for (const size_t nDim : {2, 10, 20}) addDiskCases(suite, nDim, 1'000'000);
}

}  // namespace cache_proto
}  // namespace perf
}  // namespace combigrid
}  // namespace sgpp
