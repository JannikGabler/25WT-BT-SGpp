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