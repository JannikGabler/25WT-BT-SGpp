/**
 * @file trapezoidal_quadrature_rule.hpp
 * @brief 1D composite trapezoidal quadrature on @f$[0,1]@f$.
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
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
  TrapezoidalQuadRule() : QuadRule(tools::fnv1aHash("Trapezoidal Quadrature Rule")) {}

  /// @copydoc QuadRule::genWeights
  base::DataVector genWeights(const GPCntType nNodes) const override {
    base::DataVector weights(nNodes);

    genWeightsInplace(weights);

    return weights;
  }

  /// @copydoc QuadRule::genWeightsInplace
  void genWeightsInplace(const std::span<double> out) const override {
    const size_t nNodes = out.size();

    if (nNodes == 0) {
      return;
    } else if (nNodes == 1) {
      out[0] = 1.0;
      return;
    }

    const double nodeDistance = 1 / static_cast<double>(nNodes - 1);

    std::ranges::fill(out, nodeDistance);
    out.front() = nodeDistance / 2;
    out.back() = nodeDistance / 2;
  }
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
