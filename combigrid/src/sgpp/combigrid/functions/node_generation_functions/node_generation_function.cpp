#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
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

void NodeGenFunc::genDeltaNodes(const LvlType lvl, const Lvl2GPCntFunc lvl2GPCntFunc,
                                std::span<double> out, const bool includeBoundary) const {
  assert(out.size() == tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, includeBoundary));

  if (includeBoundary) {
    genDeltaNodesWithBoundary(lvl, lvl2GPCntFunc, out);
  } else {
    genDeltaNodesWithoutBoundary(lvl, lvl2GPCntFunc, out);
  }
}

base::DataVector NodeGenFunc::genDeltaNodes(const LvlType lvl, const Lvl2GPCntFunc lvl2GPCntFunc,
                                            const bool includeBoundary) const {
  const GPCntType nNodes = tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, includeBoundary);

  base::DataVector result(nNodes);
  genDeltaNodes(lvl, lvl2GPCntFunc, result, includeBoundary);
  return result;
}

void NodeGenFunc::genDeltaNodesWithBoundary(const LvlType lvl, const Lvl2GPCntFunc lvl2GPCntFunc,
                                            std::span<double> out) const {
  const GPCntType nNodes = tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, true);

  assert(out.size() == nNodes);

  if (nNodes <= 2) {
    if (nNodes == 1) {
      out[0] = 0.5;
    } else if (nNodes == 2) {
      out[0] = 0.0;
      out[nNodes - 1] = 1.0;
    }
  } else {  // nNodes >= 3
    genDeltaNodesWithoutBoundary(lvl, lvl2GPCntFunc, out.subspan(1, nNodes - 2));
    out[0] = 0.0;
    out[nNodes - 1] = 1.0;
  }
}

base::DataVector NodeGenFunc::genDeltaNodesWithBoundary(const LvlType lvl,
                                                        const Lvl2GPCntFunc lvl2GPCntFunc) const {
  const GPCntType nNodes = tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, true);

  base::DataVector result(nNodes);
  genDeltaNodesWithBoundary(lvl, lvl2GPCntFunc, result);
  return result;
}

base::DataVector NodeGenFunc::genDeltaNodesWithoutBoundary(
    const LvlType lvl, const Lvl2GPCntFunc lvl2GPCntFunc) const {
  const GPCntType nNodes = tools::getDeltaNodeCnt(lvl, lvl2GPCntFunc, false);

  base::DataVector result(nNodes);
  genDeltaNodesWithoutBoundary(lvl, lvl2GPCntFunc, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp