// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file operator_data_cache.hpp
 * @brief Defines @c OperatorDataCache, which keeps the query-independent data that operators
 * build lazily for a grid (one object per type).
 */
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Thread-safe store of lazily built, query-independent operator data of a grid, at most one
 * object per type (e.g. the index of the linear interpolation on a deltas sparse grid).
 *
 * The grid holds the cache without knowing the types stored in it, so operators can attach data
 * to a grid without changing the grid's class. The first @ref get of a type builds the object
 * (under the cache's lock, so concurrent first calls build it once); later calls return it. If
 * the build throws, nothing is stored and the next call builds again. The objects live as long as
 * the cache and are never modified after their construction.
 *
 * Every @ref get takes the lock and scans the few stored types: tens of nanoseconds, meant for
 * one lookup per operator call, not per grid point.
 */
class OperatorDataCache {
 public:
  OperatorDataCache() = default;
  OperatorDataCache(const OperatorDataCache&) = delete;
  OperatorDataCache& operator=(const OperatorDataCache&) = delete;

  /**
   * @brief Returns the object of type @p T, built by @p build on the first call for @p T.
   *
   * @tparam T     Type of the object (the key).
   * @tparam Build Callable without arguments returning a @c std::unique_ptr<T>.
   * @param build  Builds the object; called at most once per successful build.
   * @return The object (valid as long as the cache).
   */
  template <typename T, typename Build>
  const T& get(Build&& build) {
    const void* const object = getOrBuild(typeid(T), [&]() -> std::shared_ptr<const void> {
      return std::shared_ptr<const T>(std::forward<Build>(build)());
    });
    return *static_cast<const T*>(object);
  }

  /// @brief Returns the number of stored objects.
  size_t size() const;

 private:
  /// @brief Returns the object stored for @p type; stores the result of @p build first if there is
  /// none (under the lock).
  const void* getOrBuild(std::type_index type,
                         const std::function<std::shared_ptr<const void>()>& build);

  mutable std::mutex mutex;  ///< Guards @ref entries.
  /// Stored objects with their types (type-erased; the shared_ptr keeps the right deleter).
  std::vector<std::pair<std::type_index, std::shared_ptr<const void>>> entries;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
