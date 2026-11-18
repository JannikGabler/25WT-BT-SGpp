// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cross_call_operator_data_cache.hpp
 * @brief Defines @c CrossCallOperatorDataCache, which keeps query-independent data that operators
 * build for a grid and reuse in later calls.
 */
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/grids/cross_call_operator_data_info.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_descriptor.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_entry.hpp>
#include <string_view>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Thread-safe store of query-independent operator data of a grid, one object per
 * (type, key) (e.g. the index of the linear interpolation on a deltas sparse grid).
 *
 * The grid holds the cache without knowing the types stored in it, so operators can attach data
 * to a grid without changing the grid's class. Operators use it only if their options ask for it
 * (@c useCrossCallOperatorDataCache).
 *
 * Guarantees under concurrency (all methods may be called concurrently):
 * - The data of a key is built at most once while it is cached: the first @ref getOrBuild of a key
 *   builds it (outside the cache's lock, so lookups and builds of other keys proceed); concurrent
 *   requests of the same key wait for that build and share its result. If the build throws,
 *   nothing is stored and the next request builds again.
 * - The data is handed out as a @c std::shared_ptr: @ref clear removes the cache's references
 *   only, so an operator call that holds the data keeps it until it returns. After @ref clear,
 *   the next request builds the data anew (also while a build of that key is still running: the
 *   data of a build that started before the clear is not stored).
 * - @ref entries and @ref memoryBytes see a consistent state (between two modifications).
 * - The data is never modified after its construction (except data that completes itself
 *   thread-safely on first use, such as the quadrature weight table).
 *
 * Every @ref getOrBuild of cached data takes the lock, scans the few entries and copies a
 * @c std::shared_ptr: tens of nanoseconds, meant for one lookup per operator call, not per grid
 * point.
 */
class CrossCallOperatorDataCache {
 public:
  CrossCallOperatorDataCache() = default;
  CrossCallOperatorDataCache(const CrossCallOperatorDataCache&) = delete;
  CrossCallOperatorDataCache& operator=(const CrossCallOperatorDataCache&) = delete;

  /**
   * @brief Returns the object of type @p T and key @p descriptor.key, built by @p build if the
   * cache holds none.
   *
   * @tparam T          Type of the object (part of the key). Its memory is reported by
   *                    <tt>T::memoryBytes()</tt> if it has one, else by @c sizeof(T).
   * @tparam Build      Callable without arguments returning a @c std::unique_ptr<T>.
   * @param descriptor  Key and description of the entry.
   * @param build       Builds the object.
   * @return The object (shared with the cache).
   */
  template <typename T, typename Build>
  std::shared_ptr<const T> getOrBuild(const CrossCallOperatorDataDescriptor& descriptor,
                                      Build&& build) {
    std::shared_ptr<const void> object = find(typeid(T), descriptor.key);
    if (object == nullptr) {
      object =
          buildOnce(typeid(T), descriptor, &memoryBytesOf<T>, [&]() -> std::shared_ptr<const void> {
            return std::shared_ptr<const T>(std::forward<Build>(build)());
          });
    }
    return std::static_pointer_cast<const T>(std::move(object));
  }

  /// @brief Removes all objects from the cache (callers that hold an object keep it).
  void clear();

  /// @brief Returns the descriptions of the stored objects, in the order of their creation.
  std::vector<CrossCallOperatorDataInfo> entries() const;

  /// @brief Returns the total memory of the stored objects in bytes.
  size_t memoryBytes() const;

  /// @brief Returns the number of stored objects.
  size_t size() const;

 private:
  /// @brief Returns the memory of the object @p object of type @p T.
  template <typename T>
  static size_t memoryBytesOf(const void* const object) {
    if constexpr (requires(const T& t) { t.memoryBytes(); }) {
      return static_cast<const T*>(object)->memoryBytes();
    } else {
      return sizeof(T);
    }
  }

  /// @brief Returns the stored object of (@p type, @p key), or an empty pointer.
  std::shared_ptr<const void> find(std::type_index type, std::string_view key) const;

  /// @brief Returns the entry of (@p type, @p key), created if there is none (under the lock).
  std::shared_ptr<CrossCallOperatorDataEntry> findOrCreateEntry(
      std::type_index type, const CrossCallOperatorDataDescriptor& descriptor,
      CrossCallOperatorDataEntry::MemoryBytesFunc memoryBytes);

  /// @brief Returns the object of (@p type, @p descriptor.key); builds it with @p build once if
  /// there is none (outside the cache's lock, under the lock of the entry) and stores it.
  std::shared_ptr<const void> buildOnce(std::type_index type,
                                        const CrossCallOperatorDataDescriptor& descriptor,
                                        CrossCallOperatorDataEntry::MemoryBytesFunc memoryBytes,
                                        const std::function<std::shared_ptr<const void>()>& build);

  mutable std::mutex mutex;  ///< Guards @ref entries_ and the objects of the entries.
  /// The entries in the order of their creation, incl. entries whose first build is running or
  /// failed (no object; not reported).
  std::vector<std::shared_ptr<CrossCallOperatorDataEntry>> entries_;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
