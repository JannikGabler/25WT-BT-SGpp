/**
 * @file persistent_cache.hpp
 * @brief Reserved for a Linux-only persistent (mmap-backed) lookup cache.
 *
 * @note Currently disabled — the implementation below is kept commented
 * out as a future extension point.
 */

// #pragma once

// #include <fcntl.h>
// #include <sys/mman.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <atomic>
// #include <cstddef>
// #include <cstdint>
// #include <fstream>
// #include <functional>
// #include <iostream>
// #include <memory>
// #include <mutex>
// #include <sgpp/combigrid/miscellaneous/caching/base_cache.hpp>
// #include <sgpp/combigrid/miscellaneous/caching/serializer.hpp>
// #include <sgpp/combigrid/tools/caching/file_system.hpp>
// #include <sgpp/combigrid/tools/caching/memory_stream_buffer.hpp>
// #include <sgpp/combigrid/tools/caching/split_mix.hpp>
// #include <sgpp/combigrid/tools/os/os.hpp>
// #include <sstream>
// #include <string>
// #include <typeindex>
// #include <unordered_map>
// #include <vector>

// namespace sgpp {
// namespace combigrid {
// namespace misc {

// template <typename K, typename V>
// class PersistentLookupCache : public BaseCache,
//                               public std::enable_shared_from_this<PersistentLookupCache<K, V>> {
//  public:
//   PersistentLookupCache(const std::string& cache_name, CacheSerializer<K> key_ser,
//                         CacheDeserializer<K> key_des, CacheSerializer<V> val_ser,
//                         CacheDeserializer<V> val_des, const std::string& base_dir = ".cache",
//                         size_t bloom_bits_per_key = 10, size_t bloom_hashes = 2)
//       : cache_name_(cache_name),
//         key_serializer_(key_ser),
//         key_deserializer_(key_des),
//         val_serializer_(val_ser),
//         val_deserializer_(val_des),
//         base_dir_(base_dir),
//         index_loaded_(false),
//         bloom_bits_per_key_(bloom_bits_per_key),
//         bloom_hashes_(bloom_hashes) {
//     index_path_ = base_dir_ + "/" + cache_name_ + "/index.dat";
//     dir_path_ = base_dir_ + "/" + cache_name_;
//   }

//   virtual ~PersistentLookupCache() {}

//   std::type_index key_type() const override { return typeid(K); }
//   std::type_index value_type() const override { return typeid(V); }

//   // add_and_persist bleibt unverändert (Precompute-Phase). Keine Concurrent-Reads/Writes in
//   // Precompute angenommen.
//   bool add_and_persist(const K& k, const V& v) {
//     if (!tools::isLinux()) return false;

//     std::lock_guard<std::mutex> g(write_mutex_);

//     if (!tools::caching::ensureDirExists(base_dir_)) {
//       tools::caching::ensureDirExists(".");
//     }
//     if (!tools::caching::ensureDirExists(dir_path_)) {
//       if (!tools::caching::ensureDirExists(dir_path_)) return false;
//     }

//     std::ostringstream fn;
//     fn << std::hex << std::hash<K>()(k) << "_" << std::dec << (write_counter_++);
//     std::string filename = fn.str();
//     std::string filepath = dir_path_ + "/" + filename;

//     std::ofstream ofs(filepath.c_str(), std::ios::binary);
//     if (!ofs) return false;
//     val_serializer_(v, ofs);
//     ofs.close();

//     std::ofstream idx(index_path_.c_str(), std::ios::binary | std::ios::app);
//     if (!idx) return false;
//     std::ostringstream keybuf;
//     key_serializer_(k, keybuf);
//     std::string kbytes = keybuf.str();
//     uint64_t klen = static_cast<uint64_t>(kbytes.size());
//     uint64_t flen = static_cast<uint64_t>(filename.size());

//     idx.write(reinterpret_cast<char*>(&klen), sizeof(klen));
//     idx.write(kbytes.data(), klen);
//     idx.write(reinterpret_cast<char*>(&flen), sizeof(flen));
//     idx.write(filename.data(), flen);
//     idx.flush();
//     idx.close();

//     // Wenn Index bereits geladen, aktualisiere die In-Memory-Struktur atomar (kopiere,
//     erweitere,
//     // store)
//     std::shared_ptr<const std::unordered_map<K, std::string>> oldmap =
//         std::atomic_load(&index_ptr_);
//     if (oldmap) {
//       auto newmap = std::make_shared<std::unordered_map<K, std::string>>(*oldmap);
//       (*newmap)[k] = filename;
//       // update bloom (unsafe parallel fallback: wir rebuilden Bloom komplett bei request; hier
//       // vereinfachter append)
//       std::atomic_store(&index_ptr_,
//                         std::static_pointer_cast<const std::unordered_map<K,
//                         std::string>>(newmap));
//     }
//     return true;
//   }

//   // Lock-free, sehr schnelle Abfrage
//   bool get(const K& k, V& out) {
//     if (!tools::isLinux()) return false;
//     load_index_once();

//     // 1) Bloom-Filter schnell checken
//     if (bloom_bits_.size()) {
//       if (!bloom_maybe_contains(k)) return false;  // sicher nicht vorhanden
//     }

//     // 2) atomisch Index holen (shared_ptr copy) -> lock-free read
//     auto idxptr = std::atomic_load(&index_ptr_);
//     if (!idxptr) return false;
//     auto it = idxptr->find(k);
//     if (it == idxptr->end()) return false;

//     std::string filepath = dir_path_ + "/" + it->second;

//     // 3) mmap Datei, bauen MemoryStreamBuf, deserialisieren ohne Copy
//     void* map = nullptr;
//     size_t mapsz = 0;
//     if (!mmap_file(filepath, map, mapsz)) return false;
//     tools::MemoryStreamBuf buf(reinterpret_cast<const char*>(map), mapsz);
//     std::istream is(&buf);
//     bool ok = val_deserializer_(out, is);
//     munmap(map, mapsz);
//     return ok;
//   }

//   bool contains(const K& k) {
//     if (!tools::isLinux()) return false;
//     load_index_once();
//     if (bloom_bits_.size()) {
//       if (!bloom_maybe_contains(k)) return false;
//     }
//     auto idxptr = std::atomic_load(&index_ptr_);
//     if (!idxptr) return false;
//     return idxptr->find(k) != idxptr->end();
//   }

//   void reload_index() {
//     // Force reload (atomically replace index_ptr_)
//     std::lock_guard<std::mutex> g(index_build_mutex_);
//     auto map = build_index_from_disk();
//     std::atomic_store(&index_ptr_, map);
//     // rebuild bloom
//     build_bloom_from_map(map);
//     index_loaded_ = true;
//   }

//   void load_index_once() {
//     std::call_once(init_flag_, [this]() {
//       std::lock_guard<std::mutex> g(index_build_mutex_);
//       auto map = build_index_from_disk();
//       std::atomic_store(&index_ptr_, map);
//       build_bloom_from_map(map);
//       index_loaded_ = true;
//     });
//   }

//  private:
//   // --- members ---
//   std::string cache_name_;
//   std::string base_dir_;
//   std::string dir_path_;
//   std::string index_path_;

//   CacheSerializer<K> key_serializer_;
//   CacheDeserializer<K> key_deserializer_;
//   CacheSerializer<V> val_serializer_;
//   CacheDeserializer<V> val_deserializer_;

//   std::once_flag init_flag_;
//   std::mutex index_build_mutex_;  // nur für building/reload
//   std::mutex write_mutex_;
//   std::atomic<bool> index_loaded_;
//   uint64_t write_counter_ = 0;

//   // shared_ptr auf unveränderliche Map => Lookups lock-free
//   std::shared_ptr<const std::unordered_map<K, std::string>> index_ptr_;

//   // Bloom-Filter intern:
//   size_t bloom_bits_per_key_;
//   size_t bloom_hashes_;
//   std::vector<uint64_t> bloom_bits_;  // bitset (64-bit chunks)

//   // --- Hilfsfunktionen ---

//   std::shared_ptr<const std::unordered_map<K, std::string>> build_index_from_disk() {
//     std::unordered_map<K, std::string> tmp;
//     if (!tools::caching::ensureDirExists(dir_path_)) {
//       return std::make_shared<const std::unordered_map<K, std::string>>(tmp);
//     }
//     std::ifstream idx(index_path_.c_str(), std::ios::binary);
//     if (!idx) return std::make_shared<const std::unordered_map<K, std::string>>(tmp);
//     while (idx.peek() != EOF) {
//       uint64_t klen = 0;
//       idx.read(reinterpret_cast<char*>(&klen), sizeof(klen));
//       if (!idx) break;
//       std::string kbytes;
//       kbytes.resize(klen);
//       idx.read(&kbytes[0], klen);
//       uint64_t flen = 0;
//       idx.read(reinterpret_cast<char*>(&flen), sizeof(flen));
//       std::string filename;
//       filename.resize(flen);
//       idx.read(&filename[0], flen);
//       std::istringstream ksin(kbytes);
//       K key;
//       bool ok = key_deserializer_(key, ksin);
//       if (ok) tmp.emplace(std::move(key), std::move(filename));
//     }
//     return std::make_shared<const std::unordered_map<K, std::string>>(std::move(tmp));
//   }

//   void build_bloom_from_map(const std::shared_ptr<const std::unordered_map<K, std::string>>& map)
//   {
//     if (!map) return;
//     size_t n = map->size();
//     if (n == 0) {
//       bloom_bits_.clear();
//       return;
//     }
//     size_t m = std::max<size_t>(16, n * bloom_bits_per_key_);  // minimal 16 bits
//     size_t words = (m + 63) / 64;
//     bloom_bits_.assign(words, 0ULL);
//     bloom_m_ = m;
//     for (const auto& p : *map) {
//       add_to_bloom(p.first);
//     }
//   }

//   // Bloom hash + set
//   void add_to_bloom(const K& k) {
//     uint64_t h1 = static_cast<uint64_t>(std::hash<K>()(k));
//     uint64_t h2 = tools::splitmix64(h1);
//     for (size_t i = 0; i < bloom_hashes_; ++i) {
//       uint64_t combined = h1 + i * h2;
//       size_t bit = combined % bloom_m_;
//       bloom_bits_[bit / 64] |= (1ULL << (bit % 64));
//     }
//   }
//   bool bloom_maybe_contains(const K& k) const {
//     if (bloom_bits_.empty()) return true;
//     uint64_t h1 = static_cast<uint64_t>(std::hash<K>()(k));
//     uint64_t h2 = tools::splitmix64(h1);
//     for (size_t i = 0; i < bloom_hashes_; ++i) {
//       uint64_t combined = h1 + i * h2;
//       size_t bit = combined % bloom_m_;
//       if ((bloom_bits_[bit / 64] & (1ULL << (bit % 64))) == 0) return false;
//     }
//     return true;  // maybe present
//   }

//   // mmap helper
//   bool mmap_file(const std::string& path, void*& out_ptr, size_t& out_size) const {
//     int fd = open(path.c_str(), O_RDONLY);
//     if (fd < 0) return false;
//     struct stat st;
//     if (fstat(fd, &st) != 0) {
//       close(fd);
//       return false;
//     }
//     if (st.st_size == 0) {
//       close(fd);
//       out_ptr = nullptr;
//       out_size = 0;
//       return true;
//     }
//     void* map = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
//     close(fd);
//     if (map == MAP_FAILED) return false;
//     out_ptr = map;
//     out_size = static_cast<size_t>(st.st_size);
//     return true;
//   }

//   size_t bloom_m_ = 0;
// };

// }  // namespace misc
// }  // namespace combigrid
// }  // namespace sgpp