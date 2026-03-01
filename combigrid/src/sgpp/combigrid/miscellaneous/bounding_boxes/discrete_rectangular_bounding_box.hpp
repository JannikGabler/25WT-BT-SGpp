#ifndef COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_HPP
#define COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_HPP

#include <cassert>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box_iterator.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
struct DiscRectBB {
 public:
  const std::vector<T> lowerBound;
  const std::vector<T> upperBound;

  DiscRectBB(std::vector<T> lowerBound, std::vector<T> upperBond)
      : lowerBound(std::move(lowerBound)), upperBound(std::move(upperBond)) {
    assert(lowerBound.size() == upperBond.size());
  }

  size_t size() const {
    if (lowerBound.size() == 0) {
      return 0;
    }

    size_t size = 1;
    for (size_t dim = 0; dim < lowerBound.size(); dim++) {
      if (lowerBound[dim] > upperBound[dim]) {
        return 0;
      }
      size *= upperBound[dim] - lowerBound[dim] + 1;
    }
    return size;
  }

  DiscRectBBIterator<T> begin() const { return DiscRectBBIterator<T>(*this); }

  DiscRectBBIterator<T> end() const { return DiscRectBBIterator<T>(*this, true); }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif