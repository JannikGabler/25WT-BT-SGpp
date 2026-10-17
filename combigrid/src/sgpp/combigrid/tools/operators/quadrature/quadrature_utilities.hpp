#pragma once

#include <cassert>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {

/**
TODO: Document
*/
void mapGPMIToDifferentLevel(const GPMI& originalGPMI, const LvlMI& currentLvlMI,
                             const LvlMI& targetLvlMI, GPMI& out);

/**
 * TODO: Document
 */
std::vector<QuadRule*> getQuadRulesForTensorGrid(const TensorGrid& tg,
                                                 const std::vector<NodeGenFunc*>& nodeGenFuncs);

/**
TODO: Document
*/
void getQuadRulesForTensorGrid(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                               std::vector<QuadRule*>& out);

/**
 * TODO: Document
 */
std::vector<QuadRule*> getQuadRulesForAllDims(std::span<const GPCntType> gpCntPerDim,
                                              const std::vector<NodeGenFunc*>& nodeGenFuncs);

/**
 * TODO: Document
 */
void getQuadRulesForAllDims(std::span<const GPCntType> gpCntPerDim,
                            const std::vector<NodeGenFunc*>& nodeGenFuncs,
                            std::vector<QuadRule*>& out);

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp