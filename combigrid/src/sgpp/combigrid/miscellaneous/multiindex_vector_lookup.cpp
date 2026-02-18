#include <sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup_hash.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>

namespace sgpp {

namespace combigrid {

namespace misc {

MIVecLookup::MIVecLookup(const MIVec& miVec)
    : miVec((miVec)),
      map(miVec.nMI(), MIVecLookupHash(miVec.nDim()), MIVecLookupEqual(miVec.nDim())) {
  const MIType* dataPtr = miVec.data();

  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    map.emplace(dataPtr, miIdx);
    dataPtr += miVec.nDim();
  }
}

bool MIVecLookup::contains(const MI& mi) const { return find(mi) < miVec.nMI(); }

size_t MIVecLookup::find(const MI& mi) const {
  if (mi.size() != miVec.nDim()) {
    return miVec.nMI();
  }

  const auto entry = map.find(mi.data());

  if (entry == map.end()) {
    return miVec.nMI();
  }

  return entry->second;
}

}  // namespace misc

}  // namespace combigrid

}  // namespace sgpp
