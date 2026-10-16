#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/trapezoidal_quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

TrapezoidalQuadRule::TrapezoidalQuadRule()
    : QuadRule(tools::fnv1aHash("Trapezoidal Quadrature Rule")) {}

base::DataVector TrapezoidalQuadRule::genWeights(const GPCntType nNodes) const {
  base::DataVector weights(nNodes);

  genWeightsInplace(weights);

  return weights;
}

void TrapezoidalQuadRule::genWeightsInplace(const std::span<double> out) const {
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

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp