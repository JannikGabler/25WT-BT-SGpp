#ifndef COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP
#define COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

std::vector<int> computeCTCoeffs(const LvlMIVec& miVec);

std::vector<int> computeCTCoeffsNaive(const LvlMIVec& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif