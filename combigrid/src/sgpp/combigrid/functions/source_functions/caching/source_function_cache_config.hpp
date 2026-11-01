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
   * stores no further values: the operators evaluate those points again in later calls, values
   * stored before are still found (rejected values are counted in
   * @c CacheStatistics::rejectedInserts). The memory may exceed the limit by about one chunk of
   * records and one growth of the index per shard. Shards allocate whole chunks of 128 records,
   * so a limit below one chunk per shard (nShards * 128 * 8 (d + m + 1) bytes, e.g. 320 KiB at
   * d = 3, m = 1 with 64 shards) stores nothing.
   */
  size_t memoryLimit = constants::source_func_cache::DEFAULT_MEMORY_LIMIT;

  /**
   * @brief Number of shards of the table (a power of two in [1, 64], default 64; checked by the
   * @c SourceFuncCache constructor). More shards: less lock contention between threads, but
   * also more chunks allocated up front per shard (see @ref memoryLimit).
   */
  size_t nShards = constants::source_func_cache::DEFAULT_SHARDS_CNT;
};

}  // namespace combigrid
}  // namespace sgpp
