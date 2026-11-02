// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// The adaptive policy of SourceFuncCache (CachePolicy::Adaptive, WP-27).

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <exception>
#include <limits>
#include <mutex>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_cache_controller.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_counters.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_sample_kind.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/cost_timer.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table.hpp>
#include <sgpp/combigrid/miscellaneous/caching/lmdb_point_value_store/lmdb_point_value_store.hpp>
#include <sgpp/combigrid/miscellaneous/exceptions/cache_file_exception.hpp>
#include <span>
#include <string>

using sgpp::combigrid::misc::BoostPointValueTable;
using sgpp::combigrid::misc::LMDBPointValueStore;
using sgpp::combigrid::misc::adaptive_caching::AdaptiveCounters;
using sgpp::combigrid::misc::adaptive_caching::CostSample;
using sgpp::combigrid::misc::adaptive_caching::CostSampleKind;
using sgpp::combigrid::misc::adaptive_caching::CostTimer;

namespace sgpp {
namespace combigrid {

namespace {

/// Marks a function interval during which the thread changed CPUs (not comparable).
constexpr uint64_t INVALID_TICKS = std::numeric_limits<uint64_t>::max();

}  // namespace

void SourceFuncCache::evaluateAdaptive(BoostPointValueTable& table, const AdaptiveCacheMode mode,
                                       const uint64_t hash, const bool sampled,
                                       const base::DataVector& point,
                                       const std::span<double> values,
                                       const EvaluateCallback evaluate, const void* context) {
  if (sampled) {
    evaluateTimed(table, mode, point, values, evaluate, context);
  } else {
    missAdaptive(table, mode, hash, point, values, evaluate, context, nullptr);
  }
}

void SourceFuncCache::evaluateTimed(BoostPointValueTable& table, const AdaptiveCacheMode mode,
                                    const base::DataVector& point, const std::span<double> values,
                                    const EvaluateCallback evaluate, const void* context) {
  uint32_t startCpu = 0;
  uint32_t endCpu = 0;
  const uint64_t start = CostTimer::read(startCpu);
  // Hashed again inside the interval, so that the sample holds the whole evaluation.
  const uint64_t hash = BoostPointValueTable::hash(point.data(), point.size());
  uint64_t functionTicks = 0;
  const CostSampleKind kind =
      table.find(point.data(), hash, values.data())
          ? CostSampleKind::MemoryHit
          : missAdaptive(table, mode, hash, point, values, evaluate, context, &functionTicks);
  const uint64_t end = CostTimer::read(endCpu);

  if (startCpu != endCpu || functionTicks == INVALID_TICKS) {
    return;  // moved to another CPU: the time-stamp counters may differ
  }
  if (const auto window = adaptive_->addSample(CostSample{kind, end - start, functionTicks})) {
    adaptive_->decide(*window, adaptiveCounters(table));
  }
}

misc::adaptive_caching::CostSampleKind SourceFuncCache::missAdaptive(
    BoostPointValueTable& table, const AdaptiveCacheMode mode, const uint64_t hash,
    const base::DataVector& point, const std::span<double> values, const EvaluateCallback evaluate,
    const void* context, uint64_t* const functionTicks) {
  const bool disk = mode == AdaptiveCacheMode::TemporaryStorage && table.onDisk();
  if (disk && table.findOnDisk(point.data(), hash, values.data())) {
    return CostSampleKind::DiskHit;
  }

  if (functionTicks == nullptr) {
    evaluate(context, point, values);
  } else {
    uint32_t startCpu = 0;
    uint32_t endCpu = 0;
    const uint64_t start = CostTimer::read(startCpu);
    evaluate(context, point, values);
    const uint64_t end = CostTimer::read(endCpu);
    *functionTicks = (startCpu == endCpu) ? end - start : INVALID_TICKS;
  }
  if (std::ranges::none_of(values, [](const double value) { return std::isnan(value); })) {
    insertAdaptive(table, mode, point.data(), hash, values.data());
  }
  return disk ? CostSampleKind::DiskMiss : CostSampleKind::MemoryMiss;
}

void SourceFuncCache::insertAdaptive(BoostPointValueTable& table, const AdaptiveCacheMode mode,
                                     const double* point, const uint64_t hash,
                                     const double* values) {
  const bool spill = mode == AdaptiveCacheMode::TemporaryStorage;
  if (table.insert(point, hash, values, spill) || !spill || hasStore()) {
    return;
  }
  // The first value beyond the memory limit in TemporaryStorage mode: create the file (the
  // rejected insert above stays counted).
  if (createTemporaryStore(table)) {
    table.insert(point, hash, values, true);
  }
}

bool SourceFuncCache::createTemporaryStore(BoostPointValueTable& table) {
  const std::lock_guard<std::mutex> guard(mutex_);
  if (store_ == nullptr && !storeFailed_) {
    try {
      store_ = LMDBPointValueStore::createTemporary(LMDBPointValueStore::temporaryDirectory(),
                                                    config_.storageSpaceLimit, config_.nShards);
      storeView_.store(store_.get(), std::memory_order_release);
      table.setStore(store_.get());
    } catch (const std::exception& error) {
      // Inside a parallel region: kept for the end of the operator call.
      storeFailed_ = true;
      storeError_ = error.what();
      adaptive_->disableDisk();
    }
  }
  return store_ != nullptr;
}

AdaptiveCounters SourceFuncCache::adaptiveCounters(const BoostPointValueTable& table) {
  const CacheStatistics stats = table.statistics();
  return AdaptiveCounters{stats.lookups, stats.hits, stats.rejectedInserts > 0 || table.onDisk()};
}

void SourceFuncCache::finishAdaptiveOperatorCall() {
  const BoostPointValueTable* const table = table_.load(std::memory_order_acquire);
  adaptive_->finishOperatorCall((table != nullptr) ? adaptiveCounters(*table) : AdaptiveCounters());

  std::string error;
  {
    const std::lock_guard<std::mutex> guard(mutex_);
    if (storeFailed_ && !storeErrorReported_) {
      storeErrorReported_ = true;
      error = storeError_;
    }
  }
  if (!error.empty()) {
    throw CacheFileException(
        "SourceFuncCache: the temporary file of the adaptive policy could not be created, the "
        "cache goes on in memory only: " +
        error);
  }
}

}  // namespace combigrid
}  // namespace sgpp
