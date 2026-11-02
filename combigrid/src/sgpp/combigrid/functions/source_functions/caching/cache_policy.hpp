// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cache_policy.hpp
 * @brief Where a source-function cache (@c SourceFuncCache) keeps its values.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief Where a @c SourceFuncCache keeps its values (@c SourceFuncCacheConfig::policy).
 *
 * With the storage policies, memory stays the primary store (@c SourceFuncCacheConfig::memoryLimit,
 * records and indices) and every further value goes to a file of at most
 * @c SourceFuncCacheConfig::storageSpaceLimit bytes, so that every evaluation is cached until the
 * file is full. @c Adaptive chooses among no caching and the first two by itself. Python:
 * @c pysgpp.CachePolicy_MemoryOnly, ...
 */
enum class CachePolicy {
  /// Memory only (default): no file; once the memory limit is reached, no further values are
  /// stored. @c SourceFuncCacheConfig::file must be empty.
  MemoryOnly,
  /// Memory and a temporary file that is deleted when the cache is destroyed or the process
  /// ends, however it ends (an unnamed file in @c $TMPDIR, else @c /var/tmp). The file holds the
  /// values beyond the memory limit. @c SourceFuncCacheConfig::file must be empty; the cache sets
  /// it to the path of the file while it lives (@c /proc/self/fd/N on Linux).
  TemporaryStorage,
  /// Memory and a persistent file @c SourceFuncCacheConfig::file (created if missing, else
  /// loaded): it holds all values and stays after the process. After every operator call of an
  /// uninterrupted run, all values the call evaluated are in the file; a crash never corrupts
  /// it (values of an interrupted operator call may be lost).
  PersistentStorage,
  /// Picks None (no caching), MemoryOnly or TemporaryStorage by itself, from the measured cost of
  /// the function, of the cache's lookups and its hit rate (WP-27; starts in MemoryOnly; the
  /// current mode: @c CacheStatistics::adaptiveMode). The temporary file is created at the first
  /// value beyond the memory limit in TemporaryStorage mode (only with LMDB, else never) and kept
  /// until the cache is destroyed. @c SourceFuncCacheConfig::file must be empty. Results are
  /// bitwise those of the uncached function in every mode. See @c SourceFuncCache.
  Adaptive,
};

}  // namespace combigrid
}  // namespace sgpp
