/**
 * @file data_vector_hash.hpp
 * @brief Hash functor for @c base::DataVector keys.
 */
#pragma once

#include <cstddef>
#include <cstring>
#include <sgpp/base/datatypes/DataVector.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Hash functor that mixes the bit pattern of every entry of a
 * @c base::DataVector.
 *
 * Used as the default hash for @c std::unordered_map keys of type
 * @c base::DataVector (e.g. inside the source-function cache).
 */
struct DataVectorHash {
  /**
   * @brief Computes the hash of @p v.
   * @param v Input vector.
   * @return 64-bit hash value.
   */
  size_t operator()(const base::DataVector& v) const noexcept {
    std::size_t h = 1469598103934665603ULL;  // arbitrary seed (FNV-ish)

    for (double d : v) {
      uint64_t bits = 0;
      // portable bit-cast double -> uint64_t
      std::memcpy(&bits, &d, sizeof(bits));
      // mix
      h ^= static_cast<std::size_t>(bits) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    }
    return h;
  }
};

}  // namespace misc

}  // namespace combigrid

}  // namespace sgpp
