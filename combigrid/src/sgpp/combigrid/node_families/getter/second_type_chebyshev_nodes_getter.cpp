#include <sgpp/combigrid/node_families/getter/second_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/node_families/second_type_chebyshev_nodes.hpp>

namespace sgpp {
namespace combigrid {

NodeFamily* getSecondTypeChebyshevNodes() {
  static node_families::SecondTypeChebyshevNodes instance;

  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp