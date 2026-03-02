#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

double quadrature(const SparseGrid& sparseGrid, const SourceFunc sourceFunc) {
  const std::vector<NodeGenFunc>& nodeGenFuncs = sparseGrid.getNodeGenFuncs();
  double result = 0;

  // #pragma omp parallel for reduction(+:result)
  for (const TensorGridCTData& tgData : sparseGrid) {
    result += tgData.coefficient * quadrature(tgData.tensorGrid, nodeGenFuncs, sourceFunc);
  }

  return result;
}

double quadrature(const TensorGrid& tg, const std::vector<NodeGenFunc>& nodeGenFuncs,
                  const SourceFunc sourceFunc) {
  assert(nodeGenFuncs.size() == tg.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  const misc::DiscRectBB<GPCntType> iterationBB(std::vector<GPCntType>(tg.nDim()), gpCntPerDim,
                                                false);
  const std::vector<base::DataVector> weights = quadrature_operator::getWeights(tg, nodeGenFuncs);
  size_t gpIdx = 0;
  double result = 0;

  for (const std::vector<GPCntType>& gpMI : iterationBB) {
    const double weight = quadrature_operator::getWeightForGP(gpMI, weights);
    const base::DataVector gp = tg[gpIdx++];
    const double funcValue = sourceFunc(gp);  // TODO: Cache
    result += weight * funcValue;
  }

  return result;
}

namespace quadrature_operator {

double getWeightForGP(const std::vector<GPCntType>& gpMI,
                      const std::vector<base::DataVector>& weights) {
  double weight = 1;

  for (size_t dim = 0; dim < gpMI.size(); dim++) {
    weight *= weights[dim][gpMI[dim]];
  }

  return weight;
}

std::vector<base::DataVector> getWeights(const TensorGrid& tg,
                                         const std::vector<NodeGenFunc>& nodeGenFuncs) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  const std::vector<QuadRule> quadRules = getQuadRules(tg, nodeGenFuncs);

  quadRules[0].getWeights(5);
}

std::vector<QuadRule> getQuadRules(const TensorGrid& tg,
                                   const std::vector<NodeGenFunc>& nodeGenFuncs) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  std::vector<QuadRule> quadRules;
  quadRules.reserve(nodeGenFuncs.size());

  for (size_t dim = 0; dim < nodeGenFuncs.size(); dim++) {
    quadRules.emplace_back(nodeGenFuncs[dim].getQuadRule(gpCntPerDim[dim]));
  }

  return quadRules;
}

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp