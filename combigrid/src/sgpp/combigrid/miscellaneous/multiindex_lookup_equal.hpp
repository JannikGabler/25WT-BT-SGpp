/**
 * @file multiindex_lookup_equal.hpp
 * @brief Equality functor over multi-indices stored as a flat pointer.
 */
#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_EQUAL_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_EQUAL_HPP

#include <cstddef>

namespace sgpp {

namespace combigrid {

namespace misc {

/**
 * @brief Equality functor used by @c MIVecLookup.
 *
 * Compares the @c nDim consecutive entries pointed to by two raw
 * pointers; allows the lookup to use raw pointers as keys without
 * materializing each multi-index as an independent @c MI object.
 *
 * @tparam T Multi-index element type.
 */
template <typename T>
struct MIVecLookupEqual {
  /**
   * @brief Constructs the functor for multi-indices of dimension @p nDim.
   */
  MIVecLookupEqual<T>(const size_t nDim) : nDim(nDim) {}

  /**
   * @brief Returns @c true iff the multi-indices pointed to by @p a and
   * @p b are component-wise equal.
   */
  bool operator()(const T* a, const T* b) const {
    for (size_t dim = 0; dim < nDim; dim++) {
      if (a[dim] != b[dim]) {
        return false;
      }
    }

    return true;
  }

 private:
  const size_t nDim;  ///< Multi-index dimensionality.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
