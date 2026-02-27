#ifndef COMBIGRID_NODE_GENERATION_FUNCTIONS_HPP
#define COMBIGRID_NODE_GENERATION_FUNCTIONS_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

base::DataVector genEquidistantNodes(GPCntType nNodes);

base::DataVector genFirstTypeChebyshevNodes(GPCntType nNodes);

base::DataVector genSecondTypeChebyshevNodes(GPCntType nNodes);

/*
Alias for second type Chebyshev nodes
*/
base::DataVector genClenshawCurtisNodes(GPCntType nNodes);

}  // namespace combigrid
}  // namespace sgpp

#endif