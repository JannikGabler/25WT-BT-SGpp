// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cached_source_function.hpp
 * @brief Source function @f$f: \mathbb{R}^d \to \mathbb{R}@f$ whose values are cached in a
 * @c SourceFuncCache.
 */
#pragma once

#include <functional>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief @c SourceFunc that looks every point up in a @c SourceFuncCache and evaluates the
 * wrapped callable only for points that are not stored yet.
 *
 * Usable wherever a @c SourceFunc is expected (all operators): it is a @c SourceFunc whose
 * stored callable consults the cache, with the cache attached as end-of-operator hook. It adds no
 * data and no virtual functions, so copies, also as plain @c SourceFunc, use the same cache.
 * Results are bitwise those of the uncached function (the stored values are the values the
 * callable returned for the same bit patterns).
 *
 * @code
 * auto cache = std::make_shared<SourceFuncCache>(CacheConfig{.memoryLimit = size_t{1} << 28});
 * const CachedSourceFunc f(expensiveFunc, cache);
 * const double integral = quadrature(f, sparseGrid);           // evaluates every point once
 * const double value = interpolate(f, point, sparseGrid);      // all hits
 * const size_t evaluations = f.statistics().lookups - f.statistics().hits;
 * @endcode
 *
 * @note @c SourceFunc has no virtual destructor: do not delete a cached function through a
 * pointer to @c SourceFunc.
 *
 * See @c SourceFuncCache for the rules (one function per cache, NaN values, memory limit).
 */
class CachedSourceFunc final : public SourceFunc {
 public:
  /**
   * @brief Wraps @p func, caching its values in @p cache (shareable with other cached functions
   * of the same function, grids and calls).
   * @param func  Callable mapping a point in problem coordinates to a scalar.
   * @param cache Cache (not null; holds the values of this function only).
   * @throws base::not_implemented_exception if the cache is not available
   * (@c SourceFuncCache::isAvailable()).
   * @throws base::data_exception if @p cache is null or holds the values of a vector-valued
   * function.
   */
  CachedSourceFunc(std::function<double(const base::DataVector&)> func,
                   const std::shared_ptr<SourceFuncCache>& cache);

  /**
   * @brief Wraps @p func, caching its values in a new cache (see @ref cache).
   * @param func   Callable mapping a point in problem coordinates to a scalar.
   * @param config Configuration of the new cache.
   * @throws base::not_implemented_exception if the cache is not available
   * (@c SourceFuncCache::isAvailable()).
   */
  explicit CachedSourceFunc(std::function<double(const base::DataVector&)> func,
                            const CacheConfig& config = CacheConfig());

  /**
   * @brief Returns the cache.
   * @return Cache shared with all copies.
   */
  std::shared_ptr<SourceFuncCache> cache() const;

  /**
   * @brief Returns the statistics of the cache (@c cache()->statistics()).
   * @return Statistics.
   */
  CacheStatistics statistics() const;

 private:
  /**
   * @brief Returns the callable that looks points up in @p cache before calling @p func.
   * @throws base::not_implemented_exception if the cache is not available
   * (@c SourceFuncCache::isAvailable()).
   * @throws base::data_exception if @p cache is null or holds another number of outputs.
   */
  static std::function<double(const base::DataVector&)> wrap(
      std::function<double(const base::DataVector&)> func,
      const std::shared_ptr<SourceFuncCache>& cache);
};

}  // namespace combigrid
}  // namespace sgpp
