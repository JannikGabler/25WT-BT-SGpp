/**
 * TODO  Remove(no longer required) */
/**
 * @file multiindex_domination.hpp
 * @brief Component-wise domination tests for multi-indices.
 *
 * @f$\vec a@f$ dominates @f$\vec b@f$ iff @f$a_k \ge b_k@f$ in every
 * dimension. This is the partial order underlying the Pareto-maxima
 * computation and the downwards-closedness check.
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  ///< Forward declaration to avoid circular includes.

namespace tools {

/**
 * @brief Returns @c true iff some multi-index in @p miVec dominates @p mi.
 * @tparam T  Element type.
 * @pre @c miVec.nDim() == mi.size().
 */
template <typename T>
bool miVecDominatesMI(const MIVec<T>& miVec, const MIView<T> mi) {
  assert(miVec.nDim() == mi.size());

  for (size_t i = 0; i < miVec.nMI(); i++) {
    if (miVec[i] >= mi) {  // Does miVec[i] dominate mi?
      return true;
    }
  }

  return false;
}

/// @copydoc miVecDominatesMI(const MIVec<T>&, MIView<T>)
template <typename T>
bool miVecDominatesMI(const MIVec<T>& miVec, const MI<T>& mi) {
  return miVecDominatesMI(miVec, MIView<T>(mi));
}

/**
 * @brief Returns @c true iff some multi-index at one of the indices in
 * @p miVecIdx dominates @p mi.
 *
 * Useful when only a subset of @p miVec is considered (e.g. the current
 * Pareto-maxima list).
 *
 * @tparam T  Element type.
 * @pre @c miVec.nDim() == mi.size().
 */
template <typename T>
bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<size_t>& miVecIdx,
                      const MIView<T> mi) {
  assert(miVec.nDim() == mi.size());

  for (const size_t idx : miVecIdx) {
    if (miVec[idx] >= mi) {  // Does miVec[idx] dominate mi?
      return true;
    }
  }

  return false;
}

/// @copydoc miVecDominatesMI(const MIVec<T>&, const std::vector<size_t>&, MIView<T>)
template <typename T>
bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<size_t>& miVecIdx, const MI<T>& mi) {
  return miVecDominatesMI(miVec, miVecIdx, MIView<T>(mi));
}

// /// @copydoc miVecDominatesMI(const MIVec<T>&, MIView<T>)
// template <typename T>
// bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<T>& mi) {
//   return miVecDominatesMI(miVec, MIView<T>(mi.data(), mi.size()));
// }

// /**
//  * @brief Returns @c true iff the multi-index at @p miVecIdx dominates @p mi
//  * (component-wise @c >=).
//  * @tparam T  Element type.
//  * @param miVec    Source vector.
//  * @param miVecIdx Index of the candidate dominator.
//  * @param mi       Candidate dominee (any view, e.g. an element of an @c MIVec).
//  * @pre @c miVec.nDim() == mi.size().
//  */
// template <typename T>
// bool miDominatesMI(const MIVec<T>& miVec, const size_t miVecIdx, const MIView<T> mi) {
//   assert(miVec.nDim() == mi.size());

//   const MIView<T> dominator = miVec[miVecIdx];

//   for (size_t dim = 0; dim < mi.size(); dim++) {
//     if (dominator[dim] < mi[dim]) {
//       return false;
//     }
//   }

//   return true;
// }

// /**
//  * @brief Returns @c true iff the multi-index at @p miIdx1 dominates the
//  * one at @p miIdx2 (component-wise @c >=).
//  *
//  * @tparam T   Multi-index element type.
//  * @param miVec Source vector.
//  * @param miIdx1 Candidate dominator.
//  * @param miIdx2 Candidate dominee.
//  */
// template <typename T>
// bool miDominatesMI(const MIVec<T>& miVec, const size_t miIdx1, const size_t miIdx2) {
//   return miDominatesMI(miVec, miIdx1, miVec[miIdx2]);
// }

// /// @copydoc miDominatesMI(const MIVec<T>&, size_t, MIView<T>)
// template <typename T>
// bool miDominatesMI(const MIVec<T>& miVec, const size_t miVecIdx, const MI<T>& mi) {
//   return miDominatesMI(miVec, miVecIdx, MIView<T>(mi));
// }

// /// @copydoc miDominatesMI(const MIVec<T>&, size_t, MIView<T>)
// template <typename T>
// bool miDominatesMI(const MIVec<T>& miVec, const size_t miVecIdx, const std::vector<T>& mi) {
//   return miDominatesMI(miVec, miVecIdx, MIView<T>(mi.data(), mi.size()));
// }

// /// @copydoc miVecDominatesMI(const MIVec<T>&, const std::vector<size_t>&, MIView<T>)
// template <typename T>
// bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<size_t>& miVecIdx,
//                       const std::vector<T>& mi) {
//   return miVecDominatesMI(miVec, miVecIdx, MIView<T>(mi.data(), mi.size()));
// }

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
