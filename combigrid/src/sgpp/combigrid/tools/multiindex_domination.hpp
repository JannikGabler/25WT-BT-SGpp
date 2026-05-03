/**
 * @file multiindex_domination.hpp
 * @brief Component-wise domination tests for multi-indices.
 *
 * @f$\vec a@f$ dominates @f$\vec b@f$ iff @f$a_k \ge b_k@f$ in every
 * dimension. This is the partial order underlying the Pareto-maxima
 * computation and the downwards-closedness check.
 */
#ifndef COMBIGRID_MULTIINDEX_DOMINATION_HPP
#define COMBIGRID_MULTIINDEX_DOMINATION_HPP

#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  ///< Forward declaration to avoid circular includes.

namespace tools {

/**
 * @brief Returns @c true iff the multi-index at @p miIdx1 dominates the
 * one at @p miIdx2 (component-wise @c >=).
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source vector.
 * @param miIdx1 Candidate dominator.
 * @param miIdx2 Candidate dominee.
 */
template <typename T>
bool miDominatesMI(const MIVec<T>& miVec, const size_t miIdx1, const size_t miIdx2) {
  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
    if (miVec(miIdx1, dim) < miVec(miIdx2, dim)) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Returns @c true iff the multi-index at @p miVecIdx dominates @p mi.
 * @tparam T  Element type.
 * @pre @c miVec.nDim() == mi.size().
 */
template <typename T>
bool miDominatesMI(const MIVec<T>& miVec, const size_t miVecIdx, const MI<T>& mi) {
  return miDominatesMI(miVec, miVecIdx, mi);
}

/// @copydoc miDominatesMI(const MIVec<T>&, size_t, const MI<T>&)
template <typename T>
bool miDominatesMI(const MIVec<T>& miVec, const size_t miVecIdx, const std::vector<T>& mi) {
  assert(miVec.nDim() == mi.size());

  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
    if (miVec(miVecIdx, dim) < mi[dim]) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Returns @c true iff some multi-index in @p miVec dominates @p mi.
 * @tparam T  Element type.
 * @pre @c miVec.nDim() == mi.size().
 */
template <typename T>
bool miVecDominatesMI(const MIVec<T>& miVec, const MI<T>& mi) {
  return miVecDominatesMI(miVec, mi);
}

/// @copydoc miVecDominatesMI(const MIVec<T>&, const MI<T>&)
template <typename T>
bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<T>& mi) {
  assert(miVec.nDim() == mi.size());

  for (size_t miVecIdx = 0; miVecIdx < miVec.nMI(); miVecIdx++) {
    if (miDominatesMI(miVec, miVecIdx, mi)) {
      return true;
    }
  }

  return false;
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
bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<size_t>& miVecIdx, const MI<T>& mi) {
  return miVecDominatesMI(miVec, miVecIdx, mi);
}

/// @copydoc miVecDominatesMI(const MIVec<T>&, const std::vector<size_t>&, const MI<T>&)
template <typename T>
bool miVecDominatesMI(const MIVec<T>& miVec, const std::vector<size_t>& miVecIdx,
                      const std::vector<T>& mi) {
  assert(miVec.nDim() == mi.size());

  for (const size_t idx : miVecIdx) {
    if (miDominatesMI(miVec, idx, mi)) {
      return true;
    }
  }

  return false;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif