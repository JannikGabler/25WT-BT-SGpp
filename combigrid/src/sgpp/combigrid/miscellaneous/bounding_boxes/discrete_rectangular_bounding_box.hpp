/**
 * @file discrete_rectangular_bounding_box.hpp
 * @brief Discrete axis-aligned bounding box of multi-indices and its
 * lexicographic iterator.
 */
#pragma once

#include <cassert>
#include <limits>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box_iterator.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Discrete rectangular bounding box of multi-indices.
 *
 * Represents the half-open or closed Cartesian product
 * @f$[\ell_1, u_1] \times \cdots \times [\ell_d, u_d]@f$ of integer
 * intervals. The box is iterable: incrementing the iterator visits each
 * contained multi-index once, in row-major (first-dim-fastest) order.
 *
 * @tparam T Multi-index element type.
 */
template <typename T>
struct DiscRectBB {
 public:
  const MI<T> lowerBound;       ///< Inclusive lower corner of the box (per dimension).
  const MI<T> upperBound;       ///< Upper corner; inclusive iff @ref includeUpperBound is @c true.
  const bool includeUpperBound; ///< Whether the upper corner is part of the box.

  /**
   * @brief Constructs a bounding box.
   * @param lowerBound        Inclusive per-dim lower corner.
   * @param upperBound        Per-dim upper corner.
   * @param includeUpperBound If @c true (default), the upper corner is
   *                          part of the box; if @c false, it is half-open.
   */
  DiscRectBB(MI<T> lowerBound, MI<T> upperBound, const bool includeUpperBound = true)
      : lowerBound(std::move(lowerBound)),
        upperBound(std::move(upperBound)),
        includeUpperBound(includeUpperBound) {
    assert(lowerBound.size() == upperBound.size());
  }

  /**
   * @brief Returns the number of multi-indices contained in the box.
   * @return Box volume; @c 0 for an empty/invalid box.
   */
  size_t size() const {
    if (lowerBound.size() == 0) {
      return 0;
    }

    size_t size = 1;
    for (size_t dim = 0; dim < lowerBound.size(); dim++) {
      if (lowerBound[dim] > upperBound[dim]) {
        return 0;
      }

      if (includeUpperBound) {
        size *= upperBound[dim] - lowerBound[dim] + 1;
      } else {
        size *= upperBound[dim] - lowerBound[dim];
      }
    }

    return size;
  }

  /// @brief Returns the dimensionality of the box.
  size_t nDim() const { return upperBound.size(); }

  /// @brief Iterator to the first multi-index (the lower corner).
  DiscRectBBIterator<T> begin() const { return DiscRectBBIterator<T>(*this); }

  /**
   * @brief Iterator starting at the given multi-index inside the box.
   * @param start Starting multi-index (must lie inside the box).
   */
  DiscRectBBIterator<T> begin(const MI<T>& start) const {
    return DiscRectBBIterator<T>(*this, start);
  }
  /**
   * @brief Iterator starting at the given linear index.
   * @param startIdx Linear index in @c [0, size()).
   */
  DiscRectBBIterator<T> begin(const size_t& startIdx) const {
    return begin(linearIndexToPos(startIdx));
  }

  /// @brief Past-the-end iterator.
  DiscRectBBIterator<T> end() const { return DiscRectBBIterator<T>(*this, true); }

  /****************
  Helper operations
  ****************/
  /**
   * @brief Per-dimension stride for converting between linear and
   * multi-index addressing inside the box.
   *
   * Entry @c k contains the number of linear-index increments needed
   * before the @c k-th component of the multi-index changes.
   */
  std::vector<size_t> getIdxCntUntilDimChange() const {
    const size_t nDim = this->nDim();
    std::vector<size_t> idxCntUntilDimChange(nDim, 1);
    size_t curCnt = 1;

    for (size_t dim = 1; dim < nDim; dim++) {
      if (includeUpperBound) {
        curCnt *= upperBound[dim - 1] - lowerBound[dim - 1] + 1;
      } else {
        curCnt *= upperBound[dim - 1] - lowerBound[dim - 1];
      }
      idxCntUntilDimChange[dim] = curCnt;
    }

    return idxCntUntilDimChange;
  }

  /**
   * @brief Maps a linear index inside the box to the corresponding
   * multi-index.
   * @param idx Linear index in @c [0, size()).
   */
  MI<T> linearIndexToPos(size_t idx) const {
    const size_t nDim = this->nDim();
    const std::vector<size_t> idxCntUntilDimChange = getIdxCntUntilDimChange();
    std::vector<T> pos(nDim);

    for (size_t dim = nDim - 1; dim != std::numeric_limits<size_t>::max(); dim--) {
      const size_t nIdx = idxCntUntilDimChange[dim];
      pos[dim] = idx / nIdx;
      idx %= nIdx;
    }

    return pos;
  }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp