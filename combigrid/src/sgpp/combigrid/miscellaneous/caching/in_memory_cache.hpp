#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/miscellaneous/caching/base_cache.hpp>
#include <typeindex>
#include <unordered_map>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename K, typename V, size_t SHARDS = 2>
class InMemoryCache : public BaseCache,
                      public std::enable_shared_from_this<InMemoryCache<K, V, SHARDS> > {
 public:
  InMemoryCache() : _size(0) { static_assert(SHARDS >= 1, "SHARDS must be >= 1"); }
  virtual ~InMemoryCache() {}

  std::type_index key_type() const override { return typeid(K); }
  std::type_index value_type() const override { return typeid(V); }

  // put (thread-safe)
  void put(const K& k, const V& v) {
    auto& bucket = get_bucket(k);
    std::lock_guard<std::mutex> g(bucket.mutex);
    auto it = bucket.map.find(k);
    if (it == bucket.map.end()) {
      bucket.map.emplace(k, v);
      _size.fetch_add(1, std::memory_order_relaxed);
    } else {
      it->second = v;
    }
  }

  // get (thread-safe). returns true if found and writes into out.
  bool get(const K& k, V& out) const {
    auto& bucket = get_bucket(k);
    std::lock_guard<std::mutex> g(bucket.mutex);
    auto it = bucket.map.find(k);
    if (it == bucket.map.end()) return false;
    out = it->second;
    return true;
  }

  bool contains(const K& k) const {
    auto& bucket = get_bucket(k);
    std::lock_guard<std::mutex> g(bucket.mutex);
    return bucket.map.find(k) != bucket.map.end();
  }

  size_t size() const { return _size.load(std::memory_order_relaxed); }

 private:
  struct Bucket {
    mutable std::mutex mutex;
    std::unordered_map<K, V> map;
  };

  Bucket _buckets[SHARDS];
  std::atomic<size_t> _size;

  size_t shard_index(const K& k) const { return (std::hash<K>()(k)) & (SHARDS - 1); }

  // requires SHARDS to be power of two for mask; if not, fallback:
  Bucket& get_bucket(const K& k) const {
    // if SHARDS is not power-of-two this still works due to modulo-like distribution if we use %
    const size_t idx = (std::hash<K>()(k)) % SHARDS;
    return const_cast<Bucket&>(_buckets[idx]);
  }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp