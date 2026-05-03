/**
 * @file clenshaw_curtis_node_generation_function_getter.hpp
 * @brief Accessor for the shared Clenshaw-Curtis node-generation function.
 */
#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) Clenshaw-Curtis
 * node-generation function (Chebyshev-Lobatto / extrema nodes).
 * @return Non-owning pointer to a singleton instance.
 */
NodeGenFunc* getClenshawCurtisNodeGenFunc();

}  // namespace combigrid
}  // namespace sgpp
