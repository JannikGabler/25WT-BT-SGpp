// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cache_statistics.hpp
 * @brief Counters of a source-function cache (@c SourceFuncCache).
 */
#pragma once

#include <cstddef>

namespace sgpp {
namespace combigrid {

/**
 * @brief Counters of a @c SourceFuncCache since its creation or its last @c clear.
 *
 * Misses are @c lookups - @c hits; each miss is one evaluation of the wrapped function. Not
 * every miss is stored: values with a NaN, values beyond the memory limit
 * (@c rejectedInserts) and a second value of a point evaluated by two threads at the same time
 * are not.
 */
struct CacheStatistics {
  size_t lookups = 0;          ///< Evaluations of the cached functions looked up in the cache.
  size_t hits = 0;             ///< Lookups answered from the cache.
  size_t inserts = 0;          ///< Points whose values were stored.
  size_t rejectedInserts = 0;  ///< Values not stored because of the memory limit.
  size_t entries = 0;          ///< Points stored at present.
  size_t bytes = 0;            ///< Memory counted against the limit (records and index).
  size_t operatorCalls = 0;    ///< Finished operator calls with a function of the cache.
};

}  // namespace combigrid
}  // namespace sgpp
