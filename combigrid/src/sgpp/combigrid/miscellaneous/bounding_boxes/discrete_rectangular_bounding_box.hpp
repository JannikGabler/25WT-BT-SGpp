#ifndef COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_HPP
#define COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_HPP

#include <cassert>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box_iterator.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
struct DiscRectBB {
 public:
  const std::vector<T>& lowerBound;
  const std::vector<T>& upperBound;

  DiscRectBB(const std::vector<T>& lowerBound, const std::vector<T>& upperBond)
      : lowerBound(lowerBound), upperBound(upperBond) {
    assert(lowerBound.size() == upperBond.size());
  }

  DiscRectBBIterator<T> begin() const { return DiscRectBBIterator<T>(this); }

  DiscRectBBIterator<T> end() const { return DiscRectBBIterator<T>(this, true); }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif