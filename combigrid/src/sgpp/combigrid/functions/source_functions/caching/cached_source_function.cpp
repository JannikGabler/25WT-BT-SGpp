// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstdint>
#include <memory>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/base/exception/dependency_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_cache_controller.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <span>
#include <stdexcept>
#include <utility>

namespace sgpp {
namespace combigrid {

namespace {

using ScalarFunc = std::function<double(const base::DataVector&)>;

/// Evaluation callback of the adaptive policy (SourceFuncCache::evaluateAdaptive).
void evaluateScalar(const void* func, const base::DataVector& point, const std::span<double> out) {
  out[0] = (*static_cast<const ScalarFunc*>(func))(point);
}

}  // namespace

/// @name Constructors
/// @{

CachedSourceFunc::CachedSourceFunc(std::function<double(const base::DataVector&)> func,
                                   const std::shared_ptr<SourceFuncCache>& cache)
    : SourceFunc(wrap(std::move(func), cache)), cache_(cache) {}

CachedSourceFunc::CachedSourceFunc(std::function<double(const base::DataVector&)> func,
                                   const SourceFuncCacheConfig& config)
    : CachedSourceFunc(std::move(func), std::make_shared<SourceFuncCache>(config)) {}

/// @}

/// @name Bookkeeping
/// @{

void CachedSourceFunc::finishOperatorCall() const { cache_->finishOperatorCall(); }

/// @}

/// @name Getters
/// @{

std::shared_ptr<SourceFuncCache> CachedSourceFunc::cache() const { return cache_; }

/// @}

/// @name Helpers
/// @{

CacheStatistics CachedSourceFunc::statistics() const { return cache()->statistics(); }

/// @}

/// @name Private helpers
/// @{

std::function<double(const base::DataVector&)> CachedSourceFunc::wrap(
    std::function<double(const base::DataVector&)> func,
    const std::shared_ptr<SourceFuncCache>& cache) {
  if (cache == nullptr) {
    throw std::invalid_argument("CachedSourceFunc::wrap: the cache must not be null");
  }
  cache->attach(1);
  if (cache->adaptiveController() != nullptr) {
    return wrapAdaptive(std::move(func), cache);
  }

  return [func = std::move(func), cache](const base::DataVector& point) {
    misc::BoostPointValueTable* const table = cache->table(point.size());
    if (table == nullptr) {
      return func(point);
    }

    double value;
    table->lookUpOrEvaluate(
        point, std::span<double>(&value, 1),
        [&func](const base::DataVector& p, const std::span<double> out) { out[0] = func(p); });
    return value;
  };
}

std::function<double(const base::DataVector&)> CachedSourceFunc::wrapAdaptive(
    std::function<double(const base::DataVector&)> func,
    const std::shared_ptr<SourceFuncCache>& cache) {
  const misc::AdaptiveCacheController* const controller = cache->adaptiveController();

  return [func = std::move(func), cache, controller](const base::DataVector& point) {
    const AdaptiveCacheMode mode = controller->mode();
    misc::BoostPointValueTable* const table =
        (mode == AdaptiveCacheMode::None) ? nullptr : cache->table(point.size());
    if (table == nullptr) {
      return func(point);  // mode None (its only cost: the load and test) or another dimension
    }

    // Memory hits inline, as with the other policies; misses and timed points out of line.
    const uint64_t hash = misc::BoostPointValueTable::hash(point.data(), point.size());
    const bool sampled = controller->sampled(hash);
    double value;
    if (!sampled && table->find(point.data(), hash, &value)) {
      return value;
    }
    cache->evaluateAdaptive(*table, mode, hash, sampled, point, std::span<double>(&value, 1),
                            &evaluateScalar, &func);
    return value;
  };
}

/// @}

}  // namespace combigrid
}  // namespace sgpp
