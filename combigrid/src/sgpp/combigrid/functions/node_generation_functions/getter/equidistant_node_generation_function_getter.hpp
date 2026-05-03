/**
 * @file equidistant_node_generation_function_getter.hpp
 * @brief Accessor for the shared equidistant node-generation function.
 */
#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) equidistant node-generation
 * function.
 * @return Non-owning pointer to a singleton instance.
 */
NodeGenFunc* getEquidistantNodeGenFunc();

}  // namespace combigrid
}  // namespace sgpp
