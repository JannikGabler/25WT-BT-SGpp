#include <cassert>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_functions.hpp>

namespace sgpp {
namespace combigrid {

base::DataVector genFirstTypeChebyshevNodes(const GPCntType nNodes) {
  const double f1 = M_PI / (2 * static_cast<double>(nNodes));
  base::DataVector nodes(nNodes);

  for (GPCntType i = 0; i < nNodes; i++) {
    const double f2 = 2 * static_cast<double>(nNodes - 1 - i) + 1;  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    nodes[i] = nonTransformedNode / 2 + 0.5;
  }

  return nodes;
}

base::DataVector genSecondTypeChebyshevNodes(const GPCntType nNodes) {
  const double f1 = M_PI / static_cast<double>(nNodes + 1);
  base::DataVector nodes(nNodes);

  for (GPCntType i = 1; i <= nNodes; i++) {
    const double f2 = static_cast<double>(nNodes - i + 1);  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    nodes[i - 1] = nonTransformedNode / 2 + 0.5;
  }

  return nodes;
}

base::DataVector genClenshawCurtisNodes(const GPCntType nNodes) {
  return genSecondTypeChebyshevNodes(nNodes);
}

}  // namespace combigrid
}  // namespace sgpp