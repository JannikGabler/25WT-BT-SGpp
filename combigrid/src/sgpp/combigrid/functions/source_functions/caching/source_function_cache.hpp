// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file source_function_cache.hpp
 * @brief Cache of the values of a source function (@c CachedSourceFunc,
 * @c CachedVectorSourceFunc) in memory and optionally a temporary or persistent file.
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache_config.hpp>
#include <span>
#include <string>

namespace sgpp {
namespace combigrid {

// Declared only: their headers (internal) include the optional libraries and depend on the
// configuration, a public header must not (one class layout in every translation unit).
namespace misc {
class AdaptiveCacheController;
class BoostPointValueTable;
class LMDBPointValueStore;
namespace adaptive_caching {
enum class CostSampleKind : uint8_t;
struct AdaptiveCounters;
}  // namespace adaptive_caching
}  // namespace misc

/**
 * @brief Cache of the values of one source function, keyed by the bit patterns of the points in
 * problem coordinates (the points the function receives): in memory and, with the storage
 * policies (@c SourceFuncCacheConfig::policy), in a temporary or persistent file.
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
 *   of another dimension are evaluated without the cache; with the storage policies, the
 *   operator call then throws at its end) - or both by an existing persistent file.
 * - Thread-safe: one table of @c SourceFuncCacheConfig::nShards shards (default 64), each with a
 *   spin lock, a record arena and a hash index (Boost.Unordered, XXH3). Two threads that miss the
 * same point at the same time both call the function; one value is kept.
 * - Values with a NaN are not stored (a failed evaluation, e.g. a Python exception, is retried).
 * - Memory per point: 8 (d + m + 1) bytes plus about 5-10 bytes of index; soft limit
 *   @c SourceFuncCacheConfig::memoryLimit.
 * - Worth it for functions that cost more than a few hundred nanoseconds (WP-25: break-even of
 *   the SG quadrature 120-310 ns at 1 thread, 270-630 ns at 16 threads); cheap functions are
 *   faster without a cache.
 *
 * Storage policies (@c CachePolicy::TemporaryStorage, @c PersistentStorage; disk store
 * @c misc::LMDBPointValueStore, an LMDB file):
 * - Memory stays the primary store; values beyond the memory limit go to the file (in batches
 *   per shard, found in memory until written), so every evaluation is cached until the file
 *   reaches @c SourceFuncCacheConfig::storageSpaceLimit (then one warning on @c std::cerr).
 *   Lookups that miss memory read the file only once it holds points that memory does not
 *   (WP-28: 0.8-0.9 us per disk hit at 1 thread, ~60 ns wall at 16 threads; disk hits pay for
 *   functions that cost more than about a microsecond).
 * - Persistent: an existing file is checked (format, key scheme, every record; d and m fix the
 *   cache) and loaded into memory up to the limit when the cache is created; its rest stays on
 *   disk. At the end of every operator call (@c finishOperatorCall) the values the call
 *   evaluated are written to the file (WP-28: 0.4-1.7 us per new point at d = 5, sorted per
 *   call); @ref flush also writes them through to the disk. The file is locked exclusively
 *   (a second cache on it throws).
 * - Temporary: the file holds only the values beyond the memory limit and is unnamed, so it
 *   disappears with the cache or the process.
 * - A mismatching or corrupted file, a write error or points of another dimension throw
 *   @c base::file_exception (the last two at the end of the operator call).
 *
 * Adaptive policy (@c CachePolicy::Adaptive, WP-27; decisions of the owner and measurements:
 * combigrid/perf/results/WP-27/):
 * - Picks the mode (@c AdaptiveCacheMode: None, MemoryOnly, TemporaryStorage; start
 *   MemoryOnly) from sampled costs: one evaluation in P is timed (P a power of two calibrated
 *   from the cost of the timer, 64 with @c rdtscp), every 64 samples the rules of
 *   @c constants::source_func_cache (ADAPTIVE_*) compare the cost of the function c_f with the
 *   expected overhead E of a tier per avoided evaluation, (h c_hit + (1 - h) o_miss) / h:
 *   to None below 0.8 E_memory, to TemporaryStorage (memory full) above 1.5 E_disk, back below
 *   1.2 E_disk. The hit rate h is the marginal one of the last 2^14 lookups from 2^16 lookups on
 *   (before: h = 1, so that the cold start never switches caching off). Switches act at once,
 *   also in the middle of an operator call.
 * - None calls the function directly (one atomic load per evaluation; WP-27, cost-0 integrands
 *   incl. probes: 0.82-0.91x of a plain @c SourceFunc at 1 thread, 0.85-1.09x at 16). It is left by probes: after k operator calls in None (k = 1, 2, 4, ...
 *   64) one call runs in MemoryOnly and stays only if caching wins by 1.25.
 * - Each mode uses only its own tiers; values stored in another mode stay. The temporary file is
 *   created at the first value beyond the memory limit in TemporaryStorage mode (needs LMDB; if
 *   its creation fails, the cache stays in memory and the end of that operator call throws
 *   @c base::file_exception once). Points of another dimension are evaluated without the cache.
 *
 * @note Needs Boost >= 1.81 (Boost.Unordered) and xxHash >= 0.8 when SG++ is built (detected in
 * tools/defines/dependencies_defines.hpp); the storage policies need LMDB >= 0.9.14 as well.
 * The class is always compiled; without the libraries its constructor, and with it those of the
 * cached functions, throw @c base::dependency_exception.
 */
class SourceFuncCache final {
 public:
  /**
   * @brief Creates a cache: empty, or with the values of an existing persistent file.
   * @param config Configuration.
   * @throws base::dependency_exception if SG++ was built without the libraries of the cache or
   * of its policy (the message names the reason).
   * @throws std::invalid_argument if @c config.nShards is not a power of two in [1, 64] or
   * @c config.file does not fit the policy (required for the persistent policy, else empty).
   * @throws base::file_exception if the file cannot be created or opened, is locked by another
   * cache, or is no valid file of a source-function cache (another format, truncated,
   * corrupted).
   */
  explicit SourceFuncCache(const SourceFuncCacheConfig& config = SourceFuncCacheConfig());

  /// @brief Writes the values not yet in the file to it as at the end of an operator call
  /// (without the @c fsync of @ref flush; errors are ignored) and closes the file; a temporary
  /// file disappears.
  ~SourceFuncCache();

  SourceFuncCache(const SourceFuncCache&) = delete;
  SourceFuncCache& operator=(const SourceFuncCache&) = delete;

  /**
   * @brief Returns the configuration.
   * @return Configuration (with @c CachePolicy::TemporaryStorage, @c file is the path of the
   * temporary file).
   */
  const SourceFuncCacheConfig& config() const;

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
   * @param deletePersistentFile With @c CachePolicy::PersistentStorage: whether the values in
   * the file are removed as well (the cache goes on with the empty file); if false, only memory
   * is cleared and the values of the file are still found (then from disk). No effect with the
   * other policies (a temporary file is always cleared).
   * @throws base::file_exception if the file cannot be cleared.
   * @warning Must not be called while a function of the cache is evaluated.
   */
  void clear(bool deletePersistentFile = false);

  /**
   * @brief Writes the values that are not in the file yet to it. Persistent: afterwards all
   * values are in the file and on the disk (@c fsync); temporary: a non-binding request (memory
   * stays as it is). No effect with @c CachePolicy::MemoryOnly. The operators do this at their
   * end anyway (without the @c fsync); call it after evaluating a cached function directly.
   * @throws base::file_exception on a write error.
   * @warning Must not be called while a function of the cache is evaluated.
   */
  void flush();

  /**
   * @brief End-of-operator hook (called by the cached functions' @c finishOperatorCall): counts
   * the operator call; with the storage policies, writes the new values (see @ref flush).
   * @throws base::file_exception on a write error or if the call evaluated points of another
   * dimension than the cache's (storage policies).
   */
  void finishOperatorCall();

 private:
  friend class CachedSourceFunc;
  friend class CachedVectorSourceFunc;

  /**
   * @brief Fixes the number of outputs of the cached function (called by the cached functions
   * built on this cache).
   * @param nOutputs Number of outputs m (at least 1).
   * @throws std::invalid_argument if the cache holds the values of a function with another m.
   */
  void attach(size_t nOutputs);

  /**
   * @brief Returns the table of the cache, created at the first call with the number of
   * coordinates of the first point.
   * @param nDim Number of coordinates of the point to look up.
   * @return The table, or nullptr if it holds points of another dimension.
   */
  misc::BoostPointValueTable* table(size_t nDim);

  /**
   * @brief Creates the table for points with @p nDim coordinates unless another thread did.
   * @param nDim Number of coordinates.
   * @return The table.
   */
  misc::BoostPointValueTable* createTable(size_t nDim);

  /// @brief Checks @c config_.file against the policy (constructor).
  void checkConfig() const;

  /// @brief Opens or creates the file of a storage policy (constructor).
  void openStore();

  /// @brief Loads the records of an existing persistent file into a new table (constructor).
  void loadStore();

  /**
   * @brief Writes the values that are not in the store yet (hook, @ref flush).
   * @param sync Whether to write them through to the disk (persistent only).
   */
  void writeToStore(bool sync);

  /// @brief Returns the disk store (nullptr: none yet).
  misc::LMDBPointValueStore* store() const { return storeView_.load(std::memory_order_acquire); }

  /// @brief Whether the cache keeps a file (storage policies; adaptive: once created).
  bool hasStore() const { return store() != nullptr; }

  /// @name Adaptive policy (source_function_cache_adaptive.cpp)
  /// @{

  /// @brief Callback of @ref evaluateAdaptive: writes the values of the wrapped function.
  using EvaluateCallback = void (*)(const void* context, const base::DataVector& point,
                                    std::span<double> values);

  /// @brief Returns the controller of the adaptive policy (nullptr for the other policies).
  misc::AdaptiveCacheController* adaptiveController() const { return adaptive_.get(); }

  /**
   * @brief Evaluation of a cached function with the adaptive policy after the inline part of the
   * cached function (mode not None, point hashed; memory searched unless sampled): a sampled
   * point is timed and looked up in all tiers of the mode, a memory miss continues with the file
   * and the function.
   * @param table    The table.
   * @param mode     The mode read by the caller.
   * @param hash     Hash of the point.
   * @param sampled  Whether the point is timed (then memory was not searched yet).
   * @param point    Point in problem coordinates.
   * @param values   Output: its values.
   * @param evaluate Evaluates the wrapped function.
   * @param context  Argument of @p evaluate.
   */
  void evaluateAdaptive(misc::BoostPointValueTable& table, AdaptiveCacheMode mode, uint64_t hash,
                        bool sampled, const base::DataVector& point, std::span<double> values,
                        EvaluateCallback evaluate, const void* context);

  /// @brief As @ref evaluateAdaptive for a sampled point: times it and reports the sample.
  void evaluateTimed(misc::BoostPointValueTable& table, AdaptiveCacheMode mode,
                     const base::DataVector& point, std::span<double> values,
                     EvaluateCallback evaluate, const void* context);

  /**
   * @brief Continues after a memory miss: looks the point up in the file (TemporaryStorage),
   * evaluates and stores it on a miss.
   * @param functionTicks Output: the ticks of the function call (nullptr: not timed; the
   * maximum if the thread changed CPUs).
   * @return The path the evaluation took (MemoryMiss, DiskHit or DiskMiss).
   */
  misc::adaptive_caching::CostSampleKind missAdaptive(misc::BoostPointValueTable& table,
                                                      AdaptiveCacheMode mode, uint64_t hash,
                                                      const base::DataVector& point,
                                                      std::span<double> values,
                                                      EvaluateCallback evaluate,
                                                      const void* context, uint64_t* functionTicks);

  /// @brief Stores a value in the tiers of @p mode (creates the temporary file if needed).
  void insertAdaptive(misc::BoostPointValueTable& table, AdaptiveCacheMode mode,
                      const double* point, uint64_t hash, const double* values);

  /**
   * @brief Creates the temporary file of the adaptive policy unless it exists or failed before
   * (errors are kept for @ref finishOperatorCall: called inside the operators' parallel regions).
   * @param table The table to attach it to.
   * @return Whether the file exists.
   */
  bool createTemporaryStore(misc::BoostPointValueTable& table);

  /// @brief Returns the exact counters of the table for the adaptive policy.
  static misc::adaptive_caching::AdaptiveCounters adaptiveCounters(
      const misc::BoostPointValueTable& table);

  /// @brief End-of-operator part of the adaptive policy (probes, a failed temporary file).
  void finishAdaptiveOperatorCall();

  /// @}

  SourceFuncCacheConfig config_;  ///< Configuration.
  size_t nOutputs_ = 0;           ///< Number of outputs (guarded by @c mutex_; 0: not fixed yet).
  /// Disk store of the storage policies or the adaptive policy (nullptr: memory only; the
  /// adaptive policy creates it once, guarded by @c mutex_); outlives the table.
  std::unique_ptr<misc::LMDBPointValueStore> store_;
  /// @c store_ for lock-free readers (set once, after the store is complete).
  std::atomic<misc::LMDBPointValueStore*> storeView_{nullptr};
  /// Controller of the adaptive policy (nullptr for the other policies).
  std::unique_ptr<misc::AdaptiveCacheController> adaptive_;
  /// Whether evaluations at points of another dimension are an error (storage policies).
  bool otherDimIsError_ = false;
  std::string storeError_;           ///< Error of the temporary file (adaptive; guarded).
  bool storeFailed_ = false;         ///< Whether its creation failed (adaptive; guarded).
  bool storeErrorReported_ = false;  ///< Whether a hook threw it (adaptive; guarded).
  /// Table (nullptr until the first lookup or the load of a file); owned by @c ownedTable_.
  std::atomic<misc::BoostPointValueTable*> table_{nullptr};
  /// Owner of the table (guarded; a shared_ptr: its deleter is bound where the table is created,
  /// so no code of this header needs the definition of the table).
  std::shared_ptr<misc::BoostPointValueTable> ownedTable_;
  std::atomic<size_t> operatorCalls_{0};  ///< Finished operator calls.
  /// Lookups of points of another dimension than the table's (storage policies only).
  std::atomic<size_t> otherDimLookups_{0};
  size_t loadedEntries_ = 0;  ///< Records of the persistent file at its load.
  mutable std::mutex mutex_;  ///< Guards the creation of the table and the number of outputs.
  std::mutex storeMutex_;     ///< Serializes the writes of the hooks, @ref flush and @ref clear.
};

}  // namespace combigrid
}  // namespace sgpp
