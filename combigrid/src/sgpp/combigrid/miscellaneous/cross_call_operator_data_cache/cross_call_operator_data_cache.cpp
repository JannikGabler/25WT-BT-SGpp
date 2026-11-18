// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/grids/cross_call_operator_data_info.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_cache.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_descriptor.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_entry.hpp>
#include <string_view>
#include <typeindex>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

void CrossCallOperatorDataCache::clear() {
  std::vector<std::shared_ptr<CrossCallOperatorDataEntry>> removed;
  {
    const std::lock_guard<std::mutex> lock(mutex);
    removed.swap(entries_);
  }
  // The objects no operator call holds any more are deleted here, outside the lock
}

std::vector<CrossCallOperatorDataInfo> CrossCallOperatorDataCache::entries() const {
  std::vector<CrossCallOperatorDataInfo> infos;
  const std::lock_guard<std::mutex> lock(mutex);
  for (const auto& entry : entries_) {
    if (entry->object != nullptr) {
      infos.push_back({.operatorName = entry->operatorName,
                       .description = entry->description,
                       .key = entry->key,
                       .bytes = entry->memoryBytes(entry->object.get())});
    }
  }
  return infos;
}

size_t CrossCallOperatorDataCache::memoryBytes() const {
  size_t bytes = 0;
  const std::lock_guard<std::mutex> lock(mutex);
  for (const auto& entry : entries_) {
    if (entry->object != nullptr) bytes += entry->memoryBytes(entry->object.get());
  }
  return bytes;
}

size_t CrossCallOperatorDataCache::size() const {
  size_t count = 0;
  const std::lock_guard<std::mutex> lock(mutex);
  for (const auto& entry : entries_) {
    if (entry->object != nullptr) count++;
  }
  return count;
}

std::shared_ptr<const void> CrossCallOperatorDataCache::find(const std::type_index type,
                                                             const std::string_view key) const {
  const std::lock_guard<std::mutex> lock(mutex);
  for (const auto& entry : entries_) {
    if (entry->type == type && entry->key == key) return entry->object;
  }
  return nullptr;
}

std::shared_ptr<CrossCallOperatorDataEntry> CrossCallOperatorDataCache::findOrCreateEntry(
    const std::type_index type, const CrossCallOperatorDataDescriptor& descriptor,
    const CrossCallOperatorDataEntry::MemoryBytesFunc memoryBytes) {
  const std::lock_guard<std::mutex> lock(mutex);
  for (const auto& entry : entries_) {
    if (entry->type == type && entry->key == descriptor.key) return entry;
  }
  return entries_.emplace_back(
      std::make_shared<CrossCallOperatorDataEntry>(type, descriptor, memoryBytes));
}

std::shared_ptr<const void> CrossCallOperatorDataCache::buildOnce(
    const std::type_index type, const CrossCallOperatorDataDescriptor& descriptor,
    const CrossCallOperatorDataEntry::MemoryBytesFunc memoryBytes,
    const std::function<std::shared_ptr<const void>()>& build) {
  const std::shared_ptr<CrossCallOperatorDataEntry> entry =
      findOrCreateEntry(type, descriptor, memoryBytes);

  // Concurrent requests of the entry wait here for the first build and then take its object. The
  // entry may have been removed by clear() meanwhile: then its object is built (or taken) for this
  // call but not stored in the cache.
  const std::lock_guard<std::mutex> buildLock(entry->buildMutex);
  {
    const std::lock_guard<std::mutex> lock(mutex);
    if (entry->object != nullptr) return entry->object;
  }

  // A throwing build leaves the entry without object
  std::shared_ptr<const void> object = build();
  const std::lock_guard<std::mutex> lock(mutex);
  entry->object = object;
  return object;
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
