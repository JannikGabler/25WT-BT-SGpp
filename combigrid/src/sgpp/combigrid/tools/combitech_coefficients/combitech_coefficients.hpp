#ifndef COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP
#define COMBIGRID_TOOLS_COMBITECH_COEFFICIENTS_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/*
General algorithm to compute the combination technique coefficients for a set of multiindices.
The miVec must be downwards-closed.
This algorithm has a runtime in O(|miVec| * nDim).
Parallel computation.
*/
std::vector<CTCoeffType> computeCTCoeffs(const LvlMIVec& miVec);

/*
Naive approach to compute the combination technique coefficients.
Should only be chosen if necessary.
Parallel computation.
*/
std::vector<CTCoeffType> computeCTCoeffsNaive(const LvlMIVec& miVec);

/*
Computes the combination technique coefficient for a single multiindex.
This algorithm has a runtime in O(2^nDim).
Serial computation.
*/
CTCoeffType computeCTCoeffSingle(const LvlMI& mi, const LvlMIVec& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif