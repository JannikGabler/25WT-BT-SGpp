// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file source_function_cache_config.hpp
 * @brief Configuration of a source-function cache (@c SourceFuncCache).
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_policy.hpp>
#include <string>

namespace sgpp {
namespace combigrid {

/**
 * @brief Configuration of a @c SourceFuncCache: an aggregate with defaults (C++:
 * @c SourceFuncCacheConfig{.memoryLimit = ...}; Python: attributes of
 * @c pysgpp.SourceFuncCacheConfig()).
 */
struct SourceFuncCacheConfig {
  /**
   * @brief Soft limit of the memory of the cache in bytes (records and their index; default
   * 1 GiB, about 25 / 9.5 / 5.4 million scalar records at d = 2 / 10 / 20).
   *
   * The limit is split evenly among the shards of the cache. A shard whose share is used up
   * stores no further values in memory: with @c CachePolicy::MemoryOnly the operators evaluate
   * those points again in later calls (counted in @c CacheStatistics::rejectedInserts), with
   * the storage policies they go to the file. Values stored before are still found. The memory
   * may exceed the limit by about one chunk of records and one growth of the index per shard
   * (storage policies: plus one buffer of @c constants::source_func_cache::SPILL_BATCH_RECORDS
   * records per shard). Shards allocate whole chunks of 128 records, so a limit below one chunk
   * per shard (nShards * 128 * 8 (d + m + 1) bytes, e.g. 320 KiB at d = 3, m = 1 with 64
   * shards) stores nothing in memory. The operating system's page cache of the file is not
   * counted.
   */
  size_t memoryLimit = constants::source_func_cache::DEFAULT_MEMORY_LIMIT;

  /**
   * @brief Number of shards of the table (a power of two in [1, 64], default 64; checked by the
   * @c SourceFuncCache constructor). More shards: less lock contention between threads, but
   * also more chunks allocated up front per shard (see @ref memoryLimit).
   */
  size_t nShards = constants::source_func_cache::DEFAULT_SHARDS_CNT;

  /// @brief Where the values are kept (default memory only; see @c CachePolicy).
  CachePolicy policy = CachePolicy::MemoryOnly;

  /**
   * @brief File of @c CachePolicy::PersistentStorage (required; created if missing, else its
   * values are loaded). Empty for the other policies; with @c CachePolicy::TemporaryStorage the
   * cache sets it to the path of its temporary file (@c SourceFuncCache::config).
   *
   * @warning The file identifies its values only by the dimension d and the number of outputs m
   * of the function: using the file of another function with the same d and m returns that
   * function's values. Use one file per function (grids, domains and operators may differ).
   * LMDB keeps a lock file <tt>file + "-lock"</tt> next to it.
   */
  std::string file{};

  /**
   * @brief Limit of the size of the file of the storage policies in bytes. Once it is reached,
   * the cache prints one warning on @c std::cerr and stores no further values (counted in @c
   * CacheStatistics::rejectedInserts); values stored before are still found.
   */
  size_t storageSpaceLimit = constants::source_func_cache::DEFAULT_STORAGE_SPACE_LIMIT;
};

}  // namespace combigrid
}  // namespace sgpp
