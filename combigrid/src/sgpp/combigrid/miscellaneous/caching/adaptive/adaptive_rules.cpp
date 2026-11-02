// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <limits>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_rules.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {
namespace adaptive_caching {

namespace {

namespace cfg = constants::source_func_cache;

AdaptiveCacheMode nextFromMemoryOnly(const AdaptiveRuleInput& in) {
  if (!in.diskAvailable || !in.memoryFull) {
    return AdaptiveCacheMode::MemoryOnly;
  }
  const double disk = diskOverhead(in);
  const double overhead = (disk >= 0) ? disk : memoryOverhead(in);
  return (in.functionCost > cfg::ADAPTIVE_TO_DISK_FACTOR * overhead)
             ? AdaptiveCacheMode::TemporaryStorage
             : AdaptiveCacheMode::MemoryOnly;
}

AdaptiveCacheMode nextFromTemporaryStorage(const AdaptiveRuleInput& in) {
  if (!in.diskAvailable) {
    return AdaptiveCacheMode::MemoryOnly;
  }
  const double disk = diskOverhead(in);
  return (disk >= 0 && in.functionCost <= cfg::ADAPTIVE_FROM_DISK_FACTOR * disk)
             ? AdaptiveCacheMode::MemoryOnly
             : AdaptiveCacheMode::TemporaryStorage;
}

}  // namespace

double expectedOverhead(const double hitCost, const double missOverhead, const double hitRate) {
  if (hitRate < 0 || missOverhead < 0) {
    return hitCost;
  }
  if (hitRate == 0) {
    return std::numeric_limits<double>::infinity();
  }
  return (hitRate * hitCost + (1 - hitRate) * missOverhead) / hitRate;
}

double memoryOverhead(const AdaptiveRuleInput& in) {
  return expectedOverhead(in.memoryHitCost, in.memoryMissOverhead, in.memoryHitRate);
}

double diskOverhead(const AdaptiveRuleInput& in) {
  if (in.diskHitRate >= 0 && in.diskMissOverhead >= 0) {
    return expectedOverhead(in.diskHitCost, in.diskMissOverhead, in.diskHitRate);
  }
  return in.diskHitCost;  // the bound for h = 1, or unknown
}

AdaptiveCacheMode nextAdaptiveMode(const AdaptiveRuleInput& in) {
  if (in.mode == AdaptiveCacheMode::None || in.functionCost < 0 || in.memoryHitCost < 0) {
    return in.mode;
  }
  if (in.mode == AdaptiveCacheMode::TemporaryStorage && in.memoryFull) {
    // The file is in use: new points go there, the hit rate of memory says nothing about the
    // cache. Only the file's rule applies (the chain None <- MemoryOnly <- TemporaryStorage).
    return nextFromTemporaryStorage(in);
  }
  const double noneFactor =
      in.probing ? cfg::ADAPTIVE_FROM_NONE_FACTOR : cfg::ADAPTIVE_TO_NONE_FACTOR;
  const bool loses = in.probing ? in.functionCost <= noneFactor * memoryOverhead(in)
                                : in.functionCost < noneFactor * memoryOverhead(in);
  if (loses) {
    return AdaptiveCacheMode::None;
  }
  return (in.mode == AdaptiveCacheMode::MemoryOnly) ? nextFromMemoryOnly(in)
                                                    : nextFromTemporaryStorage(in);
}

}  // namespace adaptive_caching
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
