#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

base::DataVector NodeGenFunc::genNodes(const GPCntType nNodes, const bool addBoundary) const {
  assert(!addBoundary || nNodes >= 2);

  base::DataVector nodes(nNodes);

  if (addBoundary) {
    genNodesInplace(nNodes - 2, nodes, 1);
    nodes[nNodes - 1] = 1.0;
  } else {
    genNodesInplace(nNodes, nodes);
  }

  return nodes;
}

}  // namespace combigrid
}  // namespace sgpp