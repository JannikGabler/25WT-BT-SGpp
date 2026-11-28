/**
 * @file first_type_chebyshev_nodes_getter.hpp
 * @brief Accessor for the shared Chebyshev-of-first-kind node family.
 */
#pragma once

#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) Chebyshev-of-first-kind
 * node family.
 * @return Non-owning pointer to a singleton instance.
 */
NodeFamily* getFirstTypeChebyshevNodes();

}  // namespace combigrid
}  // namespace sgpp
