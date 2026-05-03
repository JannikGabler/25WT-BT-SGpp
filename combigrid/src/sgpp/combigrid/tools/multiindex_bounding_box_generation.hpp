/**
 * @file multiindex_bounding_box_generation.hpp
 * @brief Helper to derive a discrete bounding box around a set of multi-indices.
 */
#ifndef COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP

#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  ///< Forward declaration to avoid circular includes.

namespace tools {

// misc::DiscRectBB<MIType> genRectMIBoundingBox(const std::vector<MI>& mi);

/**
 * @brief Builds a discrete rectangular bounding box from
 * @f$\vec 0@f$ to the component-wise maximum of @p miVec.
 *
 * The resulting box covers every multi-index in @p miVec (component-wise)
 * and can be iterated to enumerate the full Cartesian product of indices
 * up to that maximum.
 *
 * @tparam T  Multi-index element type.
 * @param miVec Source set of multi-indices.
 * @return Discrete rectangular bounding box.
 */
template <typename T>
misc::DiscRectBB<T> genRectMIBoundingBox(const MIVec<T>& miVec) {
  const std::shared_ptr<MI<T>> max = miVec.componentWiseMax();
  const MI<T> lowerBound(max->size(), 0);

  return misc::DiscRectBB<T>(lowerBound, *max);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
