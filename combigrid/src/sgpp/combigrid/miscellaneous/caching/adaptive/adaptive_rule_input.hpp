// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_rule_input.hpp
 * @brief Measured state from which the adaptive caching policy picks its mode.
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief Input of @c nextAdaptiveMode. Costs are per evaluation in one unit (ticks of
 * @c CostTimer, without the cost of the timer); a negative cost or hit rate is unknown.
 */
struct AdaptiveRuleInput {
  AdaptiveCacheMode mode = AdaptiveCacheMode::MemoryOnly;  ///< Current mode.
  /// Whether the current mode is a probe from None (then caching must win by
  /// @c ADAPTIVE_FROM_NONE_FACTOR instead of losing by @c ADAPTIVE_TO_NONE_FACTOR).
  bool probing = false;
  double functionCost = -1;        ///< Function call alone.
  double memoryHitCost = -1;       ///< Evaluation answered from memory.
  double memoryMissOverhead = -1;  ///< Evaluation missing memory, without the function call.
  double diskHitCost = -1;         ///< Evaluation answered from the temporary file.
  double diskMissOverhead = -1;    ///< Evaluation missing both, without the function call.
  double memoryHitRate = -1;       ///< Marginal hit rate of memory (unknown: bound h = 1).
  double diskHitRate = -1;         ///< Marginal hit rate of the file among memory misses.
  bool memoryFull = false;         ///< Whether a value did not fit into memory.
  bool diskAvailable = false;      ///< Whether a temporary file can be used (LMDB, no error).
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
