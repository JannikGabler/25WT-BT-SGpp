// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_cache_controller.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_rules.hpp>
#include <vector>

using sgpp::combigrid::misc::adaptive_caching::AdaptiveCounters;
using sgpp::combigrid::misc::adaptive_caching::AdaptiveRuleInput;
using sgpp::combigrid::misc::adaptive_caching::COST_SAMPLE_KIND_CNT;
using sgpp::combigrid::misc::adaptive_caching::CostEstimate;
using sgpp::combigrid::misc::adaptive_caching::CostSample;
using sgpp::combigrid::misc::adaptive_caching::CostSampleKind;

namespace sgpp {
namespace combigrid {
namespace misc {

namespace {

namespace cfg = constants::source_func_cache;

/// Adds the mean of @p values without the largest and smallest eighth (preemptions, migrations).
void addTrimmed(CostEstimate& estimate, std::vector<double>& values) {
  if (values.empty()) {
    return;
  }
  std::sort(values.begin(), values.end());
  const size_t trim = values.size() / 8;
  double sum = 0;
  for (size_t i = trim; i < values.size() - trim; i++) sum += values[i];
  estimate.add(sum / static_cast<double>(values.size() - 2 * trim), values.size());
}

double knownOrNegative(const CostEstimate& estimate) {
  return estimate.known() ? estimate.mean() : -1.0;
}

double toNs(const CostEstimate& estimate, const double ticksPerNs) {
  return estimate.mean() / ticksPerNs;
}

}  // namespace

AdaptiveCacheController::AdaptiveCacheController(const bool diskAvailable,
                                                 const unsigned samplingShift,
                                                 const double readCostTicks)
    : samplingMask_((uint64_t{1} << samplingShift) - 1),
      samplingShift_(samplingShift),
      readCostTicks_(readCostTicks),
      diskAvailable_(diskAvailable) {}

void AdaptiveCacheController::decide(const uint64_t window, const AdaptiveCounters& counters) {
  const std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);
  if (!lock.owns_lock()) {
    return;
  }
  update(window_.collect(window));
  memoryHitRate_.update(counters.lookups, counters.hits, cfg::ADAPTIVE_HIT_RATE_LOOKUPS);
  apply(counters);
}

void AdaptiveCacheController::finishOperatorCall(const AdaptiveCounters& counters) {
  const std::lock_guard<std::mutex> lock(mutex_);
  if (mode() != AdaptiveCacheMode::None) {
    if (probing_) finishProbe(counters);
    return;
  }
  noneOperatorCalls_++;
  if (switchedOffInCall_) {
    switchedOffInCall_ = false;  // that call ran cached until the switch
    return;
  }
  if (++callsInNone_ >= probeInterval_) {
    startProbe(counters);
  }
}

void AdaptiveCacheController::disableDisk() {
  const std::lock_guard<std::mutex> lock(mutex_);
  diskAvailable_ = false;
  if (mode() == AdaptiveCacheMode::TemporaryStorage) {
    setMode(AdaptiveCacheMode::MemoryOnly);
  }
}

void AdaptiveCacheController::reset() {
  const std::lock_guard<std::mutex> lock(mutex_);
  window_.clear();
  functionCost_ = memoryHitCost_ = memoryMissOverhead_ = CostEstimate();
  diskHitCost_ = diskMissOverhead_ = CostEstimate();
  memoryHitRate_.reset(0, 0);
  diskHitRate_.reset(0, 0);
  diskQueries_ = diskHits_ = 0;
  probing_ = false;
  probeInterval_ = 1;
  callsInNone_ = modeSwitches_ = noneOperatorCalls_ = 0;
  switchedOffInCall_ = false;
  mode_.store(AdaptiveCacheMode::MemoryOnly, std::memory_order_relaxed);
}

void AdaptiveCacheController::addStatistics(CacheStatistics& stats, const double ticksPerNs) const {
  const std::lock_guard<std::mutex> lock(mutex_);
  stats.adaptiveMode = mode();
  stats.modeSwitches = modeSwitches_;
  stats.noneOperatorCalls = noneOperatorCalls_;
  stats.functionCostNs = toNs(functionCost_, ticksPerNs);
  stats.memoryHitCostNs = toNs(memoryHitCost_, ticksPerNs);
  stats.memoryMissOverheadNs = toNs(memoryMissOverhead_, ticksPerNs);
  stats.diskHitCostNs = toNs(diskHitCost_, ticksPerNs);
  stats.diskMissOverheadNs = toNs(diskMissOverhead_, ticksPerNs);
}

AdaptiveRuleInput AdaptiveCacheController::ruleInput(const AdaptiveCounters& counters) const {
  AdaptiveRuleInput in;
  in.mode = mode();
  in.probing = probing_;
  in.functionCost = knownOrNegative(functionCost_);
  in.memoryMissOverhead = knownOrNegative(memoryMissOverhead_);
  // Without any hit (hit rate 0, e.g. the first window of a cache at 16 threads or a deltas
  // sparse grid), a miss's overhead (lookup and insert) stands in for the hit cost.
  in.memoryHitCost = memoryHitCost_.known() ? memoryHitCost_.mean() : in.memoryMissOverhead;
  in.diskHitCost = knownOrNegative(diskHitCost_);
  in.diskMissOverhead = knownOrNegative(diskMissOverhead_);
  if (counters.lookups >= cfg::ADAPTIVE_WARMUP_LOOKUPS) {
    in.memoryHitRate =
        memoryHitRate_.rate(counters.lookups, counters.hits, cfg::ADAPTIVE_HIT_RATE_LOOKUPS);
  }
  if (diskQueries_ >= cfg::ADAPTIVE_WARMUP_LOOKUPS) {
    in.diskHitRate = diskHitRate_.rate(diskQueries_, diskHits_, cfg::ADAPTIVE_HIT_RATE_LOOKUPS);
  }
  in.memoryFull = counters.memoryFull;
  in.diskAvailable = diskAvailable_;
  return in;
}

/// @name Private helpers
/// @{

void AdaptiveCacheController::update(const std::vector<CostSample>& samples) {
  // Corrected for the timer: an interval holds one read; a miss's total holds the two inner
  // reads of the function interval in addition.
  std::array<std::vector<double>, COST_SAMPLE_KIND_CNT> overheads;
  std::vector<double> functions;
  for (const CostSample& s : samples) {
    const bool miss = s.kind == CostSampleKind::MemoryMiss || s.kind == CostSampleKind::DiskMiss;
    const double total = static_cast<double>(s.totalTicks) - (miss ? 3 : 1) * readCostTicks_;
    const double function = static_cast<double>(s.functionTicks) - readCostTicks_;
    overheads[static_cast<size_t>(s.kind)].push_back(std::max(total - (miss ? function : 0), 0.0));
    if (miss) functions.push_back(std::max(function, 0.0));
  }

  const size_t nDiskHits = overheads[static_cast<size_t>(CostSampleKind::DiskHit)].size();
  diskQueries_ += (nDiskHits + overheads[static_cast<size_t>(CostSampleKind::DiskMiss)].size())
                  << samplingShift_;
  diskHits_ += nDiskHits << samplingShift_;
  diskHitRate_.update(diskQueries_, diskHits_, cfg::ADAPTIVE_HIT_RATE_LOOKUPS);

  addTrimmed(functionCost_, functions);
  addTrimmed(memoryHitCost_, overheads[static_cast<size_t>(CostSampleKind::MemoryHit)]);
  addTrimmed(memoryMissOverhead_, overheads[static_cast<size_t>(CostSampleKind::MemoryMiss)]);
  addTrimmed(diskHitCost_, overheads[static_cast<size_t>(CostSampleKind::DiskHit)]);
  addTrimmed(diskMissOverhead_, overheads[static_cast<size_t>(CostSampleKind::DiskMiss)]);
}

void AdaptiveCacheController::apply(const AdaptiveCounters& counters) {
  if (mode() == AdaptiveCacheMode::None) {
    return;  // switched off by a concurrent window: only probes leave None
  }
  setMode(adaptive_caching::nextAdaptiveMode(ruleInput(counters)));
}

void AdaptiveCacheController::setMode(const AdaptiveCacheMode mode) {
  if (mode == this->mode()) {
    return;
  }
  modeSwitches_++;
  if (mode == AdaptiveCacheMode::None) {
    callsInNone_ = 0;
    switchedOffInCall_ = true;
    if (probing_) {
      probing_ = false;
      probeInterval_ = std::min(2 * probeInterval_, cfg::ADAPTIVE_MAX_PROBE_INTERVAL);
    }
  }
  mode_.store(mode, std::memory_order_relaxed);
}

void AdaptiveCacheController::startProbe(const AdaptiveCounters& counters) {
  setMode(AdaptiveCacheMode::MemoryOnly);
  probing_ = true;
  probeStartLookups_ = counters.lookups;
  probeStartHits_ = counters.hits;
  memoryHitRate_.reset(counters.lookups, counters.hits);
}

void AdaptiveCacheController::finishProbe(const AdaptiveCounters& counters) {
  const size_t lookups = counters.lookups - probeStartLookups_;
  AdaptiveRuleInput in = ruleInput(counters);
  if (lookups > 0) {
    in.memoryHitRate =
        static_cast<double>(counters.hits - probeStartHits_) / static_cast<double>(lookups);
  }
  const AdaptiveCacheMode next = adaptive_caching::nextAdaptiveMode(in);
  if (next != AdaptiveCacheMode::None) {
    probing_ = false;
    probeInterval_ = 1;
  }
  setMode(next);
}

/// @}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
