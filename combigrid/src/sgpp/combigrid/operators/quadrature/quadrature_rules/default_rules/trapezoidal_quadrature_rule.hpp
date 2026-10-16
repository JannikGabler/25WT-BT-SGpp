/**
 * @file trapezoidal_quadrature_rule.hpp
 * @brief 1D composite trapezoidal quadrature on @f$[0,1]@f$.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

/**
 * @brief Composite trapezoidal quadrature on equidistant nodes in @f$[0,1]@f$.
 *
 * For @c nNodes >= 2 the weights are
 * @f$h/2, h, h, \ldots, h, h/2@f$ with @f$h = 1/(\mathrm{nNodes}-1)@f$.
 * For a single node the weight is @c 1.0 (degenerate).
 */
class TrapezoidalQuadRule : public QuadRule {
 public:
  /// @brief Constructs the rule with a deterministic id derived from its name.
  TrapezoidalQuadRule();

  /// @copydoc QuadRule::genWeights
  base::DataVector genWeights(const GPCntType nNodes) const override;

  /// @copydoc QuadRule::genWeightsInplace
  void genWeightsInplace(const std::span<double> out) const override;
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp