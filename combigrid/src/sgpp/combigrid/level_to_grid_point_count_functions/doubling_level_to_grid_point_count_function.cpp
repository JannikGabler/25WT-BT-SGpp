#include <sgpp/combigrid/tools/math/power.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

GPCntType linearLvl2GPCntFunction(const MIType lvl) {
  return tools::pow(static_cast<GPCntType>(2), static_cast<GPCntType>(lvl + 1)) - 1;
}

}  // namespace combigrid
}  // namespace sgpp
