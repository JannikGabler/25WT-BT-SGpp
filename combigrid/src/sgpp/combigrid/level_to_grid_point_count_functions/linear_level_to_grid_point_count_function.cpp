#include <sgpp/combigrid/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

GPCntType linearLvl2GPCntFunction(const LvlType lvl) { return lvl + 1; }

}  // namespace combigrid
}  // namespace sgpp
