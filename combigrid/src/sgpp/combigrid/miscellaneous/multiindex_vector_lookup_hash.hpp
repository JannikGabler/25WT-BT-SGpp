#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HASH_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HASH_HPP

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>

namespace sgpp {

namespace combigrid {

namespace misc {

struct MIVecLookupHash {
 public:
  MIVecLookupHash(size_t nDim);

  size_t operator()(const MIType* dataPtr) const;

 private:
  const size_t nDim;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif