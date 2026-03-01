// #include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

void populateSG(const SGGenInstr& genInstr, const LvlMIVec& miVec,
                const std::vector<CTCoeffType>& coeffs, SparseGrid& out) {
  assert(out.nTG() >= miVec.nMI());

  const SGGenNodeLookup lookup = genSGNodeLookup(genInstr, miVec, coeffs);

  // #pragma omp parallel for schedule(guided)
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const LvlMI mi = miVec[miIdx];
    const CTCoeffType coeff = coeffs[miIdx];

    TensorGrid tg = genTGForMI(mi, genInstr, lookup);
    out.setTensorGrid(miIdx, {mi, coeff, std::move(tg)});
  }
}

TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr, const SGGenNodeLookup& lookup) {
  const GPMI gpCntPerDim = sg_gen::getGPCntPerDim(mi, genInstr);
  const misc::DiscRectBB<GPCntType> iterationBB = sg_gen::getBBForIteration(gpCntPerDim);
  const std::vector<base::DataVector> nodesPerDimForTG =
      sg_gen::getNodesPerDimForTG(mi, genInstr, gpCntPerDim, lookup);

  TensorGrid tg(gpCntPerDim);
  size_t idx = 0;

  for (const std::vector<GPCntType>& it : iterationBB) {
    for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
      const base::DataVector& nodes = nodesPerDimForTG[dim];
      tg(idx, dim) = nodes[it[dim]];
    }
    idx++;
  }

  return tg;
}

namespace sg_gen {

/*
Does include the grid points on the boundary
*/
GPMI getGPCntPerDim(const LvlMI& mi, const SGGenInstr& genInstr) {
  GPMI gpCntPerDim(genInstr.nDim());

  for (size_t dim = 0; dim < gpCntPerDim.size(); dim++) {
    if (mi[dim] < genInstr.getBoundaryIndexOffset()) {  // Should the boundary be included?
      gpCntPerDim[dim] = genInstr.getLvl2GPCntFuncForDim(dim)(mi[dim]);
    } else {
      gpCntPerDim[dim] = genInstr.getLvl2GPCntFuncForDim(dim)(mi[dim]) + 2;
    }
  }

  return gpCntPerDim;
}

misc::DiscRectBB<GPCntType> getBBForIteration(GPMI gpCntPerDim) {
  for (size_t dim = 0; dim < gpCntPerDim.size(); dim++) {
    gpCntPerDim[dim]--;
  }

  return misc::DiscRectBB<GPCntType>(std::vector<GPCntType>(gpCntPerDim.size()),
                                     std::move(gpCntPerDim));
}

std::vector<base::DataVector> getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                                  const GPMI& gpCntPerDim,
                                                  const SGGenNodeLookup& lookup) {
  std::vector<base::DataVector> nodesPerDim(gpCntPerDim.size());

  for (size_t dim = 0; dim < nodesPerDim.size(); dim++) {
    const NodeGenFunc nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);

    if (mi[dim] < genInstr.getBoundaryIndexOffset()) {  // Should the boundary be included?
      const size_t innerNodeCnt = gpCntPerDim[dim];
      nodesPerDim[dim] = lookup.find({nodeGenFunc, innerNodeCnt})->second;
    } else {
      const size_t innerNodeCnt = gpCntPerDim[dim] - 2;
      const base::DataVector& cachedNodes = lookup.find({nodeGenFunc, innerNodeCnt})->second;

      nodesPerDim[dim] = base::DataVector(gpCntPerDim[dim]);
      std::copy(cachedNodes.begin(), cachedNodes.end(), nodesPerDim[dim].begin() + 1);
      nodesPerDim[dim][gpCntPerDim[dim] - 1] = static_cast<double>(1);
    }
  }

  return nodesPerDim;
}

}  // namespace sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp