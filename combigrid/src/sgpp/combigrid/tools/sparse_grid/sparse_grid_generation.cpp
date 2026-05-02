#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

void populateSG(const SGGenInstr& genInstr, const LvlMIVec& miVec,
                const std::vector<CTCoeffType>& coeffs, SparseGrid& out) {
  assert(out.nTG() >= miVec.nMI());

  // const SGGenNodeLookup lookup = genSGNodeLookup(genInstr, miVec, coeffs);
  size_t maxGPCnt = 0;
  size_t maxSumOverGPCntsPerDim = 0;

#pragma omp parallel for reduction(max : maxGPCnt, maxSumOverGPCntsPerDim) \
    schedule(guided)  // TODO: Schedule (benchmark)
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const LvlMI mi = miVec[miIdx];
    const CTCoeffType coeff = coeffs[miIdx];

    const TensorGrid tg = genTGForMI(mi, genInstr);
    out.setTensorGrid(miIdx, {mi, coeff, std::move(tg)});

    maxGPCnt = std::max(maxGPCnt, tg.nGP());
    maxSumOverGPCntsPerDim = std::max(maxSumOverGPCntsPerDim, tg.getNodesPerDim().size());
  }

  out.setMaxTGGPCnt(maxGPCnt);
  out.setMaxTGSumOverGPCntsPerDim(maxSumOverGPCntsPerDim);
}

TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr) {
  GPMI gpCntPerDim = sg_gen::getGPCntPerDim(mi, genInstr);
  // const misc::DiscRectBB<GPCntType> iterationBB(std::vector<GPCntType>(genInstr.nDim()),
  //                                               gpCntPerDim, false);
  base::DataVector nodesPerDimForTG = sg_gen::getNodesPerDimForTG(mi, genInstr, gpCntPerDim);

  // TODO: Delete
  // TensorGrid tg(gpCntPerDim);
  // size_t idx = 0;

  // for (const std::vector<GPCntType>& it : iterationBB) {
  //   for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
  //     const base::DataVector& nodes = nodesPerDimForTG[dim];
  //     tg(idx, dim) = nodes[it[dim]];
  //   }
  //   idx++;
  // }

  return TensorGrid(std::move(gpCntPerDim), std::move(nodesPerDimForTG));
}

namespace sg_gen {

/*
Does include the grid points on the boundary
*/
GPMI getGPCntPerDim(const LvlMI& mi, const SGGenInstr& genInstr) {
  GPMI gpCntPerDim(genInstr.nDim());

  for (size_t dim = 0; dim < gpCntPerDim.size(); dim++) {
    if (mi[dim] < genInstr.getBoundaryLevelOffset()) {  // Should the boundary be included?
      gpCntPerDim[dim] = genInstr.getLvl2GPCntFuncForDim(dim)(mi[dim]);
    } else {
      gpCntPerDim[dim] = genInstr.getLvl2GPCntFuncForDim(dim)(mi[dim]) + 2;
    }
  }

  return gpCntPerDim;
}

base::DataVector getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                     const GPMI& gpCntPerDim) {
  const size_t nDim = genInstr.nDim();

  base::DataVector nodesPerDim(gpCntPerDim.sumOfElems<size_t>());
  size_t vecIdx = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
    const GPCntType nodeCnt = gpCntPerDim[dim];
    const bool includeBoundary = mi[dim] >= genInstr.getBoundaryLevelOffset();

    nodeGenFunc->genNodes(nodeCnt, nodesPerDim, includeBoundary, vecIdx);
    vecIdx += nodeCnt;

    // TODO: Delete
    // if (mi[dim] < genInstr.getBoundaryLevelOffset()) {  // Should the boundary be included?
    //   const size_t innerNodeCnt = gpCntPerDim[dim];
    //   nodeGenFunc nodesPerDim[dim] = lookup.find({nodeGenFunc, innerNodeCnt})->second;
    // } else {
    //   const size_t innerNodeCnt = gpCntPerDim[dim] - 2;
    //   const base::DataVector& cachedNodes = lookup.find({nodeGenFunc, innerNodeCnt})->second;

    //   nodesPerDim[dim] = base::DataVector(gpCntPerDim[dim]);
    //   std::copy(cachedNodes.begin(), cachedNodes.end(), nodesPerDim[dim].begin() + 1);
    //   nodesPerDim[dim][gpCntPerDim[dim] - 1] = static_cast<double>(1);
    // }
  }

  return nodesPerDim;
}

}  // namespace sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp