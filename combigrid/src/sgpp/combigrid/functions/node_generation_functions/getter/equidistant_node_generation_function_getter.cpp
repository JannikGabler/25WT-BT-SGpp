#include <sgpp/combigrid/functions/node_generation_functions/default_functions/equidistant_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

NodeGenFunc* getEquidistantNodeGenFunc() {
  static node_gen_funcs::EquidistantNodeGenFunc instance;

  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp