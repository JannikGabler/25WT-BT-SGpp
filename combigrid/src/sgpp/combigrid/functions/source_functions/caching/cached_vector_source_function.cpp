// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstdint>
#include <memory>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/adaptive/adaptive_cache_controller.hpp>
#include <sgpp/combigrid/miscellaneous/caching/boost_point_value_table/boost_point_value_table.hpp>
#include <stdexcept>
#include <utility>

namespace sgpp {
namespace combigrid {

namespace {

using VectorFunc = std::function<void(const base::DataVector&, std::span<double>)>;

/// Evaluation callback of the adaptive policy (SourceFuncCache::evaluateAdaptive).
void evaluateVector(const void* func, const base::DataVector& point, const std::span<double> out) {
  (*static_cast<const VectorFunc*>(func))(point, out);
}

}  // namespace

/// @name Constructors
/// @{

CachedVectorSourceFunc::CachedVectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)> func, const size_t nOutputs,
    const std::shared_ptr<SourceFuncCache>& cache)
    : VectorSourceFunc(wrap(std::move(func), nOutputs, cache), nOutputs), cache_(cache) {}

CachedVectorSourceFunc::CachedVectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)> func, const size_t nOutputs,
    const SourceFuncCacheConfig& config)
    : CachedVectorSourceFunc(std::move(func), nOutputs, std::make_shared<SourceFuncCache>(config)) {
}

/// @name Bookkeeping
/// @{

void CachedVectorSourceFunc::finishOperatorCall() const { cache_->finishOperatorCall(); }

/// @}

/// @name Getters
/// @{

std::shared_ptr<SourceFuncCache> CachedVectorSourceFunc::cache() const { return cache_; }

/// @}

/// @name Helpers
/// @{

CacheStatistics CachedVectorSourceFunc::statistics() const { return cache()->statistics(); }

/// @}

/// @name Private helpers
/// @{

std::function<void(const base::DataVector&, std::span<double>)> CachedVectorSourceFunc::wrap(
    std::function<void(const base::DataVector&, std::span<double>)> func, const size_t nOutputs,
    const std::shared_ptr<SourceFuncCache>& cache) {
  if (nOutputs == 0) {
    throw std::invalid_argument("The number of outputs must be at least 1");
  }
  if (cache == nullptr) {
    throw std::invalid_argument("The cache must not be null");
  }
  cache->attach(nOutputs);
  if (cache->adaptiveController() != nullptr) {
    return wrapAdaptive(std::move(func), cache);
  }

  return
      [func = std::move(func), cache](const base::DataVector& point, const std::span<double> out) {
        misc::BoostPointValueTable* const table = cache->table(point.size());
        if (table == nullptr) {
          func(point, out);
          return;
        }

        table->lookUpOrEvaluate(point, out, func);
      };
}

std::function<void(const base::DataVector&, std::span<double>)>
CachedVectorSourceFunc::wrapAdaptive(
    std::function<void(const base::DataVector&, std::span<double>)> func,
    const std::shared_ptr<SourceFuncCache>& cache) {
  const misc::AdaptiveCacheController* const controller = cache->adaptiveController();

  return [func = std::move(func), cache, controller](const base::DataVector& point,
                                                     const std::span<double> out) {
    const AdaptiveCacheMode mode = controller->mode();
    misc::BoostPointValueTable* const table =
        (mode == AdaptiveCacheMode::None) ? nullptr : cache->table(point.size());
    if (table == nullptr) {
      func(point, out);  // mode None (its only cost: the load and test) or another dimension
      return;
    }

    // Memory hits inline, as with the other policies; misses and timed points out of line.
    const uint64_t hash = misc::BoostPointValueTable::hash(point.data(), point.size());
    const bool sampled = controller->sampled(hash);
    if (!sampled && table->find(point.data(), hash, out.data())) {
      return;
    }
    cache->evaluateAdaptive(*table, mode, hash, sampled, point, out, &evaluateVector, &func);
  };
}

/// @}

}  // namespace combigrid
}  // namespace sgpp
