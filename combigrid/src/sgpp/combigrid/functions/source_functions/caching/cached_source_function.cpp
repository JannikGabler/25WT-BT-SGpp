// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <memory>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/point_value_table.hpp>
#include <span>
#include <utility>

namespace sgpp {
namespace combigrid {

CachedSourceFunc::CachedSourceFunc(std::function<double(const base::DataVector&)> func,
                                   const std::shared_ptr<SourceFuncCache>& cache)
    : SourceFunc(wrap(std::move(func), cache), cache) {}

CachedSourceFunc::CachedSourceFunc(std::function<double(const base::DataVector&)> func,
                                   const CacheConfig& config)
    : CachedSourceFunc(std::move(func), std::make_shared<SourceFuncCache>(config)) {}

std::shared_ptr<SourceFuncCache> CachedSourceFunc::cache() const {
  // The hook of a cached function is always its cache (see the constructors).
  return std::static_pointer_cast<SourceFuncCache>(hook());
}

CacheStatistics CachedSourceFunc::statistics() const { return cache()->statistics(); }

std::function<double(const base::DataVector&)> CachedSourceFunc::wrap(
    std::function<double(const base::DataVector&)> func,
    const std::shared_ptr<SourceFuncCache>& cache) {
  SourceFuncCache::requireAvailable();
  if (cache == nullptr) {
    throw base::data_exception("CachedSourceFunc: the cache must not be null");
  }
  cache->attach(1);

  return [func = std::move(func), cache](const base::DataVector& point) {
    misc::PointValueTable* const table = cache->table(point.size());
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

}  // namespace combigrid
}  // namespace sgpp
