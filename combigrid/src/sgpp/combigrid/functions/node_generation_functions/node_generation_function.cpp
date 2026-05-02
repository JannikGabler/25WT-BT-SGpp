#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

void NodeGenFunc::genNodes(const GPCntType nNodes, base::DataVector& out,
                           const bool includeBoundary, const size_t startIdx) const {
  assert(out.size() >= nNodes + startIdx);

  if (includeBoundary) {
    genNodesWithBoundary(nNodes, out, startIdx);
  } else {
    genNodesWithoutBoundary(nNodes, out, startIdx);
  }
}

base::DataVector NodeGenFunc::genNodes(const GPCntType nNodes, const bool includeBoundary) const {
  base::DataVector result(nNodes);
  genNodes(nNodes, result, includeBoundary);
  return result;
}

void NodeGenFunc::genNodesWithBoundary(const GPCntType nNodes, base::DataVector& out,
                                       const size_t startIdx) const {
  assert(out.size() >= nNodes + startIdx);

  if (nNodes <= 2) {
    if (nNodes == 1) {
      out[startIdx] = 0.5;
    } else if (nNodes == 2) {
      out[startIdx] = 0.0;
      out[startIdx + nNodes - 1] = 1.0;
    }
  } else {  // nNodes >= 3
    genNodesWithoutBoundary(nNodes - 2, out, startIdx + 1);
    out[startIdx] = 0.0;
    out[startIdx + nNodes - 1] = 1.0;
  }
}

base::DataVector NodeGenFunc::genNodesWithBoundary(const GPCntType nNodes) const {
  base::DataVector result(nNodes);
  genNodesWithBoundary(nNodes, result);
  return result;
}

base::DataVector NodeGenFunc::genNodesWithoutBoundary(const GPCntType nNodes) const {
  base::DataVector result(nNodes);
  genNodesWithoutBoundary(nNodes, result);
  return result;
}

// void NodeGenFunc::genNodesInplace(const GPCntType nNodes, base::DataVector& out,
//                                   const bool includeBoundary, const size_t startIdx) const {
//   assert((!includeBoundary || nNodes >= 2) && (out.size() - startIdx >= nNodes));

//   if (includeBoundary) {
//     out[startIdx] = 0.0;
//     genNodesInplace(nNodes - 2, out, startIdx + 1);
//     out[startIdx + nNodes - 1] = 1.0;
//   } else {
//     genNodesInplace(nNodes, out, startIdx);
//   }
// }

}  // namespace combigrid
}  // namespace sgpp