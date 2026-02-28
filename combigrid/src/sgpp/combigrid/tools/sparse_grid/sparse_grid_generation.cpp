#include <algorithm>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>
#include "sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

SparseGrid genSG(const SGGenInstr& genInstr) {
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> p = genInstr.genMIVecWithCoeff();
  const SGGenNodeLookup lookup = genSGNodeLookup(genInstr, p.first, p.second);

#pragma omp parallel for  // TODO: schedule
  for (size_t miIdx = 0; miIdx < p.first.nMI(); miIdx++) {
    const LvlMI mi = p.first[miIdx];
    const TensorGrid tg = genTGForMI(mi, genInstr, lookup);
  }
}

TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr, const SGGenNodeLookup& lookup) {
  const std::vector<size_t> gpCntPerDim = sg_gen::getGPCntPerDim(mi, genInstr);
  const misc::DiscRectBB<size_t> iterationBB = sg_gen::getBBForIteration(gpCntPerDim);
  const std::vector<base::DataVector> nodesPerDimForTG =
      sg_gen::getNodesPerDimForTG(mi, genInstr, gpCntPerDim, lookup);

  // TensorGrid tg(gpCntPerDim);  // TODO
  // size_t idx = 0;

  // for (const std::vector<size_t>& it : iterationBB) {
  //   for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
  //     const base::DataVector& nodes = nodesPerDimForTG[dim];
  //     tg(idx, dim) = nodes[it[dim]];
  //   }
  //   idx++;
  // }
}

namespace sg_gen {

/*
Does include the grid points on the boundary
*/
std::vector<size_t> getGPCntPerDim(const LvlMI& mi, const SGGenInstr& genInstr) {
  std::vector<size_t> gpCntPerDim(genInstr.nDim());

  for (size_t dim = 0; dim < gpCntPerDim.size(); dim++) {
    if (mi[dim] < genInstr.getBoundaryIndexOffset()) {  // Should the boundary be included?
      gpCntPerDim[dim] = genInstr.getLvl2GPCntFuncForDim(dim)(mi[dim]);
    } else {
      gpCntPerDim[dim] = genInstr.getLvl2GPCntFuncForDim(dim)(mi[dim]) + 2;
    }
  }

  return gpCntPerDim;
}

misc::DiscRectBB<size_t> getBBForIteration(const std::vector<size_t>& gpCntPerDim) {
  return misc::DiscRectBB<size_t>(std::vector<size_t>(gpCntPerDim.size()), gpCntPerDim);
}

std::vector<base::DataVector> getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                                  const std::vector<size_t>& gpCntPerDim,
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