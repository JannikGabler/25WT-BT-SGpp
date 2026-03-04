#pragma once

#include <cstddef>
#include <cstring>
#include <sgpp/base/datatypes/DataVector.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

struct DataVectorHash {
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