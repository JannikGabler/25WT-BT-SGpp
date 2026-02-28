// #include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup_hash.hpp>
// #include <sgpp/combigrid/multiindices/multiindex.hpp>
// #include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
// #include <sgpp/globaldef.hpp>

// namespace sgpp {

// namespace combigrid {

// namespace misc {

// MIVecLookupHash::MIVecLookupHash(const size_t nDim) : nDim(nDim) {}

// size_t MIVecLookupHash::operator()(const MIType* dataPtr) const {
//   size_t h = 1469598103934665603ULL;  // FNV offset basis

//   for (size_t dim = 0; dim < nDim; dim++) {
//     const size_t t = dataPtr[dim] + 0x9e3779b97f4a7c15ULL;
//     h ^= t;
//     h *= 1099511628211ULL;  // FNV prime
//   }

//   return h;
// }

// }  // namespace misc
// }  // namespace combigrid
// }  // namespace sgpp
