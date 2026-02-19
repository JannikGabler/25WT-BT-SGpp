#include <sgpp/combigrid/tools/math/math_operations.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

size_t numberOfMIWithComponentSum(const size_t nDim, const MIType sumOfComponents) {
  return binomial(sumOfComponents + nDim - 1, nDim - 1);  // nDim = 0 should work fine
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp