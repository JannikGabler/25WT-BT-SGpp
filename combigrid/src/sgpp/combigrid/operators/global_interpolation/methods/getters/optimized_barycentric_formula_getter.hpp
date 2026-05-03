/**
 * @file optimized_barycentric_formula_getter.hpp
 * @brief Accessor for the shared Clenshaw-Curtis-optimized barycentric-formula
 * instance.
 */
#pragma once

#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) Clenshaw-Curtis-optimized
 * barycentric Lagrange formula.
 * @return Non-owning pointer to a singleton method instance.
 */
InterpolationMethod* getOptBarycentricFormula();

}
}  // namespace sgpp
