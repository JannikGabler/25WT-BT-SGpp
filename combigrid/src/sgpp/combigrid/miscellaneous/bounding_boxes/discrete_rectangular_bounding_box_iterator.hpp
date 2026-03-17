#ifndef COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP
#define COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP

#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>

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
        curPos(finished ? MI<T>{} : rectDiscBB.lowerBound),
        finished(finished) {}

  DiscRectBBIterator(const DiscRectBB<T>& rectDiscBB, const MI<T>& startPos)
      : rectDiscBB(rectDiscBB), curPos(startPos), finished(false) {
    assert(startPos.size() == rectDiscBB.nDim());
  }

  const MI<T>& operator*() const { return curPos; }
  const MI<T>* operator->() const { return &curPos; }

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
  MI<T> curPos;
  bool finished = false;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif