/**
 * @file split_mix.hpp
 * @brief SplitMix64 finalizer used as a fast 64-bit hash mixer.
 */
#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief SplitMix64 mixer (Jenkins, 2014).
 *
 * Maps a 64-bit input to a well-distributed 64-bit output. Used as a
 * lightweight finalizer when combining hash values for cache keys.
 *
 * @param x Input value.
 * @return Mixed 64-bit value.
 */
inline uint64_t splitmix64(uint64_t x) {
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
