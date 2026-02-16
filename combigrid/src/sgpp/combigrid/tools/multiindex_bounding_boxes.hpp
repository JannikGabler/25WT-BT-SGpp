#ifndef COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP

#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>
#include "sgpp/combigrid/multiindices/multiindex.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

std::vector<MIType> genRectMIBoundingBox(const std::vector<MI>& mi);
std::vector<MIType> genRectMIBoundingBox(const MIVec& miVec);
std::vector<MIType> genRectMIBoundingBox(const MIVec& miVec, const std::vector<size_t>& idx);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
