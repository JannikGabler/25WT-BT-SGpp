#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {
namespace tools {

// Simple SplitMix64
inline uint64_t splitmix64(uint64_t x) {
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
