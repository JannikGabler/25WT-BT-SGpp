#include <omp.h>
#include <cassert>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_scratch.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  assert(sparseGrid.getGenInstr() != nullptr);

  const std::shared_ptr<const SGGenInstr> genInstr = sparseGrid.getGenInstr();
  const size_t nDim = genInstr->nDim();
  double result = 0;

#pragma omp parallel reduction(+ : result)
  {
    tools::QuadScratch scratch(nDim, sparseGrid.getMaxTGSumOverGPCntsPerDim());

#pragma omp for schedule(guided) nowait
    for (const TensorGridCTData& tgData : sparseGrid) {
      result += tgData.coefficient *
                quadrature_operator::quadrature(sourceFunc, tgData.tensorGrid, *genInstr, scratch);
    }
  }

  return genInstr->getVolumeOfDomain() * result;
}

namespace quadrature_operator {

double quadrature(const SourceFunc& sourceFunc, const TensorGrid& tg, const SGGenInstr& genInstr,
                  tools::QuadScratch& scratch) {
  assert(genInstr.nDim() == tg.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  const HyperCubeArea& bounds = genInstr.getDomain();
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  const std::vector<NodeGenFunc*>& nodeGenFuncs = genInstr.getNodeGenFuncs();
  quadrature_operator::getWeights(tg, nodeGenFuncs, scratch);

  double result = 0;

  for (auto& [gpMI, gp] : tg.gridPointsMutable()) {
    const double weight = quadrature_operator::getWeightForGP(gpMI, gpCntPerDim, scratch);
    const double funcValue = sourceFunc.evaluateNormalizedInPlace(gp, bounds);
    result += weight * funcValue;
  }

  return result;
}

void getWeights(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                tools::QuadScratch& scratch) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  scratch.weights.resize(gpCntPerDim.sumOfElems<size_t>());
  getQuadRules(tg, nodeGenFuncs, scratch);
  // std::vector<base::DataVector> weights(tg.nDim());
  size_t vecOffset = 0;

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    assert(scratch.weights.size() >= vecOffset + gpCntPerDim[dim]);  // TODO: Delete

    const GPCntType nNodes = gpCntPerDim[dim];
    scratch.quadRules[dim]->genWeightsInplace(nNodes, scratch.weights, vecOffset);
    vecOffset += nNodes;
  }
}

double getWeightForGP(const std::vector<GPCntType>& gpMI, const GPMI& gpCntPerDim,
                      const tools::QuadScratch& scratch) {
  double weight = 1;
  size_t vecOffset = 0;

  for (size_t dim = 0; dim < gpMI.size(); dim++) {
    weight *= scratch.weights[vecOffset + gpMI[dim]];
    vecOffset += gpCntPerDim[dim];
  }

  return weight;
}

void getQuadRules(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                  tools::QuadScratch& scratch) {
  assert(scratch.quadRules.size() == tg.nDim());

  const size_t nDim = tg.nDim();
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();

  for (size_t dim = 0; dim < nDim; dim++) {
    scratch.quadRules[dim] = nodeGenFuncs[dim]->getQuadRule(gpCntPerDim[dim]);
  }
}

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp