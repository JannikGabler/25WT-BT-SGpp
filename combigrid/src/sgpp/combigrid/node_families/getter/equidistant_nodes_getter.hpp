/**
 * @file equidistant_nodes_getter.hpp
 * @brief Accessor for the shared equidistant node family.
 */
#pragma once

#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) equidistant node family.
 * @return Non-owning pointer to a singleton instance.
 */
NodeFamily* getEquidistantNodes();

}  // namespace combigrid
}  // namespace sgpp
