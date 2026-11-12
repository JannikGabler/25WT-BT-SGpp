// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/miscellaneous/operator_data_cache/operator_data_cache.hpp>
#include <typeindex>
#include <utility>

namespace sgpp {
namespace combigrid {
namespace misc {

size_t OperatorDataCache::size() const {
  const std::lock_guard<std::mutex> lock(mutex);
  return entries.size();
}

const void* OperatorDataCache::getOrBuild(
    const std::type_index type, const std::function<std::shared_ptr<const void>()>& build) {
  const std::lock_guard<std::mutex> lock(mutex);

  for (const auto& entry : entries) {
    if (entry.first == type) {
      return entry.second.get();
    }
  }

  // A throwing build leaves the cache unchanged
  std::shared_ptr<const void> object = build();
  const void* const result = object.get();
  entries.emplace_back(type, std::move(object));
  return result;
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
