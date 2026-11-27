// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {

GPCntType getDeltaNodeCnt(LvlType lvl, Lvl2GPCntFunc lvl2GPCntFunc, bool includeNodesOnBoundary) {
  return lvl2GPCntFunc(lvl) - (lvl > 0 ? lvl2GPCntFunc(lvl - 1) : 0) +
         (includeNodesOnBoundary ? 2 : 0);
}

/// @brief Internal implementation details of the deltas-sparse-grid generation.
namespace deltas_sparse_grid_generation {

std::vector<NodeIdxLvlMapFunc> compNodeIdxLvlMapFuncs(const DeltasSparseGrid& deltasSparseGrid) {
  const size_t nDim = deltasSparseGrid.nDim();
  const std::vector<NodeGenFunc*>& nodeGenFuncs = deltasSparseGrid.getSetup().getNodeGenFuncs();
  const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs =
      deltasSparseGrid.getSetup().getLvl2GPCntFuncs();

  std::vector<NodeIdxLvlMapFunc> nodeIdxLvlMapFuncs(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    nodeIdxLvlMapFuncs[dim] =
        nodeGenFuncs[dim]->getNodeIdxMapFromDeltaTGLvlToRegularLvl(lvl2GPCntFuncs[dim]);
  }

  return nodeIdxLvlMapFuncs;
}

GPCntType mapDeltaNodeIdxToRegular(const GPCntType nNodesInDeltaTG,
                                   const GPCntType nNodesInTargetTG, const LvlType deltaTGLvl,
                                   const LvlType targetTGLvl, const GPCntType gpIdxInDeltaTG,
                                   const NodeIdxLvlMapFunc nodeIdxLvlMapFunc,
                                   const LvlType boundaryLvlOffset) {
  if (deltaTGLvl == boundaryLvlOffset) {  // Has the delta tensor grid boundary nodes?
    if (gpIdxInDeltaTG == 0) {            // left boundary node
      return 0;
    } else if (gpIdxInDeltaTG == nNodesInDeltaTG - 1) {  // right boundary node
      return nNodesInTargetTG - 1;
    } else {
      // Has the target tensor grid boundary nodes?
      return nodeIdxLvlMapFunc(gpIdxInDeltaTG - 1, deltaTGLvl, targetTGLvl) +
             (targetTGLvl >= boundaryLvlOffset ? 1 : 0);
    }
  } else {
    // Has the target tensor grid boundary nodes?
    return nodeIdxLvlMapFunc(gpIdxInDeltaTG, deltaTGLvl, targetTGLvl) +
           (targetTGLvl >= boundaryLvlOffset ? 1 : 0);
  }
}

}  // namespace deltas_sparse_grid_generation

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
