/**
 * @file std_pair_hash.hpp
 * @brief Hash functor for @c std::pair keys.
 */
#ifndef COMBIGRID_MISC_STD_PAIR_HASH_HPP
#define COMBIGRID_MISC_STD_PAIR_HASH_HPP

#include <cstddef>
#include <functional>
#include <utility>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Generic hash functor for @c std::pair.
 *
 * Combines the standard hashes of the two elements with a small
 * multiplicative mixer. Sufficient for the cache key types used by the
 * combigrid module (e.g. @c (NodeGenFunc*, GPCntType)).
 *
 * @tparam T1 First element type.
 * @tparam T2 Second element type.
 */
template <class T1, class T2>
struct PairHash {
  /**
   * @brief Computes the hash of @p p.
   * @param p Input pair.
   * @return 64-bit hash value.
   */
  std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
    std::size_t h1 = std::hash<T1>{}(p.first);
    std::size_t h2 = std::hash<T2>{}(p.second);

    return h1 * 1315423911u + h2;
  }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
