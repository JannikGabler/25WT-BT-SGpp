#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

/// Value of the key @c HEADER_KEY in the sub-database @c meta.
struct FileHeader {
  char magic[8];
  uint32_t version;
  uint32_t keyScheme;
  uint64_t nDim;
  uint64_t nOutputs;
};

}  // namespace lmdb_point_value_store
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp