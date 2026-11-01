// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file boost_point_value_table_record_arena.hpp
 * @brief Chunked storage of the fixed-size records of the source-function cache.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/caching/record_layout.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace boost_point_value_table {

/**
 * @brief Append-only storage of records with stable 32-bit indices, in chunks of
 * @c constants::source_func_cache::RECORDS_PER_CHUNK_LOG2 records (not synchronized; one arena
 * per shard of the cache, used under the shard's lock).
 */
class RecordArena {
 public:
  /**
   * @brief Creates an empty arena.
   * @param layout Layout of the records.
   */
  explicit RecordArena(const source_func_caching::RecordLayout layout) : layout_(layout) {}

  /// @brief Returns the layout of the records.
  const source_func_caching::RecordLayout& layout() const { return layout_; }

  /// @brief Returns the number of records.
  size_t size() const { return size_; }

  /// @brief Whether another record can be indexed with 32 bits.
  bool full() const { return size_ == std::numeric_limits<uint32_t>::max(); }

  /// @brief Returns the bytes of the allocated chunks.
  size_t allocatedBytes() const { return chunks_.size() * chunkBytes(); }

  /// @brief Returns the bytes the next @ref allocate allocates (a chunk or nothing).
  size_t bytesOfNextRecord() const { return (size_ & CHUNK_MASK) == 0 ? chunkBytes() : 0; }

  /**
   * @brief Appends an uninitialized record.
   * @return Index of the record.
   */
  uint32_t allocate() {
    if ((size_ & CHUNK_MASK) == 0) {
      chunks_.emplace_back(new uint64_t[layout_.stride() << CHUNK_LOG2]);
    }
    return static_cast<uint32_t>(size_++);
  }

  /**
   * @brief Returns the record with index @p idx.
   * @param idx Index returned by @ref allocate.
   * @return Pointer to the first word of the record.
   */
  uint64_t* record(const uint32_t idx) const {
    return chunks_[idx >> CHUNK_LOG2].get() + (idx & CHUNK_MASK) * layout_.stride();
  }

 private:
  static constexpr size_t CHUNK_LOG2 = constants::source_func_cache::RECORDS_PER_CHUNK_LOG2;
  static constexpr size_t CHUNK_MASK = (size_t{1} << CHUNK_LOG2) - 1;

  size_t chunkBytes() const { return layout_.bytes() << CHUNK_LOG2; }

  source_func_caching::RecordLayout layout_;         ///< Layout of the records.
  std::vector<std::unique_ptr<uint64_t[]>> chunks_;  ///< Chunks of records.
  size_t size_ = 0;                                  ///< Number of records.
};

}  // namespace boost_point_value_table
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
