#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/simpson_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/simpson_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

QuadRule* getSimpsonQuadRule() {
  static quadrature_rules::SimpsonQuadRule instance;
  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp