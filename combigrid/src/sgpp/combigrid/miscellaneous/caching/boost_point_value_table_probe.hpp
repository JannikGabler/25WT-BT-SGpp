#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace boost_point_value_table {

/// @brief Key of the heterogeneous lookups in the index: a point and its hash.
struct Probe {
  const double* point;
  uint64_t hash;
};

}  // namespace boost_point_value_table
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
