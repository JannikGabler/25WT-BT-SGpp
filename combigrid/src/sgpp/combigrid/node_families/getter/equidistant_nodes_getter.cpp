#include <sgpp/combigrid/node_families/equidistant_nodes.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

NodeFamily* getEquidistantNodes() {
  static node_families::EquidistantNodes instance;

  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp