#include <sgpp/combigrid/node_families/clenshaw_curtis_nodes.hpp>
#include <sgpp/combigrid/node_families/getter/second_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

NodeFamily* getSecondTypeChebyshevNodes() {
  static node_families::ClenshawCurtisNodes instance;

  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp