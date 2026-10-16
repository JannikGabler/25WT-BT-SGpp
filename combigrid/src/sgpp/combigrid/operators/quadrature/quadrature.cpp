#include <omp.h>
#include <cassert>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using sgpp::combigrid::misc::QuadratureContext;
using sgpp::combigrid::misc::QuadratureScratch;

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  assert(sparseGrid.getGenInstr() != nullptr);

  const std::shared_ptr<const SGGenInstr> genInstr = sparseGrid.getGenInstr();
  const size_t nDim = sparseGrid.nDim();

  const QuadratureContext context{.sparseGrid = sparseGrid,
                                  .sourceFunc = sourceFunc,
                                  .domain = genInstr->getDomain(),
                                  .nodeGenFuncs = genInstr->getNodeGenFuncs()};

  double result = 0;

#pragma omp parallel reduction(+ : result)
  {
    QuadratureScratch scratch(nDim, sparseGrid.getMaxTGSumOverGPCntsPerDim());

#pragma omp for schedule(guided) nowait
    for (const TensorGridCTData& tgData : sparseGrid) {
      result += tgData.coefficient *
                quadrature_operator::processTensorGrid(context, tgData.tensorGrid, scratch);
    }
  }

  return genInstr->getVolumeOfDomain() * result;
}

namespace quadrature_operator {

// TODO: Change argument structure (see delta quadrature)
double processTensorGrid(const QuadratureContext& context, const TensorGrid& tg,
                         QuadratureScratch& scratch) {
  assert(tg.nDim() == context.sparseGrid.nDim());

  if (tg.nGP() == 0) {
    return 0;
  }

  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  getWeights(context, tg, scratch);

  double result = 0;

  for (auto& [gpMI, gp] : tg.gridPointsMutable()) {
    const double weight = getWeightForGP(gpMI, gpCntPerDim, scratch);
    const double funcValue = context.sourceFunc.evaluateNormalizedInPlace(gp, context.domain);
    result += weight * funcValue;
  }

  return result;
}

void getWeights(const QuadratureContext& context, const TensorGrid& tg,
                QuadratureScratch& scratch) {
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();
  scratch.weights1D.resize(gpCntPerDim.sumOfElems<size_t>());
  tools::getQuadRulesForTensorGrid(tg, context.nodeGenFuncs, scratch.quadRules);

  size_t vecOffset = 0;

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    assert(scratch.weights1D.size() >= vecOffset + gpCntPerDim[dim]);  // TODO: Delete

    const GPCntType nNodes = gpCntPerDim[dim];
    scratch.quadRules[dim]->genWeightsInplace(
        std::span<double>(scratch.weights1D).subspan(vecOffset, nNodes));
    vecOffset += nNodes;
  }
}

double getWeightForGP(const std::vector<GPCntType>& gpMI, const GPMI& gpCntPerDim,
                      const QuadratureScratch& scratch) {
  double weight = 1;
  size_t vecOffset = 0;

  for (size_t dim = 0; dim < gpMI.size(); dim++) {
    weight *= scratch.weights1D[vecOffset + gpMI[dim]];
    vecOffset += gpCntPerDim[dim];
  }

  return weight;
}

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp