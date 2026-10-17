#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/delta_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using sgpp::combigrid::misc::DeltaQuadratureContext;
using sgpp::combigrid::misc::FlatNestedVec;

namespace sgpp {
namespace combigrid {

double quadratureDelta(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();
  // TODO: Consider boundary!!!
  const FlatNestedVec<double, 3> weights1D = delta_quadrature_operator::comp1DWeights(sparseGrid);
  const std::vector<NodeIdxLvlMapFunc> nodeIdxLvlMapFuncs =
      delta_quadrature_operator::compNodeIdxLvlMapFuncs(sparseGrid);

  const DeltaQuadratureContext context{.sparseGrid = sparseGrid,
                                       .sourceFunc = sourceFunc,
                                       .domain = genInstr.getDomain(),
                                       .boundaryLvlOffset = genInstr.getBoundaryLevelOffset(),
                                       .weights1D = weights1D,
                                       .nodeIdxLvlMapFuncs = nodeIdxLvlMapFuncs};

  double result = 0.0;

#pragma omp parallel for schedule(guided) reduction(+ : result)
  for (const TensorGridCTData& tgData : sparseGrid) {
    result += delta_quadrature_operator::processDeltaTensorGrid(context, tgData);
  }

  return genInstr.getVolumeOfDomain() * result;
}

/******************
Internal operations
******************/

namespace delta_quadrature_operator {

// TODO: Parallelize
size_t comp1DWeightsSize(const SparseGrid& sparseGrid) {
  const size_t nDim = sparseGrid.nDim();
  const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs = sparseGrid.getGenInstr().getLvl2GPCntFuncs();
  const LvlType boundaryLvlOffset = sparseGrid.getGenInstr().getBoundaryLevelOffset();

  size_t totalSize = 0;

  for (const auto& tgData : sparseGrid) {
    const LvlMI& tgMI = tgData.mi;

    for (size_t dim = 0; dim < nDim; dim++) {
      const LvlType lvl = tgMI[dim];
      totalSize += lvl2GPCntFuncs[dim](lvl) + (lvl >= boundaryLvlOffset ? 2 : 0);
    }
  }

  return totalSize;
}

// TODO: Parallelize
FlatNestedVec<double, 3> comp1DWeights(const SparseGrid& sparseGrid) {
  const size_t nDim = sparseGrid.nDim();
  const size_t nTG = sparseGrid.nTG();
  const LvlType boundaryLvlOffset = sparseGrid.getGenInstr().getBoundaryLevelOffset();

  const std::vector<NodeGenFunc*>& nodeGenFuncs = sparseGrid.getGenInstr().getNodeGenFuncs();
  const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs = sparseGrid.getGenInstr().getLvl2GPCntFuncs();

  std::vector<QuadRule*> quadRules(nDim);

  FlatNestedVec<double, 3> weights;
  weights.reserve(comp1DWeightsSize(sparseGrid));
  weights.reserveSubVecs(1, nTG);
  weights.reserveSubVecs(2, nTG * nDim);

  for (const TensorGridCTData& tgData : sparseGrid) {
    const LvlMI& tgMI = tgData.mi;

    tools::getQuadRulesForTensorGrid(tgData.tensorGrid, nodeGenFuncs, quadRules);
    weights.appendSubVec(1);  // weights[tgIdx]

    for (size_t dim = 0; dim < nDim; dim++) {
      const LvlType lvl = tgMI[dim];
      const GPCntType nNodes = lvl2GPCntFuncs[dim](lvl) + (lvl >= boundaryLvlOffset ? 2 : 0);

      weights.appendSubVec(2, nNodes);  // weights[tgIdx][dim]

      // The sub-vector just appended consists of the last nNodes elements.
      quadRules[dim]->genWeightsInplace(weights.flat().last(nNodes));
    }
  }

  return weights;
}

std::vector<NodeIdxLvlMapFunc> compNodeIdxLvlMapFuncs(const SparseGrid& sparseGrid) {
  const size_t nDim = sparseGrid.nDim();
  const std::vector<NodeGenFunc*>& nodeGenFuncs = sparseGrid.getGenInstr().getNodeGenFuncs();
  const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs = sparseGrid.getGenInstr().getLvl2GPCntFuncs();

  std::vector<NodeIdxLvlMapFunc> nodeIdxLvlMapFuncs(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    nodeIdxLvlMapFuncs[dim] =
        nodeGenFuncs[dim]->getNodeIdxMapFromDeltaTGLvlToRegularLvl(lvl2GPCntFuncs[dim]);
  }

  return nodeIdxLvlMapFuncs;
}

double processDeltaTensorGrid(const DeltaQuadratureContext& context,
                              const TensorGridCTData& deltaTGCTData) {
  const TensorGrid& deltaTG = deltaTGCTData.tensorGrid;

  if (deltaTG.nGP() == 0) {
    return 0;
  }

  double result = 0.0;

  for (auto& [gpMI, gp] : deltaTG.gridPointsMutable()) {
    const double globalWeight = compGlobalWeightForGP(context, deltaTGCTData, gpMI);
    const double funcValue = context.sourceFunc.evaluateNormalizedInPlace(gp, context.domain);

    result += globalWeight * funcValue;
  }

  return result;
}

double compGlobalWeightForGP(const DeltaQuadratureContext& context,
                             const TensorGridCTData& deltaTGCTData, const GPMI& gpMIInDeltaTG) {
  const size_t nTGs = context.sparseGrid.nTG();
  const LvlMI& deltaTGMI = deltaTGCTData.mi;

  double result = 0.0;

  for (size_t tgIdx = 0; tgIdx < nTGs; tgIdx++) {  // Iterate over all (regular) tensor grids
    const TensorGridCTData& tgData = context.sparseGrid.getTensorGrid(tgIdx);

    if (tgData.mi >= deltaTGMI) {  // Only consider tensor grids that contain the grid point
      const double localWeight =
          compLocalWeightForGP(context, deltaTGCTData, gpMIInDeltaTG, tgData.mi, tgIdx);

      result += tgData.coefficient * localWeight;
    }
  }

  return result;
}

double compLocalWeightForGP(const DeltaQuadratureContext& context,
                            const TensorGridCTData& deltaTGCTData, const GPMI& gpMIInDeltaTG,
                            const LvlMI& targetTGMI, const size_t targetTGIdx) {
  const size_t nDim = context.sparseGrid.nDim();
  const LvlMI& deltaTGMI = deltaTGCTData.mi;

  const FlatNestedVec<double, 3>::const_entry_type weights1DForTargetTG =
      context.weights1D[targetTGIdx];

  double result = 1.0;

  for (size_t dim = 0; dim < nDim; dim++) {
    // const GPCntType mappedNodeIdx =
    //     context.nodeIdxLvlMapFuncs[dim](gpMIInDeltaTG[dim], deltaTGMI[dim], targetTGMI[dim]);
    const GPCntType mappedNodeIdx =
        mapNodeIdx(context, deltaTGCTData.tensorGrid.getGPCntPerDim()[dim],
                   static_cast<GPCntType>(weights1DForTargetTG[dim].size()), deltaTGMI[dim],
                   targetTGMI[dim], gpMIInDeltaTG[dim], context.nodeIdxLvlMapFuncs[dim]);

    const double weight1D = weights1DForTargetTG[dim][mappedNodeIdx];
    result *= weight1D;
  }

  return result;
}

GPCntType mapNodeIdx(const DeltaQuadratureContext& context, const GPCntType nNodesInDeltaTG,
                     const GPCntType nNodesInTargetTG, const LvlType deltaTGLvl,
                     const LvlType targetTGLvl, const GPCntType gpIdxInDeltaTG,
                     const NodeIdxLvlMapFunc nodeIdxLvlMapFunc) {
  if (deltaTGLvl == context.boundaryLvlOffset) {  // Has the delta tensor grid boundary nodes?
    if (gpIdxInDeltaTG == 0) {                    // left boundary node
      return 0;
    } else if (gpIdxInDeltaTG == nNodesInDeltaTG - 1) {  // right boundary node
      return nNodesInTargetTG - 1;
    } else {
      // Has the target tensor grid boundary nodes?
      return nodeIdxLvlMapFunc(gpIdxInDeltaTG - 1, deltaTGLvl, targetTGLvl) +
             (targetTGLvl >= context.boundaryLvlOffset ? 1 : 0);
    }
  } else {
    // Has the target tensor grid boundary nodes?
    return nodeIdxLvlMapFunc(gpIdxInDeltaTG, deltaTGLvl, targetTGLvl) +
           (targetTGLvl >= context.boundaryLvlOffset ? 1 : 0);
  }
}

// double compLocalWeightForNonBoundaryGP(const DeltaQuadratureContext& context,
//                             const TensorGridCTData& deltaTGCTData, const GPMI& gpMIInDeltaTG,
//                             const LvlMI& targetTGMI, const size_t targetTGIdx) {
//   const size_t nDim = context.sparseGrid.nDim();
//   const LvlMI& deltaTGMI = deltaTGCTData.mi;

//   const FlatNestedVec<double, 3>::const_entry_type weights1DForTargetTG =
//       context.weights1D[targetTGIdx];

//   double result = 1.0;

//   for (size_t dim = 0; dim < nDim; dim++) {
//     const GPCntType mappedNodeIdx =
//         context.nodeIdxLvlMapFuncs[dim](gpMIInDeltaTG[dim], deltaTGMI[dim], targetTGMI[dim]);

//     const double weight1D = weights1DForTargetTG[dim][mappedNodeIdx];
//     result *= weight1D;
//   }

//   return result;
// }

}  // namespace delta_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp