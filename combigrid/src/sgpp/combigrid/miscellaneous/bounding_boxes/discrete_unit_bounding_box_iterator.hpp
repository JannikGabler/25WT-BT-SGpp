#ifndef COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_ITERATOR_HPP
#define COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_ITERATOR_HPP

#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
class DiscUnitBBIterator {
 public:
  DiscUnitBBIterator(const DiscUnitBB<T>& discUnitBB, const bool finished = false)
      : discUnitBB(discUnitBB), finished(finished), curPos(finished ? 0 : discUnitBB.nDim, 0) {}

  const T& operator*() const { return curPos; }
  const T* operator->() const { return &curPos; }

  DiscUnitBBIterator& operator++() {
    for (std::size_t dim = 0; dim < curPos.size(); dim++) {
      if (curPos[dim]++ == 0) {
        return *this;
      }
      curPos[dim] = 0;
    }
    finished = true;
    return *this;
  }

  bool operator==(const DiscUnitBBIterator& other) const {
    return finished == other.finished && (finished || curPos == other.curPos);
  }

  bool operator!=(const DiscUnitBBIterator& other) const { return !(*this == other); }

 private:
  const DiscUnitBB<T>& discUnitBB;
  std::vector<T> curPos;
  bool finished = false;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif