// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_rules.hpp
 * @brief Switching rules of the adaptive caching policy (pure functions of measured costs).
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_rule_input.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief Expected overhead of a cache tier per evaluation it avoids (WP-25 cost model): of the
 * lookups, a share h is answered (cost @p hitCost each, instead of the function), the rest pays
 * @p missOverhead on top of the function, so each avoided call costs
 * @f$(h c_{hit} + (1 - h) o_{miss}) / h@f$.
 * @param hitCost      Cost of a hit.
 * @param missOverhead Overhead of a miss (negative: unknown).
 * @param hitRate      Hit rate h (negative: unknown).
 * @return The overhead; @p hitCost if h or the miss overhead is unknown (the bound for h = 1),
 * infinity for h = 0.
 */
double expectedOverhead(double hitCost, double missOverhead, double hitRate);

/**
 * @brief Expected overhead of the memory tier per avoided evaluation (@ref expectedOverhead).
 * @param in Measured state (@c memoryHitCost must be known).
 * @return The overhead.
 */
double memoryOverhead(const AdaptiveRuleInput& in);

/**
 * @brief Expected overhead of the temporary file per avoided evaluation (@ref expectedOverhead
 * over the lookups that miss memory).
 * @param in Measured state.
 * @return The overhead, or a negative value if the costs of the file are unknown.
 */
double diskOverhead(const AdaptiveRuleInput& in);

/**
 * @brief The owner's switching rules (WP-27; factors in @c constants::source_func_cache), with
 * c_f the cost of the function and E the expected overhead of a tier per avoided evaluation:
 * - MemoryOnly / TemporaryStorage -> None if c_f < 0.8 E_memory (in a probe from None: unless
 *   c_f > 1.25 E_memory);
 * - MemoryOnly -> TemporaryStorage once memory is full, if c_f > 1.5 E_disk (before the file
 *   was ever measured: E_memory, a lower bound of E_disk);
 * - TemporaryStorage -> MemoryOnly if c_f <= 1.2 E_disk, or if no file can be used; while
 *   memory is full (the file in use), this is the only rule of TemporaryStorage (None only via
 *   MemoryOnly: the hit rate of memory then misses the hits of the file).
 *
 * None is only left by a probe (@c AdaptiveCacheController::finishOperatorCall). An unknown hit
 * rate gives the bound for h = 1: during the cold start of a cache, caching is only switched
 * off if even all hits would not pay.
 * @param in Measured state.
 * @return The next mode (the current one while the costs are unknown).
 */
AdaptiveCacheMode nextAdaptiveMode(const AdaptiveRuleInput& in);

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
