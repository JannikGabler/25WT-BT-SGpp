/**
 * @file barycentric_formula_getter.hpp
 * @brief Accessor for the shared generic barycentric-formula instance.
 */
#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) generic barycentric Lagrange
 * formula bound to the given node-generation function.
 * @param nodeGenFunc Node-generation function the formula should use.
 * @return Non-owning pointer to a singleton method instance.
 */
InterpolationMethod* getBarycentricFormula(const NodeGenFunc* const nodeGenFunc);

}  // namespace combigrid
}  // namespace sgpp
