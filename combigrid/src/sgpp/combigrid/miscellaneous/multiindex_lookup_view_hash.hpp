#ifndef COMBIGRID_MISC_MULTIINDEX_LOOKUP_VIEW_HASH_HPP
#define COMBIGRID_MISC_MULTIINDEX_LOOKUP_VIEW_HASH_HPP

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>

namespace sgpp {

namespace combigrid {

namespace misc {

struct MILookupViewHash {
  size_t operator()(const MIVec::MILookupView& v) const noexcept {
    size_t h = 0;
    for (size_t i = 0; i < v.n; ++i) {
      h ^= std::hash<MIType>{}(v.data[i]) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    }
    return h;
  }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif