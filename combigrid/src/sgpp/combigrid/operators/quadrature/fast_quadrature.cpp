#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/fast_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_utilities.hpp>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

double quadratureFast(const SourceFunc& sourceFunc, const ExtensiveSparseGrid& sparseGrid) {
  if (!tools::areNodesOfAllDimsNested(sparseGrid)) {
    throw std::invalid_argument("quadratureFast: A sparse grid with nested nodes is required.");
  }

  double result = 0.0;

  const std::span<const TensorGridCTData> gridsWithCoeffZero =
      sparseGrid.getTensorGridsWithCoeffZero();
  const std::span<const TensorGridCTData> gridsWithCoeffNonZero = sparseGrid.getTensorGrids();
  const std::size_t nGridsWithCoeffZero = gridsWithCoeffZero.size();
  const std::size_t nTotal = nGridsWithCoeffZero + gridsWithCoeffNonZero.size();

#pragma omp parallel for reduction(+ : result) schedule(guided)
  for (std::size_t i = 0; i < nTotal; ++i) {
    const TensorGridCTData& tgData = (i < nGridsWithCoeffZero)
                                         ? gridsWithCoeffZero[i]
                                         : gridsWithCoeffNonZero[i - nGridsWithCoeffZero];
    result += fast_quadrature_operator::processTensorGrid(sourceFunc, sparseGrid, tgData);
  }

  // #pragma omp parallel
  //   {
  // #pragma omp for reduction(+ : result) schedule(guided)
  //     for (const TensorGridCTData& tgData : sparseGrid.getTen()) {
  //       result += fast_quadrature_operator::processTensorGrid(sourceFunc, sparseGrid, tgData);
  //     }
  // #pragma omp for reduction(+ : result) schedule(guided)
  //     for (const TensorGridCTData& tgData : sparseGrid.getTensorGridsWithCoeffZero()) {
  //       result += fast_quadrature_operator::processTensorGrid(sourceFunc, sparseGrid, tgData);
  //     }
  //   }

  return sparseGrid.getGenInstr()->getVolumeOfDomain() * result;
}

/******************
Internal operations
******************/

namespace fast_quadrature_operator {

double processTensorGrid(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                         const TensorGridCTData& tgData) {
  assert(sparseGrid.nDim() == tgData.tensorGrid.nDim());

  const size_t nGP = tgData.tensorGrid.nGP();
  const size_t nDim = tgData.tensorGrid.nDim();

  if (nGP == 0) {
    return 0;
  }

  const HyperCubeArea& bounds = sparseGrid.getGenInstr()->getDomain();
  const GPMI& gpCntPerDim = tgData.tensorGrid.getGPCntPerDim();
  const std::vector<NodeGenFunc*>& nodeGenFuncs = sparseGrid.getGenInstr()->getNodeGenFuncs();
  const misc::DiscRectBB<GPCntType> iterationBB(GPMI(nDim), gpCntPerDim, false);

  double result = 0.0;
  std::vector<QuadRule*> quadRules(nDim);
  GPMI gpMIOnHigherLvl(nDim);
  base::DataVector gp;

  for (const GPMI& gpMIOnLowerLvl : iterationBB) {
    tgData.tensorGrid.getGridPoint(gpMIOnLowerLvl, gp);
    double globalWeight = 0.0;

    for (const TensorGridCTData& tgDataOther : sparseGrid) {
      if (tgData.mi <= tgDataOther.mi) {
        tools::mapGPMIToDifferentLevel(gpMIOnLowerLvl, tgData.mi, tgDataOther.mi, gpMIOnHigherLvl);
        tools::getQuadRulesForTensorGrid(tgDataOther.tensorGrid, nodeGenFuncs, quadRules);

        const double localQuadWeight = tools::genQuadratureWeightOfSingleGP(
            tgDataOther.tensorGrid, gpMIOnHigherLvl, nodeGenFuncs);

        globalWeight += tgDataOther.coefficient * localQuadWeight;
      }
    }

    const double funcValue = sourceFunc.evaluateNormalizedInPlace(gp, bounds);
    result += globalWeight * funcValue;
  }

  return result;
}

}  // namespace fast_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp