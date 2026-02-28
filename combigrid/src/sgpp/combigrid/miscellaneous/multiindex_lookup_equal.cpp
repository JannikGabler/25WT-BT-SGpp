// #include "sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp"
// #include <sgpp/combigrid/multiindices/multiindex.hpp>
// #include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
// #include <sgpp/globaldef.hpp>

// namespace sgpp {

// namespace combigrid {

// namespace misc {

// MIVecLookupEqual::MIVecLookupEqual(const size_t nDim) : nDim(nDim) {}

// bool MIVecLookupEqual::operator()(const MIType* a, const MIType* b) const {
//   for (size_t dim = 0; dim < nDim; dim++) {
//     if (a[dim] != b[dim]) {
//       return false;
//     }
//   }

//   return true;
// }

// }  // namespace misc
// }  // namespace combigrid
// }  // namespace sgpp