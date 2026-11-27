/**
 * @file clenshaw_curtis_nodes_getter.hpp
 * @brief Accessor for the shared Clenshaw-Curtis node family.
 */
#pragma once

#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) Clenshaw-Curtis
 * node family (Chebyshev-Lobatto / extrema nodes).
 * @return Non-owning pointer to a singleton instance.
 */
NodeFamily* getClenshawCurtisNodes();

}  // namespace combigrid
}  // namespace sgpp
