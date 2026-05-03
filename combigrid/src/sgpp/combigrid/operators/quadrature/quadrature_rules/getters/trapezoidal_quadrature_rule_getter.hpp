/**
 * @file trapezoidal_quadrature_rule_getter.hpp
 * @brief Accessor for the shared trapezoidal quadrature-rule instance.
 */
#pragma once

#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) trapezoidal quadrature rule.
 * @return Non-owning pointer to a singleton rule instance.
 */
QuadRule* getTrapezoidalQuadRule();

}
}  // namespace sgpp
