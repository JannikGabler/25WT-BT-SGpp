#ifndef COMBIGRID_LEVEL_TO_GRID_POINTS_COUNT_FUNCTIONS_HPP
#define COMBIGRID_LEVEL_TO_GRID_POINTS_COUNT_FUNCTIONS_HPP

#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

GPCntType linearLvl2GPCntFunction(LvlType lvl);

GPCntType doublingLvl2GPCntFunction(LvlType lvl);

}  // namespace combigrid
}  // namespace sgpp

#endif