#pragma once

#include <cassert>
#include <limits>
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
  const bool includeUpperBound;

  DiscRectBB(std::vector<T> lowerBound, std::vector<T> upperBound,
             const bool includeUpperBound = true)
      : lowerBound(std::move(lowerBound)),
        upperBound(std::move(upperBound)),
        includeUpperBound(includeUpperBound) {
    assert(lowerBound.size() == upperBound.size());
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

      if (includeUpperBound) {
        size *= upperBound[dim] - lowerBound[dim] + 1;
      } else {
        size *= upperBound[dim] - lowerBound[dim];
      }
    }

    return size;
  }

  size_t nDim() const { return upperBound.size(); }

  DiscRectBBIterator<T> begin() const { return DiscRectBBIterator<T>(*this); }

  DiscRectBBIterator<T> begin(const std::vector<T>& start) const {
    return DiscRectBBIterator<T>(*this, start);
  }
  DiscRectBBIterator<T> begin(const size_t& startIdx) const {
    return begin(linearIndexToPos(startIdx));
  }

  DiscRectBBIterator<T> end() const { return DiscRectBBIterator<T>(*this, true); }

  /****************
  Helper operations
  ****************/
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

  std::vector<T> linearIndexToPos(size_t idx) const {
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