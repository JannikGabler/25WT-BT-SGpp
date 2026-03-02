#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>

namespace sgpp {
namespace combigrid {

class TrapezoidalQuadRule : public QuadRule {
 public:
  TrapezoidalQuadRule() : QuadRule(tools::fnv1aHash("Trapezoidal Quadrature Rule")) {}

  base::DataVector getWeights(const size_t nNodes) const override {
    if (nNodes == 0) {
      return {};
    } else if (nNodes == 1) {
      return base::DataVector{1};
    }

    const double nodeDistance = 1 / static_cast<double>(nNodes - 1);
    base::DataVector weights(nNodes);

    weights[0] = nodeDistance / 2;
    std::fill_n(weights.begin() + 1, nNodes - 2, nodeDistance);
    weights[nNodes - 1] = nodeDistance / 2;

    return weights;
  }
};

}  // namespace combigrid
}  // namespace sgpp