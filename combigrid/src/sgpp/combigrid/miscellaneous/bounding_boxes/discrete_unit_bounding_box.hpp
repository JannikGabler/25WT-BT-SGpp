#ifndef COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_HPP
#define COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_HPP

#include <cassert>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box_iterator.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
struct DiscUnitBB {
  DiscUnitBB(const size_t nDim) : nDim(nDim) {}

 public:
  const size_t nDim;

  DiscUnitBBIterator<T> begin() const { return DiscUnitBBIterator<T>(*this); }

  DiscUnitBBIterator<T> end() const { return DiscUnitBBIterator<T>(*this, true); }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif