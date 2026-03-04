#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/trapezoidal_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/trapezoidal_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

QuadRule* getTrapezoidalQuadRule() {
  static quadrature_rules::TrapezoidalQuadRule instance;
  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp