// #include <cassert>
// #include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
// #include <sgpp/combigrid/tools/multiindex_domination.hpp>
// #include <vector>
// // #include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
// // #include <sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp>

// namespace sgpp {
// namespace combigrid {
// namespace tools {

// template <typename T>
// bool miDominatesMI(const MIVec<T>& miVec, const size_t miIdx1, const size_t miIdx2) {
//   for (size_t dim = 0; dim < miVec.nDim(); dim++) {
//     if (miVec(miIdx1, dim) < miVec(miIdx2, dim)) {
//       return false;
//     }
//   }

//   return true;
// }

// /*
// Should only be called with miVec.nDim == mi.nDim
// */
// template <typename T>
// bool miDominatesMI(const MIVec<T>& miVec, const size_t miVecIdx, const MI<T>& mi) {
//   return miDominatesMI(miVec, miVecIdx, mi);
// }

// template <typename T>
// bool miDominatesMI(const MIVec<T>& miVec, const size_t miVecIdx, const std::vector<T>& mi) {
//   assert(miVec.nDim() == mi.size());

//   for (size_t dim = 0; dim < miVec.nDim(); dim++) {
//     if (miVec(miVecIdx, dim) < mi[dim]) {
//       return false;
//     }
//   }

//   return true;
// }

// /*
// Should only be called with miVec.nDim == mi.nDim
// */
// template <typename T>
// bool miVecDominatesMI(const MIVec<T>& miVec, const MI<T>& mi) {
//   return miVecDominatesMI(miVec, mi);
// }

// template <typename T>
// bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<T>& mi) {
//   assert(miVec.nDim() == mi.size());

//   for (size_t miVecIdx = 0; miVecIdx < miVec.nMI(); miVecIdx++) {
//     if (miDominatesMI(miVec, miVecIdx, mi)) {
//       return true;
//     }
//   }

//   return false;
// }

// /*
// Should only be called with miVec.nDim == mi.nDim
// */
// template <typename T>
// bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<size_t>& miVecIdx, const MI<T>&
// mi) {
//   return miVecDominatesMI(miVec, miVecIdx, mi);
// }

// template <typename T>
// bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<size_t>& miVecIdx,
//                       const std::vector<T>& mi) {
//   assert(miVec.nDim() == mi.size());

//   for (const size_t idx : miVecIdx) {
//     if (miDominatesMI(miVec, idx, mi)) {
//       return true;
//     }
//   }

//   return false;
// }

// }  // namespace tools
// }  // namespace combigrid
// }  // namespace sgpp
