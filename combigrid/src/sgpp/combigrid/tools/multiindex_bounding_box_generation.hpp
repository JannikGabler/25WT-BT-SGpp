#ifndef COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP

#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  // Forward declaration to avoid circular includes

namespace tools {

// misc::DiscRectBB<MIType> genRectMIBoundingBox(const std::vector<MI>& mi);
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
