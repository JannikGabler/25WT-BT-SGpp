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
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_record_arena.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table_shard.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

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
   * @throws base::dependency_exception if SG++ was built without the libraries.
   */
  BoostPointValueTable(boost_point_value_table::RecordLayout layout, size_t memoryLimit,
                       size_t nShards);

  ~BoostPointValueTable();

  BoostPointValueTable(const BoostPointValueTable&) = delete;
  BoostPointValueTable& operator=(const BoostPointValueTable&) = delete;

  /// @brief Returns the number of coordinates of the points.
  size_t nDim() const { return layout_.nDim; }

  /**
   * @brief Hash of the bit patterns of a point (XXH3; -0.0 and 0.0 differ).
   * @param point Coordinates.
   * @param nDim  Number of coordinates.
   * @return Hash.
   */
  static uint64_t hash(const double* point, size_t nDim);

  /**
   * @brief Looks a point up.
   * @param point  Coordinates (@ref nDim).
   * @param hash   @ref hash of the point.
   * @param values Output: the values of the point on a hit (@c nOutputs).
   * @return Whether the point is stored.
   */
  bool find(const double* point, uint64_t hash, double* values);

  /**
   * @brief Stores the values of a point, unless it is stored already (by a concurrent insert),
   * the shard reached its share of the memory limit, or its arena is full.
   * @param point  Coordinates (@ref nDim).
   * @param hash   @ref hash of the point.
   * @param values Values of the point (@c nOutputs).
   */
  void insert(const double* point, uint64_t hash, const double* values);

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

    evaluate(point, values);
    if (std::ranges::none_of(values, [](const double value) { return std::isnan(value); })) {
      insert(point.data(), pointHash, values.data());
    }
  }

  /**
   * @brief Returns the statistics of all shards (all fields except @c operatorCalls).
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

  boost_point_value_table::RecordLayout layout_;  ///< Layout of the records.
  size_t shardMemoryLimit_;                       ///< Share of the memory limit per shard.
  std::vector<std::unique_ptr<boost_point_value_table::Shard>> shards_;  ///< The shards.

  /// @}

  /// @name Helper
  /// @{

  /// @brief Returns the shard of a hash.
  boost_point_value_table::Shard& shard(uint64_t hash) const;

  /// @}
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
