#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>
#include <vector>

using namespace sgpp::combigrid::tools::deltas_sparse_grid_generation;

namespace sgpp {
namespace combigrid {

namespace tools {

GPCntType getDeltaNodeCnt(LvlType lvl, Lvl2GPCntFunc lvl2GPCntFunc, bool includeNodesOnBoundary) {
  return lvl2GPCntFunc(lvl) + (lvl > 0 ? lvl2GPCntFunc(lvl - 1) : 0) +
         (includeNodesOnBoundary ? 2 : 0);
}

/// @brief Internal implementation details of the deltas-sparse-grid generation.
namespace deltas_sparse_grid_generation {

std::vector<TensorGridCTData> genTGData(const SGGenInstr& genInstr, const LvlMIVecView mis,
                                        const std::span<const CTCoeffType> coeffs) {
  assert(mis.nDim() == genInstr.nDim());
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
    const LvlType lvl = mi[dim];

    const bool includeNodesOnBoundary = (lvl == genInstr.getBoundaryLevelOffset());

    gpCntPerDim[dim] =
        getDeltaNodeCnt(lvl, genInstr.getLvl2GPCntFuncForDim(dim), includeNodesOnBoundary);
  }

  return gpCntPerDim;
}

base::DataVector getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                     const GPMI& gpCntPerDim) {
  const size_t nDim = genInstr.nDim();
  const LvlType boundaryLvlOffset = genInstr.getBoundaryLevelOffset();

  base::DataVector nodesPerDim(gpCntPerDim.sumOfElems<size_t>());
  size_t writeIdx = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const LvlType lvl = mi[dim];
    const NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
    const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
    const GPCntType nodeCnt = gpCntPerDim[dim];
    const bool includeBoundary = (lvl == boundaryLvlOffset);

    nodeGenFunc->genDeltaNodes(lvl, lvl2GPCntFunc,
                               std::span(nodesPerDim).subspan(writeIdx, nodeCnt), includeBoundary);

    writeIdx += nodeCnt;
  }

  return nodesPerDim;
}

}  // namespace deltas_sparse_grid_generation

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp