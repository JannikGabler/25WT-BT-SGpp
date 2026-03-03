#pragma once

#include <memory>
#include <mutex>
#include <sgpp/combigrid/miscellaneous/caching/base_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/cache_manager.hpp>
#include <sgpp/combigrid/miscellaneous/caching/in_memory_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/persistent_cache.hpp>
#include <sgpp/combigrid/miscellaneous/caching/serializer.hpp>
#include <string>
#include <unordered_map>

namespace sgpp {
namespace combigrid {
namespace misc {

// create temporary cache (in-memory). Returns shared_ptr to cache, or nullptr if id used with
// other types.
template <typename K, typename V>
std::shared_ptr<InMemoryCache<K, V>> CacheManager::create_inmemory_cache(const int id) {
  std::lock_guard<std::mutex> g(mutex_);
  auto it = map_.find(id);
  if (it != map_.end()) {
    // type check
    if (it->second->key_type() != typeid(K) || it->second->value_type() != typeid(V)) {
      return nullptr;
    }
    return std::static_pointer_cast<InMemoryCache<K, V>>(it->second);
  }
  auto c = std::make_shared<InMemoryCache<K, V>>();
  map_.emplace(id, c);
  return c;
}

// create persistent lookup cache. Requires serializers/deserializers for K and V.
template <typename K, typename V>
std::shared_ptr<PersistentLookupCache<K, V>> CacheManager::create_persistent_cache(
    const int id, CacheSerializer<K> key_ser, CacheDeserializer<K> key_des,
    CacheSerializer<V> val_ser, CacheDeserializer<V> val_des, const std::string& base_dir,
    const std::string& cache_name_override) {
  std::string cache_name =
      cache_name_override.empty() ? ("cache_" + std::to_string(id)) : cache_name_override;
  std::lock_guard<std::mutex> g(mutex_);
  auto it = map_.find(id);
  if (it != map_.end()) {
    if (it->second->key_type() != typeid(K) || it->second->value_type() != typeid(V)) {
      return nullptr;
    }
    return std::static_pointer_cast<PersistentLookupCache<K, V>>(it->second);
  }
  auto c = std::make_shared<PersistentLookupCache<K, V>>(cache_name, key_ser, key_des, val_ser,
                                                         val_des, base_dir);
  map_.emplace(id, c);
  return c;
}

// get cache by id and types K,V; returns nullptr if not found or type mismatch
template <typename K, typename V>
std::shared_ptr<BaseCache> CacheManager::get_cache_base(int id) {
  std::lock_guard<std::mutex> g(mutex_);
  auto it = map_.find(id);
  if (it == map_.end()) return nullptr;
  if (it->second->key_type() != typeid(K) || it->second->value_type() != typeid(V)) return nullptr;
  return it->second;
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp