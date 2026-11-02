// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file boost_point_value_table.hpp
 * @brief Concurrent hash table of the source-function cache (points -> values).
 */
#pragma once

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

// Declared only: the shard holds the Boost index and depends on the configuration
// (boost_point_value_table_shard.hpp, included by the implementation).
namespace boost_point_value_table {
struct Shard;
}  // namespace boost_point_value_table

class LMDBPointValueStore;

/**
 * @brief Concurrent table of the records of a @c SourceFuncCache: the values of points, keyed by
 * the bit patterns of the point (XXH3 hash).
 *
 * @c SourceFuncCacheConfig::nShards shards (a power of two <= 64, default
 * @c constants::source_func_cache::DEFAULT_SHARDS_CNT), selected by bits 24..29 of the hash (the
 * index uses the high bits for the position and the low bits for its tags), each with a spin
 * lock, an own @c RecordArena (no allocation counter shared by all threads) and a
 * @c boost::unordered_flat_set of 4-byte record indices, searched by point and hash. The memory
 * limit is split evenly among the shards. WP-25: lookups 78 / 253 ns (1 thread, 1e4 / 1e6 points,
 * d = 10), 12 / 21 ns wall at 16 threads; inserts 59 ns wall at 16 threads, where designs with one
 * arena shared by all threads stall (150-240 ns).
 *
 * With a disk store (storage policies of @c SourceFuncCache), the table is the memory tier in
 * front of it: values beyond the memory limit are collected per shard (found there) and written
 * to the store in batches of @c constants::source_func_cache::SPILL_BATCH_RECORDS; misses look
 * the store up only once it holds records that memory does not (@ref markOnDisk). With
 * @c storeAllRecords (persistent policy), @ref collectUnflushed also hands out the records of
 * memory that are not in the store yet.
 *
 * Needs Boost.Unordered and xxHash (@c SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE, see
 * dependencies_defines.hpp); without them the class compiles, but its constructor throws.
 */
class BoostPointValueTable {
 public:
  /**
   * @brief Creates an empty table.
   * @param layout      Layout of the records (number of coordinates and of values).
   * @param memoryLimit Soft limit of the bytes of all shards (records and index).
   * @param nShards     Number of shards (@ref checkShardsCnt).
   * @param store       Disk store behind the table (nullptr: memory only; must outlive the
   *                    table and have at least @p nShards read slots).
   * @param storeAllRecords Whether the store receives all records (persistent) or only those
   *                    beyond the memory limit (temporary).
   * @throws base::dependency_exception if SG++ was built without the libraries.
   */
  BoostPointValueTable(source_func_caching::RecordLayout layout, size_t memoryLimit, size_t nShards,
                       LMDBPointValueStore* store = nullptr, bool storeAllRecords = false);

  ~BoostPointValueTable();

  BoostPointValueTable(const BoostPointValueTable&) = delete;
  BoostPointValueTable& operator=(const BoostPointValueTable&) = delete;

  /// @brief Returns the number of coordinates of the points.
  size_t nDim() const { return layout_.nDim; }

  /// @brief Returns the layout of the records.
  source_func_caching::RecordLayout layout() const { return layout_; }

  /**
   * @brief Hash of the bit patterns of a point (XXH3; -0.0 and 0.0 differ).
   * @param point Coordinates.
   * @param nDim  Number of coordinates.
   * @return Hash.
   */
  static uint64_t hash(const double* point, size_t nDim);

  /**
   * @brief Looks a point up in memory.
   * @param point  Coordinates (@ref nDim).
   * @param hash   @ref hash of the point.
   * @param values Output: the values of the point on a hit (@c nOutputs).
   * @return Whether the point is stored.
   */
  bool find(const double* point, uint64_t hash, double* values);

  /**
   * @brief Stores the values of a point, unless it is stored already (by a concurrent insert),
   * the shard reached its share of the memory limit, or its arena is full; in the last two
   * cases the values go to the disk store, if there is one, @p allowSpill is set and the store
   * is not full.
   * @param point  Coordinates (@ref nDim).
   * @param hash   @ref hash of the point.
   * @param values Values of the point (@c nOutputs).
   * @param allowSpill Whether values beyond the memory limit may go to the disk store (the
   *               adaptive policy passes false in mode MemoryOnly).
   * @return False if the values were rejected (memory full and not spilled), else true.
   */
  bool insert(const double* point, uint64_t hash, const double* values, bool allowSpill = true);

  /**
   * @brief Looks @p point up and, on a miss, evaluates it and stores the values unless one of
   * them is NaN (a failed evaluation is retried next time).
   * @param point    Point (@ref nDim coordinates).
   * @param values   Output: the values of the point (@c nOutputs).
   * @param evaluate Callable @c evaluate(point, values) writing the values of a miss.
   */
  template <typename Evaluate>
  void lookUpOrEvaluate(const base::DataVector& point, const std::span<double> values,
                        const Evaluate& evaluate) {
    const uint64_t pointHash = hash(point.data(), point.size());
    if (find(point.data(), pointHash, values.data())) {
      return;
    }
    if (onDisk_.load(std::memory_order_relaxed)) {
      [[unlikely]] if (findOnDisk(point.data(), pointHash, values.data())) { return; }
    }

    evaluate(point, values);
    if (std::ranges::none_of(values, [](const double value) { return std::isnan(value); })) {
      insert(point.data(), pointHash, values.data());
    }
  }

  /**
   * @brief Looks a point up in the disk store (incl. the records collected for it in memory).
   * @param point  Coordinates (@ref nDim).
   * @param hash   @ref hash of the point.
   * @param values Output: the values of the point on a hit (@c nOutputs).
   * @return Whether the store holds the point.
   */
  bool findOnDisk(const double* point, uint64_t hash, double* values);

  /**
   * @brief Stores a record loaded from the disk store in memory (no disk write).
   * @param record Record @c [hash | point | values].
   * @return Whether it was stored (false: the shard's share of the memory limit is used up).
   */
  bool insertLoaded(const uint64_t* record);

  /// @brief Marks all records in memory as present in the disk store.
  void markFlushed();

  /// @brief Marks that the disk store holds records that memory does not: misses look it up.
  void markOnDisk() { onDisk_.store(true, std::memory_order_relaxed); }

  /// @brief Whether the disk store holds records that memory does not.
  bool onDisk() const { return onDisk_.load(std::memory_order_relaxed); }

  /**
   * @brief Attaches a disk store to a table created without one (the adaptive policy creates its
   * temporary file at the first value beyond the memory limit; once, concurrently with lookups).
   * @param store The store (must outlive the table and have at least @c nShards read slots).
   */
  void setStore(LMDBPointValueStore* store) { store_.store(store, std::memory_order_release); }

  /**
   * @brief Hands out the records that are not in the disk store yet: the collected records
   * beyond the memory limit (copied to @p pendingRecords, removed from the shards) and, with
   * @c storeAllRecords, the records of memory since the last call (pointers into memory,
   * stable while the table lives).
   * @param records        Output: appended pointers to the records (also those copied).
   * @param pendingRecords Output: storage of the copied records (must outlive @p records).
   */
  void collectUnflushed(std::vector<const uint64_t*>& records,
                        std::vector<uint64_t>& pendingRecords);

  /**
   * @brief Returns the statistics of all shards (all fields except @c operatorCalls and the
   * fields of the disk store).
   * @return Statistics.
   */
  CacheStatistics statistics() const;

  /**
   * @brief Checks a number of shards: a power of two, at least 1 and at most 64 (the shard is
   * selected by up to 6 bits of the hash).
   * @param nShards Number of shards.
   * @throws std::invalid_argument otherwise.
   */
  static void checkShardsCnt(size_t nShards);

 private:
  /// @name Attributes
  /// @{

  const size_t nShards_;  ///< Number of shards.

  source_func_caching::RecordLayout layout_;  ///< Layout of the records.
  size_t shardMemoryLimit_;                   ///< Share of the memory limit per shard.
  std::vector<std::unique_ptr<boost_point_value_table::Shard>> shards_;  ///< The shards.
  /// Disk store (nullptr: memory only; set once later by @ref setStore).
  std::atomic<LMDBPointValueStore*> store_;
  bool storeAllRecords_;             ///< Whether the store receives the records of memory as well.
  std::atomic<bool> onDisk_{false};  ///< Whether the store holds records that memory does not.

  /// @}

  /// @name Helper
  /// @{

  /// @brief Returns the index of the shard of a hash.
  size_t shardIndex(uint64_t hash) const { return (hash >> 24) & (nShards_ - 1); }

  /// @brief Returns the shard of a hash.
  boost_point_value_table::Shard& shard(uint64_t hash) const;

  /**
   * @brief Stores the values of a point in the disk store (memory is full; shard not locked).
   * @return False if the store is full (rejected).
   */
  bool spill(boost_point_value_table::Shard& s, const double* point, uint64_t hash,
             const double* values);

  /**
   * @brief Appends a record to the shard's buffer for the disk store (shard locked).
   * @return Whether the buffer is full (the caller writes it).
   */
  bool appendPending(boost_point_value_table::Shard& s, const double* point, uint64_t hash,
                     const double* values);

  /// @brief Looks a point up in the shard's buffer for the disk store (shard locked).
  bool findPending(const boost_point_value_table::Shard& s, const double* point, uint64_t hash,
                   double* values) const;

  /// @brief Writes a full buffer of records to the disk store.
  void writePending(const std::vector<uint64_t>& records);

  /// @}
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
