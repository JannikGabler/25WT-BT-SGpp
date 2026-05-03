/**
 * @file discrete_rectangular_bounding_box_iterator.hpp
 * @brief Forward iterator that walks all multi-indices contained in a
 * @c DiscRectBB in lexicographic (first-dim-fastest) order.
 */
#ifndef COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP
#define COMBIGRID_MISC_DISCRETE_RECT_BOUNDING_BOX_ITERATOR_HPP

#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
struct DiscRectBB;  ///< Forward declaration to avoid circular includes.

/**
 * @brief Forward iterator over a @c DiscRectBB.
 *
 * Yields the contained multi-indices one by one. Incrementing the
 * iterator advances dimension 0 first, carrying over into higher
 * dimensions when the per-dim upper bound is reached.
 *
 * @tparam T Multi-index element type.
 */
template <typename T>
class DiscRectBBIterator {
 public:
  /**
   * @brief Constructs an iterator pointing at the lower corner of the box
   * (or at @c end if @p finished is @c true).
   * @param rectDiscBB Box to iterate.
   * @param finished   Whether to construct the past-the-end iterator.
   */
  DiscRectBBIterator(const DiscRectBB<T>& rectDiscBB, const bool finished = false)
      : rectDiscBB(rectDiscBB),
        curPos(finished ? MI<T>{} : rectDiscBB.lowerBound),
        finished(finished) {}

  /**
   * @brief Constructs an iterator pointing at @p startPos inside @p rectDiscBB.
   * @param rectDiscBB Box to iterate.
   * @param startPos   Starting multi-index (must lie inside the box).
   */
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