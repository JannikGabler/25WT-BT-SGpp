/**
 * @file second_type_chebyshev_nodes_getter.hpp
 * @brief Accessor for the shared Chebyshev-of-second-kind node family.
 */
#pragma once

#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) Chebyshev-of-second-kind
 * node family (Chebyshev-Lobatto nodes).
 * @return Non-owning pointer to a singleton instance.
 */
NodeFamily* getSecondTypeChebyshevNodes();

}  // namespace combigrid
}  // namespace sgpp
