#ifndef COMBIGRID_TOOLS_ITERATIONS_HPP
#define COMBIGRID_TOOLS_ITERATIONS_HPP

#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

template <typename T>
class HyperBoxIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = std::vector<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = const value_type*;
  using reference = const value_type&;

  HyperBoxIterator(const std::vector<T>& lowerBounds, const std::vector<T>& upperBounds,
                   const bool finished = false)
      : lowerBounds(lowerBounds),
        upperBounds(upperBounds),
        curPos(lowerBounds),
        finished_(finished) {
    if (finished) {
      curPos.clear();
    }
  }

  HyperBoxIterator(const std::vector<T>& upperBounds, const bool finished = false)
      : lowerBounds(upperBounds.size()),
        upperBounds(upperBounds),
        curPos(lowerBounds),
        finished_(finished) {
    if (finished) {
      curPos.clear();
    }
  }

  reference operator*() const { return curPos; }
  pointer operator->() const { return &curPos; }

  HyperBoxIterator& operator++() {
    increment();
    return *this;
  }

  HyperBoxIterator operator++(int) {
    HyperBoxIterator tmp = *this;
    increment();
    return tmp;
  }

  bool operator==(const HyperBoxIterator<T>& other) const {
    return finished_ == other.finished_ && curPos == other.curPos;
  }

  bool operator!=(const HyperBoxIterator<T>& other) const { return !(*this == other); }

  bool finished() const { return finished_; }

 private:
  const std::vector<T>& lowerBounds;
  const std::vector<T>& upperBounds;
  std::vector<int> curPos;
  bool finished_;

  void increment() {
    if (finished_) {
      return;
    }

    for (std::size_t dim = 0; dim < curPos.size(); dim++) {
      if (++curPos[dim] <= upperBounds[dim]) {
        return;
      }
      curPos[dim] = 0;
    }

    // Overflow → end
    finished_ = true;
    curPos.clear();
  }
};

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
