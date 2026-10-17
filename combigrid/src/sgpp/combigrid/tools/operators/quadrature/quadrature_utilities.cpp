#pragma once

#include <cassert>
#include <memory>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

// /**
//  * TODO: Document
//  */
// void mapGPMIToDifferentLevel(const GPMI& originalGPMI, const LvlMI& currentLvlMI,
//                              const LvlMI& targetLvlMI,
//                              const std::vector<NodeGenFunc*>& nodeGenFuncs, GPMI& out) {
//   assert(originalGPMI.size() == currentLvlMI.size() && currentLvlMI.size() == targetLvlMI.size()
//   &&
//          targetLvlMI.size() == out.size());

//   const size_t nDim = originalGPMI.size();

//   for (size_t dim = 0; dim < nDim; dim++) {
//     const NodeGenFunc* nodeGenFunc = nodeGenFuncs[dim];
//     out[dim] = nodeGenFunc->mapNodeIndexToDifferentLvl(originalGPMI[dim], currentLvlMI[dim],
//                                                        targetLvlMI[dim]);
//   }
// }

std::vector<QuadRule*> getQuadRulesForTensorGrid(const TensorGrid& tg,
                                                 const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  std::vector<QuadRule*> result(tg.nDim());

  getQuadRulesForTensorGrid(tg, nodeGenFuncs, result);

  return result;
}

void getQuadRulesForTensorGrid(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                               std::vector<QuadRule*>& out) {
  assert(tg.nDim() == nodeGenFuncs.size());
  assert(tg.nDim() == out.size());

  getQuadRulesForAllDims(tg.getGPCntPerDim(), nodeGenFuncs, out);
}

std::vector<QuadRule*> getQuadRulesForAllDims(const std::span<const GPCntType> gpCntPerDim,
                                              const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  std::vector<QuadRule*> result(gpCntPerDim.size());

  getQuadRulesForAllDims(gpCntPerDim, nodeGenFuncs, result);

  return result;
}

void getQuadRulesForAllDims(const std::span<const GPCntType> gpCntPerDim,
                            const std::vector<NodeGenFunc*>& nodeGenFuncs,
                            std::vector<QuadRule*>& out) {
  assert(gpCntPerDim.size() == nodeGenFuncs.size());
  assert(gpCntPerDim.size() == out.size());

  const size_t nDim = gpCntPerDim.size();

  for (size_t dim = 0; dim < nDim; dim++) {
    out[dim] = nodeGenFuncs[dim]->getQuadRule(gpCntPerDim[dim]);
  }
}

// /*
// TODO: Document
// Computes the quadrature weight of a single grid point directly.
// */
// double genQuadratureWeightOfSingleGP(const TensorGrid& tg, const GPMI& gpMI,
//                                      const std::vector<NodeGenFunc*>& nodeGenFuncs) {
//   assert(tg.nDim() == nodeGenFuncs.size());

//   const size_t nDim = tg.nDim();
//   const GPMI& gpCntPerDim = tg.getGPCntPerDim();

//   QuadRule* quadRule;
//   double result = 1.0;

//   for (size_t dim = 0; dim < nDim; dim++) {
//     quadRule = nodeGenFuncs[dim]->getQuadRule(gpCntPerDim[dim]);
//     result *= quadRule->genSingleWeight(gpMI[dim], gpCntPerDim[dim]);
//   }

//   return result;
// }

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp