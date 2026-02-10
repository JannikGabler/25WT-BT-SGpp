#ifndef COMBIGRID_MISC_MULTIINDEX_LOOKUP_HPP
#define COMBIGRID_MISC_MULTIINDEX_LOOKUP_HPP

#include <sgpp/combigrid/miscellaneous/multiindex_lookup_view.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_lookup_view_equal.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_lookup_view_hash.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <unordered_set>

namespace sgpp {

namespace combigrid {

namespace misc {

class MILookup {
 public:
  explicit MILookup(const MIVec& miVec) : nDim(miVec.nDim()) {
    lookup.reserve(miVec.nMI());

    for (size_t i = 0; i < miVec.nMI(); ++i) {
      lookup.insert(miVec.getLookupView(i));
    }
  }

  bool contains(const MI& mi) const {
    if (mi.size() != nDim) {
      return false;
    }

    MIVec::MILookupView q{mi.data(), nDim};
    return lookup.find(q) != lookup.end();
  }

 private:
  size_t nDim;
  std::unordered_set<MIVec::MILookupView, MILookupViewHash, MILookupViewEqual> lookup;
};

}  // namespace misc

}  // namespace combigrid

}  // namespace sgpp

#endif