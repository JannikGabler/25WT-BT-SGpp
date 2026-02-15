#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_EQUAL_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_EQUAL_HPP

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>

namespace sgpp {

namespace combigrid {

namespace misc {

struct MIVecLookupEqual {
  MIVecLookupEqual(size_t nDim);

  bool operator()(const MIType* a, const MIType* b) const;

 private:
  const size_t nDim;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif