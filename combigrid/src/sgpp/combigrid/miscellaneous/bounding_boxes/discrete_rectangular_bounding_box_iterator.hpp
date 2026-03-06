#ifndef COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP
#define COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP

#include <cassert>
#include <cstddef>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
struct DiscRectBB;  // Forward declaration to avoid circular includes

template <typename T>
class DiscRectBBIterator {
 public:
  DiscRectBBIterator(const DiscRectBB<T>& rectDiscBB, const bool finished = false)
      : rectDiscBB(rectDiscBB),
        curPos(finished ? std::vector<T>{} : rectDiscBB.lowerBound),
        finished(finished) {}

  DiscRectBBIterator(const DiscRectBB<T>& rectDiscBB, const std::vector<T>& startPos)
      : rectDiscBB(rectDiscBB), curPos(startPos), finished(false) {
    assert(startPos.size() == rectDiscBB.nDim());
  }

  const std::vector<T>& operator*() const { return curPos; }
  const std::vector<T>* operator->() const { return &curPos; }

  DiscRectBBIterator& operator++() {
    if (rectDiscBB.includeUpperBound) {
      for (size_t dim = 0; dim < curPos.size(); dim++) {
        if (++curPos[dim] <= rectDiscBB.upperBound[dim]) {
          return *this;
        }
        curPos[dim] = rectDiscBB.lowerBound[dim];
      }
    } else {
      for (size_t dim = 0; dim < curPos.size(); dim++) {
        if (++curPos[dim] < rectDiscBB.upperBound[dim]) {
          return *this;
        }
        curPos[dim] = rectDiscBB.lowerBound[dim];
      }
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