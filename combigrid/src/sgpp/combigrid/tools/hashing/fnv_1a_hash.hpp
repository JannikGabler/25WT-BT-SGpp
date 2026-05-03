/**
 * @file fnv_1a_hash.hpp
 * @brief @c constexpr FNV-1a hash for short C strings and 64-bit values.
 *
 * Used to derive deterministic numeric ids for quadrature rules,
 * interpolation methods, and node-generation functions from their string
 * names. Computed at compile-time so equality comparisons remain cheap.
 */
#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {
namespace tools {

/// @brief FNV-1a 64-bit offset basis.
constexpr uint64_t FNV_OFFSET = 0xcbf29ce484222325ULL;
/// @brief FNV-1a 64-bit prime.
constexpr uint64_t FNV_PRIME = 0x00000100000001B3ULL;

namespace {

/**
 * @brief Tail-recursive helper for @ref fnv1aHash.
 * @param str  Null-terminated string position.
 * @param hash Running hash value.
 * @return Final 64-bit hash.
 */
constexpr uint64_t fnv1aHashImpl(const char* str, uint64_t hash) noexcept {
  return (*str == 0)
             ? hash
             : fnv1aHashImpl(str + 1, (hash ^ static_cast<unsigned char>(*str)) * FNV_PRIME);
}

}  // namespace

/**
 * @brief Computes the FNV-1a 64-bit hash of a null-terminated C string.
 * @param str Null-terminated input string.
 * @return 64-bit hash value.
 */
constexpr uint64_t fnv1aHash(const char* str) noexcept { return fnv1aHashImpl(str, FNV_OFFSET); }

namespace {

/**
 * @brief Tail-recursive byte-wise mixing of @p value into @p hash.
 * @param hash  Current hash.
 * @param value Value being mixed in.
 * @param byte  Index of the byte currently being consumed.
 */
constexpr uint64_t fnv1aHashCombineImpl(const uint64_t hash, const uint64_t value,
                                        const int byte) noexcept {
  return (byte == 8) ? hash
                     : fnv1aHashCombineImpl(
                           (hash ^ static_cast<uint8_t>((value >> (byte * 8)) & 0xFF)) * FNV_PRIME,
                           value, byte + 1);
}

}  // namespace

/**
 * @brief Combines two 64-bit hashes using FNV-1a mixing.
 *
 * Useful to build a compound hash (e.g. of a (rule, count) pair) without
 * having to allocate or stringify.
 *
 * @param hash1 First hash (used as starting state).
 * @param hash2 Second hash mixed in byte by byte.
 * @return Combined hash.
 */
constexpr uint64_t fnv1aHashCombine(const uint64_t hash1, const uint64_t hash2) noexcept {
  return fnv1aHashCombineImpl(hash1, hash2, 0);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
