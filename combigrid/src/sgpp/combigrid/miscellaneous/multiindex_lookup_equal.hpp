#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_EQUAL_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_EQUAL_HPP

#include <cstddef>

namespace sgpp {

namespace combigrid {

namespace misc {

template <typename T>
struct MIVecLookupEqual {
  MIVecLookupEqual<T>(const size_t nDim) : nDim(nDim) {}

  bool operator()(const T* a, const T* b) const {
    for (size_t dim = 0; dim < nDim; dim++) {
      if (a[dim] != b[dim]) {
        return false;
      }
    }

    return true;
  }

 private:
  const size_t nDim;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif