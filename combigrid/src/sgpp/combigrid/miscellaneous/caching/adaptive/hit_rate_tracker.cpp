// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/hit_rate_tracker.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

void HitRateTracker::reset(const size_t lookups, const size_t hits) {
  olderLookups_ = newerLookups_ = lookups;
  olderHits_ = newerHits_ = hits;
}

void HitRateTracker::update(const size_t lookups, const size_t hits, const size_t span) {
  if (lookups >= newerLookups_ + span) {
    olderLookups_ = newerLookups_;
    olderHits_ = newerHits_;
    newerLookups_ = lookups;
    newerHits_ = hits;
  }
}

double HitRateTracker::rate(const size_t lookups, const size_t hits, const size_t span) const {
  if (lookups < olderLookups_ + span || lookups == olderLookups_) {
    return -1.0;
  }
  return static_cast<double>(hits - olderHits_) / static_cast<double>(lookups - olderLookups_);
}

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
