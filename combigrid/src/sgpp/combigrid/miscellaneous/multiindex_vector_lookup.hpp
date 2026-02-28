#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HPP

#include <sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup_hash.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
#include <unordered_map>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  // Forward declaration to avoid circular includes

namespace misc {

template <typename T>
class MIVecLookup {
 public:
  MIVecLookup<T>(const MIVec<T>& miVec)
      : miVec((miVec)),
        map(miVec.nMI(), MIVecLookupHash<T>(miVec.nDim()), MIVecLookupEqual<T>(miVec.nDim())) {
    const T* dataPtr = miVec.data();

    for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
      map.emplace(dataPtr, miIdx);
      dataPtr += miVec.nDim();
    }
  }

  bool contains(const MI<T>& mi) const { return find(mi) < miVec.nMI(); }

  size_t find(const MI<T>& mi) const {
    if (mi.size() != miVec.nDim()) {
      return miVec.nMI();
    }

    const auto entry = map.find(mi.data());

    if (entry == map.end()) {
      return miVec.nMI();
    }

    return entry->second;
  }

 private:
  const MIVec<T>& miVec;
  std::unordered_map<const T*, size_t, MIVecLookupHash<T>, MIVecLookupEqual<T>> map;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
