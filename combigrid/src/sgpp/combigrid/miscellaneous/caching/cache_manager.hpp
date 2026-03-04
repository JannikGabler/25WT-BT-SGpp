// TODO: Delete

// #pragma once

// #include <memory>
// #include <mutex>
// #include <sgpp/combigrid/miscellaneous/caching/base_cache.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/in_memory_cache.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/persistent_cache.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/serializer.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
// #include <string>
// #include <type_traits>
// #include <unordered_map>
// #include "sgpp/combigrid/tools/caching/default_serializer.hpp"

// namespace sgpp {
// namespace combigrid {
// namespace misc {

// /*
// Static CacheManager class
// */
// class CacheManager {
//  public:
//   // create temporary cache (in-memory). Returns shared_ptr to cache, or nullptr if id used with
//   // other types.
//   template <typename K, typename V>
//   static std::shared_ptr<InMemoryCache<K, V>> createInMemoryCache(const CacheId id) {
//     std::lock_guard<std::mutex> g(mutex_);
//     auto it = map_.find(id);
//     if (it != map_.end()) {
//       // type check
//       if (it->second->key_type() != typeid(K) || it->second->value_type() != typeid(V)) {
//         return nullptr;
//       }
//       return std::static_pointer_cast<InMemoryCache<K, V>>(it->second);
//     }
//     auto c = std::make_shared<InMemoryCache<K, V>>();
//     map_.emplace(id, c);
//     return c;
//   }

//   // create persistent lookup cache. Requires serializers/deserializers for K and V.
//   template <typename K, typename V>
//   static std::shared_ptr<PersistentLookupCache<K, V>> createPersistentCache(
//       const CacheId id, CacheSerializer<K> key_ser, CacheDeserializer<K> key_des,
//       CacheSerializer<V> val_ser, CacheDeserializer<V> val_des,
//       const std::string& base_dir = ".cache", const std::string& cache_name_override = "") {
//     std::string cache_name =
//         cache_name_override.empty() ? ("cache_" + std::to_string(id)) : cache_name_override;
//     std::lock_guard<std::mutex> g(mutex_);
//     auto it = map_.find(id);
//     if (it != map_.end()) {
//       if (it->second->key_type() != typeid(K) || it->second->value_type() != typeid(V)) {
//         return nullptr;
//       }
//       return std::static_pointer_cast<PersistentLookupCache<K, V>>(it->second);
//     }
//     auto c = std::make_shared<PersistentLookupCache<K, V>>(cache_name, key_ser, key_des, val_ser,
//                                                            val_des, base_dir);
//     map_.emplace(id, c);
//     return c;
//   }

//   // K is trivially copyable -> Default serializer & deserializier
//   template <typename K, typename V>
//   static typename std::enable_if<std::is_trivially_copyable<K>::value,
//                                  std::shared_ptr<PersistentLookupCache<K, V>>>::type
//   createPersistentCache(const int id, CacheSerializer<V> val_ser, CacheDeserializer<V> val_des,
//                         const std::string& base_dir = ".cache",
//                         const std::string& cache_name_override = "") {
//     return create_persistent_cache<K, V>(id, tools::caching::getDefaultCacheSerializer<K>(),
//                                          tools::caching::getDefaultCacheDeserializer<K>(),
//                                          val_ser, val_des, base_dir, cache_name_override);
//   }

//   // V is trivially copyable -> Default serializer & deserializier
//   template <typename K, typename V>
//   static typename std::enable_if<std::is_trivially_copyable<V>::value,
//                                  std::shared_ptr<PersistentLookupCache<K, V>>>::type
//   createPersistentCache(const int id, CacheSerializer<K> key_ser, CacheDeserializer<K> key_des,
//                         const std::string& base_dir = ".cache",
//                         const std::string& cache_name_override = "") {
//     return create_persistent_cache<K, V>(
//         id, key_ser, key_des, tools::caching::getDefaultCacheSerializer<V>(),
//         tools::caching::getDefaultCacheDeserializer<V>(), base_dir, cache_name_override);
//   }

//   // K and V are trivially copyable -> Default serializer & deserializier
//   template <typename K, typename V>
//   static typename std::enable_if<std::is_trivially_copyable<K>::value &&
//                                      std::is_trivially_copyable<V>::value,
//                                  std::shared_ptr<PersistentLookupCache<K, V>>>::type
//   createPersistentCache(const int id, const std::string& base_dir = ".cache",
//                         const std::string& cache_name_override = "") {
//     return create_persistent_cache<K, V>(id, tools::caching::getDefaultCacheSerializer<K>(),
//                                          tools::caching::getDefaultCacheDeserializer<K>(),
//                                          tools::caching::getDefaultCacheSerializer<V>(),
//                                          tools::caching::getDefaultCacheDeserializer<V>(),
//                                          base_dir, cache_name_override);
//   }

//   // get cache by id and types K,V; returns nullptr if not found or type mismatch
//   template <typename K, typename V>
//   static std::shared_ptr<BaseCache> getCacheBase(const CacheId id) {
//     std::lock_guard<std::mutex> g(mutex_);
//     auto it = map_.find(id);
//     if (it == map_.end()) return nullptr;
//     if (it->second->key_type() != typeid(K) || it->second->value_type() != typeid(V))
//       return nullptr;
//     return it->second;
//   }

//  private:
//   CacheManager() = delete;
//   ~CacheManager() = delete;

//   static std::mutex mutex_;
//   static std::unordered_map<CacheId, std::shared_ptr<BaseCache>> map_;
// };

// }  // namespace misc
// }  // namespace combigrid
// }  // namespace sgpp
