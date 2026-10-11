#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/delta_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

struct DeltaQuadratureData {  // TODO: Move
  std::vector<NodeGenFunc*>& nodeGenFuncs;
  std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs;

  // weights
};

// TODO: Change arguments
double quadratureDelta(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  if (!tools::areNodesOfAllDimsNested(sparseGrid)) {  // TODO: Remove
    throw std::invalid_argument("quadratureDelta: A sparse grid with nested nodes is required.");
  }

  const std::shared_ptr<const SGGenInstr> genInstr = sparseGrid.getGenInstr();
  const size_t nDeltaTG = sparseGrid.nTG();
  double result = 0.0;

#pragma omp parallel for reduction(+ : result) schedule(guided)
  for (const TensorGridCTData& tgData : sparseGrid) {
    result +=
        delta_quadrature_operator::processDeltaTensorGrid(sourceFunc, deltaTG, *genInstr, scratch);
  }

  return sparseGrid.getGenInstr()->getVolumeOfDomain() * result;
}

/******************
Internal operations
******************/

namespace delta_quadrature_operator {

double processDeltaTensorGrid(const SourceFunc& sourceFunc, const TensorGrid& deltaTG,
                              const SGGenInstr& genInstr) {
  const size_t nGP = deltaTG.nGP();
  const size_t nDim = deltaTG.nDim();

  if (nGP == 0) {
    return 0;
  }

  const HyperCubeArea& bounds = genInstr.getDomain();
  const GPMI& gpCntPerDim = deltaTG.getGPCntPerDim();
  const std::vector<NodeGenFunc*>& nodeGenFuncs = genInstr.getNodeGenFuncs();
  const misc::DiscRectBB<GPCntType> iterationBB(GPMI(nDim), gpCntPerDim, false);

  double result = 0.0;

  for (const GPMI& gpMI : iterationBB) {
    deltaTG.getGridPoint(gpMI, scratch.gp);
    const double weight = quadrature_operator::getWeightForGP(gpMI, gpCntPerDim, scratch);
    const double funcValue = sourceFunc.evaluateNormalizedInPlace(scratch.gp, bounds);
    result += weight * funcValue;
  }

  return result;
}

double compGlobalWeightForGP(const SparseGrid& sparseGrid, const GPMI& gpMIInDeltaTG,
                             const LvlMI& deltaTGMI,
                             const std::vector<std::vector<base::DataVector>>& weights1D,
                             const LvlMI& targetTGMI, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                             const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs) {
  const size_t nTGs = sparseGrid.nTG();

  double result = 0.0;

  for (size_t tgIdx = 0; tgIdx < nTGs; tgIdx++) {  // Iterate over all (regular) tensor grids
    const TensorGridCTData& tgData = sparseGrid.getTensorGrid(tgIdx);

    if (tgData.mi >= deltaTGMI) {  // Only consider tensor grids that contain the grid point
      const double localWeight = compLocalWeightForGP(
          gpMIInDeltaTG, deltaTGMI, tgData.mi, nodeGenFuncs, lvl2GPCntFuncs, weights1D[tgIdx]);
      result += tgData.coefficient * localWeight;
    }
  }

  return result;
}

// TODO: Check arguments
double compLocalWeightForGP(const GPMI& gpMIInDeltaTG, const LvlMI& deltaTGMI,
                            const LvlMI& targetTGMI, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                            const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs,
                            const std::vector<base::DataVector>& weights1DForTG) {
  const size_t nDim = gpMIInDeltaTG.size();
  double result = 1.0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const NodeGenFunc* nodeGenFunc = nodeGenFuncs[dim];
    const Lvl2GPCntFunc& lvl2GPCntFunc = lvl2GPCntFuncs[dim];

    const GPCntType transformedNodeIdx = nodeGenFunc->mapNodeIdxFromDeltaTGLevel(
        gpMIInDeltaTG[dim], deltaTGMI[dim], targetTGMI[dim], lvl2GPCntFunc);

    const double weight1D = weights1DForTG[dim][transformedNodeIdx];  // TODO
    result *= weight1D;
  }

  return result;
}

}  // namespace delta_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp