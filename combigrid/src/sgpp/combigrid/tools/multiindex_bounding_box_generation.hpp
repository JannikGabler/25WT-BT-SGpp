#ifndef COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_BOUNDING_BOXES_HPP

#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include "sgpp/combigrid/multiindices/multiindex.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

// misc::DiscRectBB<MIType> genRectMIBoundingBox(const std::vector<MI>& mi);
misc::DiscRectBB<MIType> genRectMIBoundingBox(const MIVec& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
