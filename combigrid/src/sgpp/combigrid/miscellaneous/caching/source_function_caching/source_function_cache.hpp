/**
 * @file source_function_cache.hpp
 * @brief Sharded thread-safe cache for source-function evaluations.
 */
#pragma once

#include <cstring>
#include <mutex>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/hashing/data_vector_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <unordered_map>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Concurrent hash-table cache mapping evaluation points to their
 * source-function values.
 *
 * The cache is sharded into independent buckets to reduce contention
 * under concurrent reads/writes from OpenMP-parallel operators. Each
 * shard is guarded by its own mutex; a hash of the query point selects
 * the shard.
 */
class SourceFunctionCache {
 public:
  /***********
  Constructors
  ***********/
  /**
   * @brief Constructs the cache.
   * @param nShards         Number of independent shards (more shards
   *                        reduce contention at the cost of memory).
   * @param perShardReserve Initial bucket-count reservation per shard
   *                        (@c 0 leaves it to the @c std::unordered_map default).
   */
  explicit SourceFunctionCache(size_t nShards = 256, size_t perShardReserve = 0);

  // For now not copyable, but movable
  /// @brief Move-construct.
  SourceFunctionCache(SourceFunctionCache&&) noexcept = default;  // TODO
  /// @brief Move-assign.
  SourceFunctionCache& operator=(SourceFunctionCache&&) noexcept = default;

  SourceFunctionCache(const SourceFunctionCache&) = delete;
  SourceFunctionCache& operator=(const SourceFunctionCache&) = delete;

  /******
  Getters
  ******/
  /**
   * @brief Looks up @p point in the cache.
   * @param point Query point.
   * @param f_out Out parameter for the cached function value.
   * @return @c true iff a cached entry was found.
   */
  bool find(const base::DataVector& point, double& f_out) const;

  /******
  Setters
  ******/

  /**
   * @brief Inserts (or overwrites) the entry for @p point.
   * @param point Query point (copied).
   * @param value Function value to store.
   */
  void insert(const base::DataVector& point, double value);

  /**
   * @brief Inserts (or overwrites) the entry for @p point.
   * @param point Query point (moved-from).
   * @param value Function value to store.
   */
  void insert(base::DataVector&& point, double value);

  /**
   * @brief Removes the entry for @p point if it exists.
   * @param point Query point.
   * @return @c true iff an entry was removed.
   */
  bool erase(const base::DataVector& point);

  /*****************
  Further operations
  *****************/

  /**
   * @brief Returns the total number of cached entries.
   * @note Sums over all shards; runtime is @c O(\#shards).
   */
  size_t size() const;

  /// @brief Number of shards configured at construction time.
  size_t shardCnt() const noexcept;

  /**
   * @brief Returns a snapshot of every shard's hash map.
   *
   * Mainly intended for debugging and serialization.
   */
  std::vector<std::unordered_map<base::DataVector, double, DataVectorHash>> dump() const;

 private:
  /**
   * @brief One independently-locked shard of the cache.
   */
  struct Shard {
    mutable std::mutex mutex;  ///< Guards @c map.
    std::unordered_map<base::DataVector, double, DataVectorHash> map;  ///< Cached entries.
  };

  std::vector<Shard> shards_;  ///< All shards.
  DataVectorHash hasher_;      ///< Stateless hash functor reused across shards.

  /**
   * @brief Returns the shard index for @p v.
   * @param v Query point.
   */
  size_t shardIndex(const base::DataVector& v) const noexcept;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
