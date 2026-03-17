#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

base::DataVector NodeGenFunc::genNodes(const GPCntType nNodes, const bool includeBoundary) const {
  base::DataVector nodes(nNodes);

  genNodesInplace(nNodes, nodes, includeBoundary);

  return nodes;
}

void NodeGenFunc::genNodesInplace(const GPCntType nNodes, base::DataVector& out,
                                  const bool includeBoundary, const size_t startIdx) const {
  assert((!includeBoundary || nNodes >= 2) && (out.size() - startIdx >= nNodes));

  if (includeBoundary) {
    out[startIdx] = 0.0;
    genNodesInplace(nNodes - 2, out, startIdx + 1);
    out[startIdx + nNodes - 1] = 1.0;
  } else {
    genNodesInplace(nNodes, out, startIdx);
  }
}

}  // namespace combigrid
}  // namespace sgpp