#ifndef COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP
#define COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

bool isMIVecDownwardsClosed(const MIVec& miVec);

MIVec genMIVecDownwardsClosure(const MIVec& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
