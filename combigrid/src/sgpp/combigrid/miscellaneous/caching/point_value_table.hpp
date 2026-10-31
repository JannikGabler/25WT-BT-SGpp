// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file point_value_table.hpp
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
#include <sgpp/combigrid/miscellaneous/caching/record_arena.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Concurrent table of the records of a @c SourceFuncCache: the values of points, keyed by
 * the bit patterns of the point (XXH3 hash).
 *
 * @c constants::source_func_cache::N_SHARDS shards, selected by bits 24..29 of the hash (the
 * index uses the high bits for the position and the low bits for its tags), each with a spin
 * lock, an own @c RecordArena (no allocation counter shared by all threads) and a
 * @c boost::unordered_flat_set of 4-byte record indices, searched by point and hash. The memory
 * limit is split evenly among the shards. WP-25: lookups 78 / 253 ns (1 thread, 1e4 / 1e6 points,
 * d = 10), 12 / 21 ns wall at 16 threads; inserts 59 ns wall at 16 threads, where designs with one
 * arena shared by all threads stall (150-240 ns).
 *
 * The shards (with the Boost index) are defined in the source file, so that this header does not
 * depend on Boost and xxHash.
 */
class PointValueTable {
 public:
  /**
   * @brief Creates an empty table.
   * @param layout      Layout of the records (number of coordinates and of values).
   * @param memoryLimit Soft limit of the bytes of all shards (records and index).
   */
  PointValueTable(RecordLayout layout, size_t memoryLimit);

  ~PointValueTable();

  PointValueTable(const PointValueTable&) = delete;
  PointValueTable& operator=(const PointValueTable&) = delete;

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

 private:
  struct Shard;

  /// @brief Returns the shard of a hash.
  Shard& shard(uint64_t hash) const;

  RecordLayout layout_;                         ///< Layout of the records.
  size_t shardMemoryLimit_;                     ///< Share of the memory limit per shard.
  std::vector<std::unique_ptr<Shard>> shards_;  ///< The shards.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
