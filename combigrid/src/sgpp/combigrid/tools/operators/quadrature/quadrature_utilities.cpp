#include <cassert>
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

std::vector<QuadRule*> getQuadRulesForTensorGrid(const TensorGrid& tg,
                                                 const std::vector<NodeGenFunc*>& nodeGenFuncs) {
  std::vector<QuadRule*> result(tg.nDim());

  getQuadRulesForTensorGrid(tg, nodeGenFuncs, result);

  return result;
}

void getQuadRulesForTensorGrid(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                               std::vector<QuadRule*>& out) {
  assert(out.size() == tg.nDim());

  const size_t nDim = tg.nDim();
  const GPMI& gpCntPerDim = tg.getGPCntPerDim();

  for (size_t dim = 0; dim < nDim; dim++) {
    out[dim] = nodeGenFuncs[dim]->getQuadRule(gpCntPerDim[dim]);
  }
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp