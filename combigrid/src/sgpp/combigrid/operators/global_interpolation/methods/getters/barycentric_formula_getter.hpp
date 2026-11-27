/**
 * @file barycentric_formula_getter.hpp
 * @brief Accessor for the shared generic barycentric-formula instance.
 */
#pragma once

#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) generic barycentric Lagrange
 * formula (it derives the barycentric weights from the nodes it is given, so one
 * instance serves every node family).
 * @return Non-owning pointer to a singleton method instance.
 */
InterpolationMethod* getBarycentricFormula();

}  // namespace combigrid
}  // namespace sgpp
