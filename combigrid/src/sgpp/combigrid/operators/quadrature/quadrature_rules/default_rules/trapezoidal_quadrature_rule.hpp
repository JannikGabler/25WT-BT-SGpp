#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include "sgpp/combigrid/type_defs.hpp"

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

class TrapezoidalQuadRule : public QuadRule {
 public:
  TrapezoidalQuadRule() : QuadRule(tools::fnv1aHash("Trapezoidal Quadrature Rule")) {}

  base::DataVector getWeights(const GPCntType nNodes) const override {
    base::DataVector weights(nNodes);

    genWeightsInplace(nNodes, weights);

    return weights;
  }

  void genWeightsInplace(const GPCntType nNodes, base::DataVector& out,
                         const size_t startIdx = 0) const override {
    assert(out.size() - startIdx >= nNodes);

    if (nNodes == 0) {
      return;
    } else if (nNodes == 1) {
      out[startIdx] = 1.0;
      return;
    }

    const double nodeDistance = 1 / static_cast<double>(nNodes - 1);

    out[startIdx] = nodeDistance / 2;
    std::fill_n(out.begin() + startIdx + 1, nNodes - 2, nodeDistance);
    out[startIdx + nNodes - 1] = nodeDistance / 2;
  }
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp