#ifndef COMBIGRID_TOOLS_FULL_SPARSE_GRID_MULTIINDEX_GENERATION_HPP
#define COMBIGRID_TOOLS_FULL_SPARSE_GRID_MULTIINDEX_GENERATION_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

MIVec genMIVecForFullSG(MIType maxLvl, size_t nDim);

}
}  // namespace combigrid
}  // namespace sgpp

#endif