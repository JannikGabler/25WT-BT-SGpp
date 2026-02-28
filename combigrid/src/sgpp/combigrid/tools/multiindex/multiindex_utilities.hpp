#ifndef COMBIGRID_TOOLS_MULTIINDEX_UTILITIES_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_UTILITIES_HPP

#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

template <typename T>
size_t numberOfMIWithComponentSum(const size_t nDim, const T sumOfComponents) {
  return binomial(sumOfComponents + nDim - 1, nDim - 1);  // nDim = 0 should work fine
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
