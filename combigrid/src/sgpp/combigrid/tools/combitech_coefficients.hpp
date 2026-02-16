#ifndef COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP
#define COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP

#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>

namespace sgpp {

namespace combigrid {

namespace tools {

std::vector<int> computeCTCoeffs(const MIVec& miVec);

std::vector<int> computeCTCoeffsNaive(const MIVec& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif