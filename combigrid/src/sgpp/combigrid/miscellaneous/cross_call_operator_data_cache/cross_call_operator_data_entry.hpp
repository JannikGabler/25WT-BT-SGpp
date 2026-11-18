// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cross_call_operator_data_entry.hpp
 * @brief Defines @c CrossCallOperatorDataEntry, one entry of a @c CrossCallOperatorDataCache.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_descriptor.hpp>
#include <string>
#include <typeindex>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief One entry of a @c CrossCallOperatorDataCache: the key and description of the data, the
 * data once built, and the lock that serializes its builds.
 *
 * Created by the first request of its key; the data is built afterwards, outside the cache's
 * lock. @ref object is read and written under the cache's lock only.
 */
struct CrossCallOperatorDataEntry {
  /// Signature of the function that returns the memory of the type-erased data in bytes.
  using MemoryBytesFunc = size_t (*)(const void*);

  /**
   * @brief Creates the entry of (@p type, @p descriptor.key) without data.
   * @param type        Type of the data.
   * @param descriptor  Key and description (copied).
   * @param memoryBytes Returns the memory of the data.
   */
  CrossCallOperatorDataEntry(const std::type_index type,
                             const CrossCallOperatorDataDescriptor& descriptor,
                             const MemoryBytesFunc memoryBytes)
      : type(type),
        key(descriptor.key),
        operatorName(descriptor.operatorName),
        description(descriptor.description),
        memoryBytes(memoryBytes) {}

  const std::type_index type;          ///< Type of the data (part of the key).
  const std::string key;               ///< Reuse conditions beyond the grid (part of the key).
  const std::string operatorName;      ///< See @c CrossCallOperatorDataDescriptor.
  const std::string description;       ///< See @c CrossCallOperatorDataDescriptor.
  const MemoryBytesFunc memoryBytes;   ///< Memory of @ref object in bytes.
  std::mutex buildMutex;               ///< Held while the data of this entry is built.
  std::shared_ptr<const void> object;  ///< The data (type-erased), empty until built.
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
