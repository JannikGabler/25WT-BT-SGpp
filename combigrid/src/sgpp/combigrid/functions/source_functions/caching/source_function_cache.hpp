// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file source_function_cache.hpp
 * @brief In-memory cache of the values of a source function (@c CachedSourceFunc,
 * @c CachedVectorSourceFunc).
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/functions/source_functions/caching/cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function_hook.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {
class PointValueTable;
}  // namespace misc

/**
 * @brief In-memory cache of the values of one source function, keyed by the bit patterns of the
 * points in problem coordinates (the points the function receives).
 *
 * A @c CachedSourceFunc or @c CachedVectorSourceFunc looks every point up in its cache first and
 * calls the wrapped function on misses only. The operators are unchanged, and a plain
 * @c SourceFunc does not pay anything. Because all operators map the grid points onto the
 * domain in the same way (@c tools::mapToDomain), stored values are found across grids,
 * operators (quadrature and interpolation), refinement steps and calls, as long as the same
 * cache is used: share it by passing the same cached function, or several cached functions
 * built on this object.
 *
 * - One cache serves one function. Its number of outputs is fixed by the first cached function
 *   built on it (another number throws), its number of coordinates by the first lookup (points
 *   of another dimension are evaluated without the cache).
 * - Thread-safe: one table of 64 shards, each with a spin lock, a record arena and a hash index
 *   (Boost.Unordered, XXH3). Two threads that miss the same point at the same time both call
 *   the function; one value is kept.
 * - Values with a NaN are not stored (a failed evaluation, e.g. a Python exception, is retried).
 * - Memory per point: 8 (d + m + 1) bytes plus about 5-10 bytes of index; soft limit
 *   @c CacheConfig::memoryLimit.
 * - Worth it for functions that cost more than a few hundred nanoseconds (WP-25: break-even of
 *   the SG quadrature 120-310 ns at 1 thread, 270-630 ns at 16 threads); cheap functions are
 *   faster without a cache.
 *
 * @note Needs Boost >= 1.81 (Boost.Unordered) and xxHash >= 0.8 when SG++ is built (detected in
 * optional_libraries.hpp). The class is always compiled; without the libraries its constructor
 * and those of the cached functions throw (@ref isAvailable).
 */
class SourceFuncCache final : public SourceFuncHook {
 public:
  /**
   * @brief Creates an empty cache.
   * @param config Configuration.
   * @throws base::not_implemented_exception if SG++ was built without the libraries of the
   * cache (@ref isAvailable).
   */
  explicit SourceFuncCache(const CacheConfig& config = CacheConfig());

  ~SourceFuncCache() override;

  /**
   * @brief Whether SG++ was built with the libraries of the cache (Boost >= 1.81, xxHash >= 0.8);
   * otherwise creating a cache or a cached function throws.
   * @return Whether the cache is available.
   */
  static bool isAvailable();

  /**
   * @brief Throws if the cache is not available (@ref isAvailable); called by the constructors of
   * the cache and of the cached functions.
   * @throws base::not_implemented_exception if SG++ was built without the libraries.
   */
  static void requireAvailable();

  SourceFuncCache(const SourceFuncCache&) = delete;
  SourceFuncCache& operator=(const SourceFuncCache&) = delete;

  /**
   * @brief Returns the configuration.
   * @return Configuration.
   */
  const CacheConfig& config() const;

  /**
   * @brief Returns the number of outputs of the cached function (0 before a cached function is
   * built on the cache).
   * @return Number of outputs m.
   */
  size_t nOutputs() const;

  /**
   * @brief Returns the counters since the creation or the last @ref clear (consistent per shard
   * if called during an operator call).
   * @return Statistics.
   */
  CacheStatistics statistics() const;

  /**
   * @brief Removes all stored values and resets the statistics (the number of outputs stays).
   * @warning Must not be called while a function of the cache is evaluated.
   */
  void clear();

  /**
   * @brief End-of-operator hook (@c SourceFuncHook): counts the operator call.
   */
  void finishOperatorCall() override;

 private:
  friend class CachedSourceFunc;
  friend class CachedVectorSourceFunc;

  /**
   * @brief Fixes the number of outputs of the cached function (called by the cached functions
   * built on this cache).
   * @param nOutputs Number of outputs m (at least 1).
   * @throws base::data_exception if the cache holds the values of a function with another m.
   */
  void attach(size_t nOutputs);

  /**
   * @brief Returns the table of the cache, created at the first call with the number of
   * coordinates of the first point.
   * @param nDim Number of coordinates of the point to look up.
   * @return The table, or nullptr if it holds points of another dimension.
   */
  misc::PointValueTable* table(size_t nDim);

  /**
   * @brief Creates the table for points with @p nDim coordinates unless another thread did.
   * @param nDim Number of coordinates.
   * @return The table.
   */
  misc::PointValueTable* createTable(size_t nDim);

  CacheConfig config_;   ///< Configuration.
  size_t nOutputs_ = 0;  ///< Number of outputs (guarded by @c mutex_; 0: not fixed yet).
  /// Table (nullptr until the first lookup); owned by @c ownedTable_.
  std::atomic<misc::PointValueTable*> table_{nullptr};
  /// Owner of the table (guarded; a shared_ptr, so that the destructor does not need the
  /// definition of the table, which exists only with the libraries).
  std::shared_ptr<misc::PointValueTable> ownedTable_;
  std::atomic<size_t> operatorCalls_{0};               ///< Finished operator calls.
  mutable std::mutex mutex_;  ///< Guards the creation of the table and the number of outputs.
};

}  // namespace combigrid
}  // namespace sgpp
