// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file hit_rate_tracker.hpp
 * @brief Hit rate of the most recent lookups (adaptive caching policy).
 */
#pragma once

#include <cstddef>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

/**
 * @brief Marginal hit rate: the hit rate of the most recent lookups (between one and two spans
 * of lookups), from cumulative counters.
 *
 * The cumulative hit rate of a cache lags far behind: early in an operator call most lookups are
 * first visits (WP-27: quad sg cc d5 0.09 after 4k lookups at 16 threads, 0.83 over the call).
 */
class HitRateTracker {
 public:
  /**
   * @brief Starts over at the given counters (no rate until a span of lookups followed).
   * @param lookups Cumulative lookups.
   * @param hits    Cumulative hits.
   */
  void reset(size_t lookups, size_t hits);

  /**
   * @brief Advances the anchors: once @p span lookups followed the newer anchor, it becomes the
   * older one and the counters the newer one.
   * @param lookups Cumulative lookups.
   * @param hits    Cumulative hits.
   * @param span    Lookups between anchors.
   */
  void update(size_t lookups, size_t hits, size_t span);

  /**
   * @brief Returns the hit rate since the older anchor.
   * @param lookups Cumulative lookups.
   * @param hits    Cumulative hits.
   * @param span    Minimum number of lookups since the older anchor.
   * @return The rate, or a negative value if fewer than @p span lookups followed the anchor.
   */
  double rate(size_t lookups, size_t hits, size_t span) const;

 private:
  size_t olderLookups_ = 0;  ///< Lookups at the older anchor.
  size_t olderHits_ = 0;     ///< Hits at the older anchor.
  size_t newerLookups_ = 0;  ///< Lookups at the newer anchor.
  size_t newerHits_ = 0;     ///< Hits at the newer anchor.
};

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
