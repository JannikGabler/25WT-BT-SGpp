#include <sgpp/combigrid/node_families/first_type_chebyshev_nodes.hpp>
#include <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

NodeFamily* getFirstTypeChebyshevNodes() {
  static node_families::FirstTypeChebyshevNodes instance;

  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp