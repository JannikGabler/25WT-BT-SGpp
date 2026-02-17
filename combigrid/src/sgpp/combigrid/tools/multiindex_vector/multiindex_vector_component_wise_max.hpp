#ifndef COMBIGRID_TOOLS_MULTIINDEX_VECTOR_COMPONENT_WISE_MAX_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_VECTOR_COMPONENT_WISE_MAX_HPP

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

MI computeComponentWiseMax(const MIVec& miVec);

MI computeComponentWiseMaxSerial(const MIVec& miVec);

MI computeComponentWiseMaxParallel(const MIVec& miVec);

MI mergeComponentWiseMax(const size_t nDim, const std::vector<MI>& localMax);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif