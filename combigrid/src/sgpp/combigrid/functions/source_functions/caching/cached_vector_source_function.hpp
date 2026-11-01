// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cached_vector_source_function.hpp
 * @brief Vector-valued source function @f$f: \mathbb{R}^d \to \mathbb{R}^m@f$ whose values are
 * cached in a @c SourceFuncCache.
 */
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief Vector-valued counterpart of @c CachedSourceFunc: a @c VectorSourceFunc that looks
 * every point up in a @c SourceFuncCache (one lookup for all m outputs) and evaluates the wrapped
 * callable only for points that are not stored yet.
 *
 * The cache holds the m values per point; it serves this function only (a cache of a function
 * with another number of outputs is rejected). See @c CachedSourceFunc and @c SourceFuncCache.
 *
 * @note Pass it to the operators by reference (as they take it). Copying or moving it into a
 * plain @c VectorSourceFunc does not compile (it would lose @c finishOperatorCall, see
 * @c VectorSourceFunc's deleted converting constructor); copies as @c CachedVectorSourceFunc share the cache.
 */
class CachedVectorSourceFunc final : public VectorSourceFunc {
 public:
  /// @name Constructors
  /// @{

  /**
   * @brief Wraps @p func, caching its values in @p cache.
   * @param func     Callable writing the @p nOutputs values at a point in problem coordinates to
   *                 its second argument.
   * @param nOutputs Number of outputs m.
   * @param cache    Cache (not null; holds the values of this function only).
   * @throws base::dependency_exception if SG++ was built without the libraries of the cache.
   * @throws std::invalid_argument if @p nOutputs is 0, @p cache is null or holds the values of a
   * function with another number of outputs.
   */
  CachedVectorSourceFunc(std::function<void(const base::DataVector&, std::span<double>)> func,
                         size_t nOutputs, const std::shared_ptr<SourceFuncCache>& cache);

  /**
   * @brief Wraps @p func, caching its values in a new cache (see @ref cache).
   * @param func     Callable writing the @p nOutputs values at a point in problem coordinates to
   *                 its second argument.
   * @param nOutputs Number of outputs m.
   * @param config   Configuration of the new cache.
   * @throws base::dependency_exception if SG++ was built without the libraries of the cache.
   * @throws std::invalid_argument if @p nOutputs is 0.
   */
  CachedVectorSourceFunc(std::function<void(const base::DataVector&, std::span<double>)> func,
                         size_t nOutputs,
                         const SourceFuncCacheConfig& config = SourceFuncCacheConfig());

  /// @}

  /// @name Bookkeeping
  /// @{

  /**
   * @brief End-of-operator hook: notifies the cache (@c SourceFuncCache::finishOperatorCall).
   */
  void finishOperatorCall() const override;

  /// @}

  /// @name Getters
  /// @{

  /**
   * @brief Returns the cache.
   * @return Cache shared with all copies.
   */
  std::shared_ptr<SourceFuncCache> cache() const;

  /// @}

  /// @name Helpers
  /// @{

  /**
   * @brief Returns the statistics of the cache (@c cache()->statistics()).
   * @return Statistics.
   */
  CacheStatistics statistics() const;

  /// @}

 private:
  /// @name Attributes
  /// @{

  std::shared_ptr<SourceFuncCache> cache_;  ///< The cache (shared with all copies).

  /// @}

  /// @name Helpers
  /// @{

  /**
   * @brief Returns the callable that looks points up in @p cache before calling @p func.
   * @throws base::dependency_exception if SG++ was built without the libraries of the cache.
   * @throws std::invalid_argument if @p nOutputs is 0, @p cache is null or holds another number
   * of outputs.
   */
  static std::function<void(const base::DataVector&, std::span<double>)> wrap(
      std::function<void(const base::DataVector&, std::span<double>)> func, size_t nOutputs,
      const std::shared_ptr<SourceFuncCache>& cache);

  /// @}
};

}  // namespace combigrid
}  // namespace sgpp
