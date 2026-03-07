#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {
namespace tools {

constexpr uint64_t FNV_OFFSET = 0xcbf29ce484222325ULL;
constexpr uint64_t FNV_PRIME = 0x00000100000001B3ULL;

namespace {

// Recursive C++11 constexpr version
constexpr uint64_t fnv1aHashImpl(const char* str, uint64_t hash) noexcept {
  return (*str == 0)
             ? hash
             : fnv1aHashImpl(str + 1, (hash ^ static_cast<unsigned char>(*str)) * FNV_PRIME);
}

}  // namespace

constexpr uint64_t fnv1aHash(const char* str) noexcept { return fnv1aHashImpl(str, FNV_OFFSET); }

namespace {

constexpr uint64_t fnv1aHashCombineImpl(const uint64_t hash, const uint64_t value,
                                        const int byte) noexcept {
  return (byte == 8) ? hash
                     : fnv1aHashCombineImpl(
                           (hash ^ static_cast<uint8_t>((value >> (byte * 8)) & 0xFF)) * FNV_PRIME,
                           value, byte + 1);
}

}  // namespace

constexpr uint64_t fnv1aHashCombine(const uint64_t hash1, const uint64_t hash2) noexcept {
  return fnv1aHashCombineImpl(hash1, hash2, 0);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp