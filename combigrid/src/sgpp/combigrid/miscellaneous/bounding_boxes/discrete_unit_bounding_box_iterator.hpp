/**
 * @file discrete_unit_bounding_box_iterator.hpp
 * @brief Forward iterator over the @f$2^d@f$ corners of a @c DiscUnitBB.
 */
#ifndef COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_ITERATOR_HPP
#define COMBIGRID_MISC_DISCRETE_UNIT_BOUNDING_BOX_ITERATOR_HPP

#include <cstddef>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T>
struct DiscUnitBB;  ///< Forward declaration to avoid circular includes.

/**
 * @brief Forward iterator over the @f$2^d@f$ corners of @f$\{0,1\}^d@f$.
 *
 * Yields each corner as an immutable @c std::vector<T>. Increment uses
 * the standard binary carry: the lowest dimension whose entry is still
 * @c 0 becomes @c 1; lower entries reset to @c 0.
 *
 * @tparam T Element type.
 */
template <typename T>
class DiscUnitBBIterator {
 public:
  /**
   * @brief Constructs the begin or end iterator.
   * @param discUnitBB Owning unit hypercube.
   * @param finished   Whether to construct the past-the-end iterator.
   */
  DiscUnitBBIterator(const DiscUnitBB<T>& discUnitBB, const bool finished = false)
      : discUnitBB(discUnitBB), finished(finished), curPos(finished ? 0 : discUnitBB.nDim, 0) {}

  /// @brief Read-only access to the current corner.
  const std::vector<T>& operator*() const { return curPos; }
  /// @brief Read-only pointer to the current corner.
  const std::vector<T>* operator->() const { return &curPos; }

  /// @brief Advances to the next corner (binary carry).
  DiscUnitBBIterator& operator++() {
    for (size_t dim = 0; dim < curPos.size(); dim++) {
      if (curPos[dim]++ == 0) {
        return *this;
      }
      curPos[dim] = 0;
    }
    finished = true;
    return *this;
  }

  /// @brief Iterator equality.
  bool operator==(const DiscUnitBBIterator& other) const {
    return finished == other.finished && (finished || curPos == other.curPos);
  }

  /// @brief Iterator inequality.
  bool operator!=(const DiscUnitBBIterator& other) const { return !(*this == other); }

 private:
  const DiscUnitBB<T>& discUnitBB;  ///< Owning hypercube.
  std::vector<T> curPos;            ///< Current corner.
  bool finished = false;            ///< @c true iff the iterator is past-the-end.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
