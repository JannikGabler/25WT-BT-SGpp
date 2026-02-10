#ifndef COMBIGRID_MISC_MULTIINDEX_LOOKUP_VIEW_EQUAL_HPP
#define COMBIGRID_MISC_MULTIINDEX_LOOKUP_VIEW_EQUAL_HPP

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>

namespace sgpp {

namespace combigrid {

namespace misc {

struct MILookupViewEqual {
  bool operator()(const MIVec::MILookupView& lhs, const MIVec::MILookupView& rhs) const noexcept {
    if (lhs.n != rhs.n) return false;
    return std::equal(lhs.data, lhs.data + lhs.n, rhs.data);
  }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif