#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

QuadRule* getClenshawCurtisQuadRule() {
  static quadrature_rules::ClenshawCurtisQuadRule instance;
  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp