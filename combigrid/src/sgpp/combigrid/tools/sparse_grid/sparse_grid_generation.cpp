#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>
#include <vector>

using namespace sgpp::combigrid::tools::sparse_grid_generation;

namespace sgpp {
namespace combigrid {

namespace tools {

void populateSG(SparseGrid& sg, const SGGenInstr& genInstr, const LvlMIVec& miVec,
                const std::vector<CTCoeffType>& coeffs) {
  assert(sg.nDim() == genInstr.nDim() && sg.nDim() == miVec.nDim());
  assert(miVec.nMI() == coeffs.size());

  std::vector<TensorGridCTData> tgData = genTGData(genInstr, miVec.getView(), coeffs);

  sg.setTensorGrids(std::move(tgData));
}

/// @brief Internal implementation details of the sparse-grid generation.
namespace sparse_grid_generation {

std::vector<TensorGridCTData> genTGData(const SGGenInstr& genInstr, const LvlMIVecView mis,
                                        const std::span<const CTCoeffType> coeffs) {
  assert(mis.size() == coeffs.size());

  std::vector<TensorGridCTData> tgData(mis.size());

#pragma omp parallel for schedule(guided)  // TODO: Schedule (benchmark)
  for (size_t miIdx = 0; miIdx < mis.size(); miIdx++) {
    LvlMI mi = mis[miIdx];

    TensorGrid tg = genTGForMI(mi, genInstr);

    tgData[miIdx] = {std::move(mi), coeffs[miIdx], std::move(tg)};
  }

  return tgData;
}

TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr) {
  GPMI gpCntPerDim = getGPCntPerDim(mi, genInstr);

  base::DataVector nodesPerDimForTG = getNodesPerDimForTG(mi, genInstr, gpCntPerDim);

  return TensorGrid(std::move(gpCntPerDim), std::move(nodesPerDimForTG));
}

// Does include the grid points on the boundary
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
  }

  return nodesPerDim;
}

}  // namespace sparse_grid_generation

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp