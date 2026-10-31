// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <memory>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/point_value_table.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

CachedVectorSourceFunc::CachedVectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)> func, const size_t nOutputs,
    const std::shared_ptr<SourceFuncCache>& cache)
    : VectorSourceFunc(wrap(std::move(func), nOutputs, cache), nOutputs, cache) {}

CachedVectorSourceFunc::CachedVectorSourceFunc(
    std::function<void(const base::DataVector&, std::span<double>)> func, const size_t nOutputs,
    const CacheConfig& config)
    : CachedVectorSourceFunc(std::move(func), nOutputs, std::make_shared<SourceFuncCache>(config)) {
}

std::shared_ptr<SourceFuncCache> CachedVectorSourceFunc::cache() const {
  // The hook of a cached function is always its cache (see the constructors).
  return std::static_pointer_cast<SourceFuncCache>(hook());
}

CacheStatistics CachedVectorSourceFunc::statistics() const { return cache()->statistics(); }

std::function<void(const base::DataVector&, std::span<double>)> CachedVectorSourceFunc::wrap(
    std::function<void(const base::DataVector&, std::span<double>)> func, const size_t nOutputs,
    const std::shared_ptr<SourceFuncCache>& cache) {
  SourceFuncCache::requireAvailable();
  if (nOutputs == 0) {
    throw base::data_exception("VectorSourceFunc: the number of outputs must be at least 1");
  }
  if (cache == nullptr) {
    throw base::data_exception("CachedVectorSourceFunc: the cache must not be null");
  }
  cache->attach(nOutputs);

  return
      [func = std::move(func), cache](const base::DataVector& point, const std::span<double> out) {
        misc::PointValueTable* const table = cache->table(point.size());
        if (table == nullptr) {
          func(point, out);
          return;
        }

        table->lookUpOrEvaluate(point, out, func);
      };
}

}  // namespace combigrid
}  // namespace sgpp
