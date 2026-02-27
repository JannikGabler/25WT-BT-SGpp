#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_functions.hpp>

namespace sgpp {
namespace combigrid {

base::DataVector genEquidistantNodes(const GPCntType nNodes) {
  const double denominator = static_cast<double>(nNodes + 1);
  base::DataVector nodes(nNodes);

  for (GPCntType i = 1; i <= nNodes; i++) {
    const double numerator = static_cast<double>(i);

    nodes[i - 1] = numerator / denominator;
  }

  return nodes;
}

}  // namespace combigrid
}  // namespace sgpp