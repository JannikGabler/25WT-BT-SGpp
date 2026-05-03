/**
 * @file multiindex_hash.hpp
 * @brief Placeholder for a hash functor over @c MI multi-indices.
 *
 * The original implementation is currently commented out; multi-index
 * hashing for lookup tables is provided by the dedicated structures in
 * @c miscellaneous/multiindex_vector_lookup_hash.hpp instead.
 */

// #include <sgpp/combigrid/multiindices/multiindex.hpp>

// namespace sgpp {
// namespace combigrid {

// struct MIHash {
//   std::size_t operator()(const MI& mi) const {
//     size_t hash = 0;

//     for (const MIType elem : mi) {
//       hash ^= std::hash<MIType>{}(elem) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
//     }

//     return hash;
//   }
// };

// }  // namespace combigrid
// }  // namespace sgpp
