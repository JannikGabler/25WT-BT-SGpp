/**
 * @file multiindex_vector_lookup_hash.hpp
 * @brief Hash functor over multi-indices stored as a flat pointer.
 */
#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HASH_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HASH_HPP

#include <cstddef>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Hash functor used by @c MIVecLookup.
 *
 * Hashes the @c nDim consecutive entries pointed to by @c dataPtr using
 * an FNV-1a-style mixer combined with the golden-ratio additive constant
 * @c 0x9e3779b97f4a7c15ULL.
 *
 * @tparam T Multi-index element type.
 */
template <typename T>
struct MIVecLookupHash {
 public:
  /**
   * @brief Constructs the functor for multi-indices of dimension @p nDim.
   * @param nDim Multi-index dimensionality.
   */
  MIVecLookupHash<T>(const size_t nDim) : nDim(nDim) {}

  /**
   * @brief Hashes the multi-index pointed to by @p dataPtr.
   * @param dataPtr Pointer to @p nDim consecutive @c T values.
   */
  size_t operator()(const T* dataPtr) const {
    size_t h = 1469598103934665603ULL;  // FNV offset basis

    for (size_t dim = 0; dim < nDim; dim++) {
      const size_t t = dataPtr[dim] + 0x9e3779b97f4a7c15ULL;
      h ^= t;
      h *= 1099511628211ULL;  // FNV prime
    }

    return h;
  }

 private:
  const size_t nDim;  ///< Multi-index dimensionality.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
