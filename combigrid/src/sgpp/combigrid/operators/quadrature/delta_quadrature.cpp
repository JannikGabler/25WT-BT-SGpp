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
#include <stdexcept>
#include <vector>

using sgpp::combigrid::misc::DeltaQuadratureContext;
using sgpp::combigrid::misc::FlatNestedVec;

namespace sgpp {
namespace combigrid {

double quadratureDelta(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  if (!tools::areNodesOfAllDimsNested(sparseGrid)) {  // TODO: Remove
    throw std::invalid_argument("quadratureDelta: A sparse grid with nested nodes is required.");
  }

  const std::shared_ptr<const SGGenInstr> genInstr = sparseGrid.getGenInstr();
  const FlatNestedVec<double, 3> weights1D = delta_quadrature_operator::comp1DWeights(sparseGrid);

  const DeltaQuadratureContext context{.sparseGrid = sparseGrid,
                                       .sourceFunc = sourceFunc,
                                       .weights1D = weights1D,
                                       .domain = genInstr->getDomain(),
                                       .nodeGenFuncs = genInstr->getNodeGenFuncs(),
                                       .lvl2GPCntFuncs = genInstr->getLvl2GPCntFuncs()};

  double result = 0.0;

#pragma omp parallel for schedule(guided) reduction(+ : result)
  for (const TensorGridCTData& tgData : sparseGrid) {
    result += delta_quadrature_operator::processDeltaTensorGrid(context, tgData);
  }

  return genInstr->getVolumeOfDomain() * result;
}

/******************
Internal operations
******************/

namespace delta_quadrature_operator {

size_t comp1DWeightsSize(const SparseGrid& sparseGrid) {
  const size_t nDim = sparseGrid.nDim();
  const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs = sparseGrid.getGenInstr()->getLvl2GPCntFuncs();

  size_t totalSize = 0;

  for (const auto& tgData : sparseGrid) {
    const LvlMI& tgMI = tgData.mi;

    for (size_t dim = 0; dim < nDim; dim++) {
      totalSize += lvl2GPCntFuncs[dim](tgMI[dim]);
    }
  }

  return totalSize;
}

FlatNestedVec<double, 3> comp1DWeights(const SparseGrid& sparseGrid) {
  const size_t nDim = sparseGrid.nDim();
  const size_t nTG = sparseGrid.nTG();

  const std::vector<NodeGenFunc*>& nodeGenFuncs = sparseGrid.getGenInstr()->getNodeGenFuncs();
  const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs = sparseGrid.getGenInstr()->getLvl2GPCntFuncs();

  std::vector<QuadRule*> quadRules(nDim);

  FlatNestedVec<double, 3> weights;
  weights.reserve(comp1DWeightsSize(sparseGrid));
  weights.reserveSubVecs(1, nTG);
  weights.reserveSubVecs(2, nTG * nDim);

  for (const auto& [idx, tgData] : std::views::enumerate(sparseGrid)) {
    const LvlMI& tgMI = tgData.mi;

    tools::getQuadRulesForTensorGrid(tgData.tensorGrid, nodeGenFuncs, quadRules);
    weights.appendSubVec(1);  // weights[tgIdx]

    for (size_t dim = 0; dim < nDim; dim++) {
      const GPCntType nNodes = lvl2GPCntFuncs[dim](tgMI[dim]);

      weights.appendSubVec(2, nNodes);  // weights[tgIdx][dim]

      // The sub-vector just appended consists of the last nNodes elements.
      quadRules[dim]->genWeightsInplace(weights.flat().last(nNodes));
    }
  }

  return weights;
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
    const NodeGenFunc* nodeGenFunc = context.nodeGenFuncs[dim];
    const Lvl2GPCntFunc& lvl2GPCntFunc = context.lvl2GPCntFuncs[dim];

    const GPCntType transformedNodeIdx = nodeGenFunc->mapNodeIdxFromDeltaTGLevel(
        gpMIInDeltaTG[dim], deltaTGMI[dim], targetTGMI[dim], lvl2GPCntFunc);

    const double weight1D = weights1DForTargetTG[dim][transformedNodeIdx];
    result *= weight1D;
  }

  return result;
}

}  // namespace delta_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp