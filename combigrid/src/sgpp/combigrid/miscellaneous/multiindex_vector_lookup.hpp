#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HPP

#include <sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup_hash.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <unordered_map>

namespace sgpp {

namespace combigrid {

namespace misc {

class MIVecLookup {
 public:
  MIVecLookup(const MIVec& miVec);

  bool contains(const MI& mi) const;

  size_t find(const MI& mi) const;

 private:
  const size_t nDim;
  std::unordered_map<const MIType*, size_t, MIVecLookupHash, MIVecLookupEqual> map;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
