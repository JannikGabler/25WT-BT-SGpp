/**
 * @file clenshaw_curtis_quadrature_rule_getter.hpp
 * @brief Accessor for the shared Clenshaw-Curtis quadrature-rule instance.
 */
#pragma once

#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) Clenshaw-Curtis quadrature rule.
 * @return Non-owning pointer to a singleton rule instance.
 */
QuadRule* getClenshawCurtisQuadRule();

}
}  // namespace sgpp
