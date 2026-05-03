/**
 * @file discrete_unit_bounding_box.hpp
 * @brief Discrete @c d-dimensional unit hypercube @f$\{0,1\}^d@f$ used to
 * iterate over corner offsets.
 */
#ifndef COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_HPP
#define COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_HPP

#include <cassert>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box_iterator.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Discrete unit hypercube @f$\{0,1\}^d@f$.
 *
 * Trivial helper that exposes a forward iterator over the @f$2^d@f$
 * corner offsets. Used by the downwards-closedness test and various
 * @f$2^d@f$-corner enumerations (e.g. multilinear interpolation,
 * combination-coefficient computation).
 *
 * @tparam T Element type of the iterated multi-index entries.
 */
template <typename T>
struct DiscUnitBB {
  /**
   * @brief Constructs the unit hypercube of dimension @p nDim.
   * @param nDim Spatial dimensionality.
   */
  DiscUnitBB(const size_t nDim) : nDim(nDim) {}

 public:
  const size_t nDim;  ///< Dimensionality of the hypercube.

  /// @brief Iterator to the first corner (the all-zero corner).
  DiscUnitBBIterator<T> begin() const { return DiscUnitBBIterator<T>(*this); }

  /// @brief Past-the-end iterator.
  DiscUnitBBIterator<T> end() const { return DiscUnitBBIterator<T>(*this, true); }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
