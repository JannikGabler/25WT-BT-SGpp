#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/second_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

NodeFamily* getClenshawCurtisNodes() { return getSecondTypeChebyshevNodes(); }

}  // namespace combigrid
}  // namespace sgpp