#include <sgpp/combigrid/functions/node_generation_functions/default_functions/second_type_chebyshev_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/second_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

NodeGenFunc* getSecondTypeChebyshevNodeGenFunc() {
  static node_gen_funcs::SecondTypeChebyshevNodeGenFunc instance;

  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp