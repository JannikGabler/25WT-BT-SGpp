#include <sgpp/combigrid/tools/functions/node_generation_functions/equidistant_node_generation_function_utilities.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace equidistant_node_gen_function {

GPCntType mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction(const GPCntType nodeIdx,
                                                            const LvlType originLvl,
                                                            const LvlType targetLvl) {
  assert(targetLvl >= originLvl);  // TODO: Maybe support deltaTGLvl < targetLvl?

  const GPCntType pow = GPCntType{1} << (targetLvl - originLvl);  // 2^{target-Lvl - originLvl}
  return pow * nodeIdx + pow - 1;
}

GPCntType mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction(
    const GPCntType nodeIdxInDeltaTG, const LvlType deltaTGLvl, const LvlType targetLvl) {
  const GPCntType nodeIdxOnRegularLvl = 2 * nodeIdxInDeltaTG;

  return mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction(nodeIdxOnRegularLvl, deltaTGLvl,
                                                           targetLvl);
}

}  // namespace equidistant_node_gen_function
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp