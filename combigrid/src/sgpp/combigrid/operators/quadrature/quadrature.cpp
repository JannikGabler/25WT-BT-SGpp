#include <cassert>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  assert(sparseGrid.getGenInstr() != nullptr);

  const std::shared_ptr<const SGGenInstr> genInstr = sparseGrid.getGenInstr();
  double result = 0;

  // #pragma omp parallel for reduction(+:result) // TODO
  for (const TensorGridCTData& tgData : sparseGrid) {
    result += tgData.coefficient *
              quadrature_operator::quadrature(sourceFunc, tgData.tensorGrid, *genInstr);
  }

  return genInstr->getVolumeOfBounds() * result;
}

namespace quadrature_operator {

double quadrature(const SourceFunc& sourceFunc, const TensorGrid& tg, const SGGenInstr& genInstr) {
  assert(genInstr.nDim() == tg.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  const HyperCubeArea bounds = genInstr.getBounds();
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  const std::vector<NodeGenFunc*>& nodeGenFuncs = genInstr.getNodeGenFuncs();
  const misc::DiscRectBB<GPCntType> iterationBB(std::vector<GPCntType>(tg.nDim()), gpCntPerDim,
                                                false);
  const std::vector<base::DataVector> weights = quadrature_operator::getWeights(tg, nodeGenFuncs);
  size_t gpIdx = 0;
  double result = 0;

  for (const std::vector<GPCntType>& gpMI : iterationBB) {
    const double weight = quadrature_operator::getWeightForGP(gpMI, weights);
    const base::DataVector gp = tg[gpIdx++];
    const double funcValue = sourceFunc.evaluateNormalized(gp, bounds);
    result += weight * funcValue;
  }

  return result;
}

std::vector<base::DataVector> getWeights(const TensorGrid& tg,
                                         const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  const std::vector<QuadRule*> quadRules = getQuadRules(tg, nodeGenFuncs);
  std::vector<base::DataVector> weights(tg.nDim());

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    const GPCntType nNodes = gpCntPerDim[dim];
    weights[dim] = quadRules[dim]->getWeights(nNodes);
  }

  return weights;
}

double getWeightForGP(const std::vector<GPCntType>& gpMI,
                      const std::vector<base::DataVector>& weights) {
  double weight = 1;

  for (size_t dim = 0; dim < gpMI.size(); dim++) {
    weight *= weights[dim][gpMI[dim]];
  }

  return weight;
}

std::vector<QuadRule*> getQuadRules(const TensorGrid& tg,
                                    const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  std::vector<QuadRule*> quadRules(nodeGenFuncs.size());

  for (size_t dim = 0; dim < nodeGenFuncs.size(); dim++) {
    quadRules[dim] = nodeGenFuncs[dim]->getQuadRule(gpCntPerDim[dim]);
  }

  return quadRules;
}

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp