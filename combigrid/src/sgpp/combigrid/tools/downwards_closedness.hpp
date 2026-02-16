#ifndef COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP
#define COMBIGRID_TOOLS_DOWNWARDS_CLOSEDNESS_HPP

#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

MIVec genMIVecDownwardsClosure(const MIVec& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
