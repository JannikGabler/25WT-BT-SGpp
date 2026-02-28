#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HASH_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HASH_HPP

#include <cstddef>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
struct MIVecLookupHash {
 public:
  MIVecLookupHash<T>(const size_t nDim) : nDim(nDim) {}

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
  const size_t nDim;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif