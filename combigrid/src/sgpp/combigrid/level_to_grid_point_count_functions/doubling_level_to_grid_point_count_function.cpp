#include <sgpp/combigrid/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

GPCntType doublingLvl2GPCntFunction(const MIType lvl) {
  return tools::pow(static_cast<GPCntType>(2), static_cast<GPCntType>(lvl + 1)) - 1;
}

}  // namespace combigrid
}  // namespace sgpp
