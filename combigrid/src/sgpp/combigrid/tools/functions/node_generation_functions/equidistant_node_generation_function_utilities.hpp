/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace equidistant_node_gen_function {

/**
 * TODO: Document
 */
GPCntType mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction(GPCntType nodeIdx, LvlType originLvl,
                                                            LvlType targetLvl);

/**
 * TODO: Document
 */
GPCntType mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction(
    GPCntType nodeIdxInDeltaTG, LvlType deltaTGLvl, LvlType targetLvl);

}  // namespace equidistant_node_gen_function
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp