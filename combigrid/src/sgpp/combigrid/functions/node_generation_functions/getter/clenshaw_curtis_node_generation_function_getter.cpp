#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/second_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

NodeGenFunc* getClenshawCurtisNodeGenFunc() { return getSecondTypeChebyshevNodeGenFunc(); }

}  // namespace combigrid
}  // namespace sgpp