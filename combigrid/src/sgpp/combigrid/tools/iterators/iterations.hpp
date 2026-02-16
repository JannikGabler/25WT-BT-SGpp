#ifndef COMBIGRID_TOOLS_ITERATIONS_HPP
#define COMBIGRID_TOOLS_ITERATIONS_HPP

#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

template <typename T>
bool iterateHyperbox(std::vector<T>& curPos, const std::vector<T>& lowerBounds,
                     const std::vector<T>& upperBounds) {
  for (size_t dim = 0; dim < curPos.size(); dim++) {
    if (++curPos[dim] <= upperBounds[dim]) {
      return true;
    }
    curPos[dim] = lowerBounds[dim];
  }

  return false;
}

template <typename T>
bool iterateHyperbox(std::vector<T>& curPos, const std::vector<T>& upperBounds) {
  for (size_t dim = 0; dim < curPos.size(); dim++) {
    if (++curPos[dim] <= upperBounds[dim]) {
      return true;
    }
    curPos[dim] = 0;
  }

  return false;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
