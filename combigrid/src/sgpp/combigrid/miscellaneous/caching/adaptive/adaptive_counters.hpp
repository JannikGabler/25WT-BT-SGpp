// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_counters.hpp
 * @brief Exact counters of a cache's table, read by the adaptive policy at its decisions.
 */
#pragma once

#include <cstddef>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/// @brief Exact counters of the table of a cache since its creation (adaptive policy).
struct AdaptiveCounters {
  size_t lookups = 0;       ///< Lookups in memory.
  size_t hits = 0;          ///< Lookups answered from memory.
  bool memoryFull = false;  ///< Whether a value did not fit into memory (rejected or spilled).
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
