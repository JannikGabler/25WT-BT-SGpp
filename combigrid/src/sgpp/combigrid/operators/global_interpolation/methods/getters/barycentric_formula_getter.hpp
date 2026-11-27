/**
 * @file barycentric_formula_getter.hpp
 * @brief Accessor for the shared generic barycentric-formula instance.
 */
#pragma once

#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) generic barycentric Lagrange
 * formula bound to the given node family.
 * @param nodeFamily Node family the formula should use.
 * @return Non-owning pointer to a singleton method instance.
 */
InterpolationMethod* getBarycentricFormula(const NodeFamily* const nodeFamily);

}  // namespace combigrid
}  // namespace sgpp
