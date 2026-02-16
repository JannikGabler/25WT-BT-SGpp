#ifndef COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP
#define COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP

#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
class DiscRectBBIterator {
 public:
  DiscRectBBIterator(const DiscRectBB<T>& rectDiscBB, const bool finished = false)
      : rectDiscBB(rectDiscBB), curPos(finished ? 0 : rectDiscBB.lowerBound), finished(finished) {}

  const T& operator*() const { return curPos; }
  const T* operator->() const { return &curPos; }

  DiscRectBBIterator& operator++() {
    for (std::size_t dim = 0; dim < curPos.size(); dim++) {
      if (++curPos[dim] <= rectDiscBB.upperBound[dim]) {
        return *this;
      }
      curPos[dim] = rectDiscBB.lowerBound[dim];
    }
    finished = true;
    return *this;
  }

  bool operator==(const DiscRectBBIterator& other) const {
    return finished == other.finished && (finished || curPos == other.curPos);
  }

  bool operator!=(const DiscRectBBIterator& other) const { return !(*this == other); }

 private:
  const DiscRectBB<T>& rectDiscBB;
  std::vector<T> curPos;
  bool finished = false;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif