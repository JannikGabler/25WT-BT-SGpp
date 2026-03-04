#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <stdexcept>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

class SimpsonQuadRule : public QuadRule {
 public:
  SimpsonQuadRule() : QuadRule(tools::fnv1aHash("Simpson Quadrature Rule")) {}

  base::DataVector getWeights(const size_t nNodes) const override {
    if ((nNodes & 1) == 0) {  // Is nNodes even?
      throw std::invalid_argument("The argument 'nNodes' has to be odd.");
    } else if (nNodes == 1) {
      return base::DataVector{1};
    }

    const double w1 = 1 / (3 * static_cast<double>(nNodes - 1));
    const double w2 = 4 * w1;
    const double w3 = 2 * w1;
    base::DataVector weights(nNodes);

    weights[0] = w1;
    for (size_t i = 1; i < nNodes - 1; i += 2) {  // i is always odd
      weights[i] = w2;
      weights[i + 1] = w3;
    }
    weights[nNodes - 1] = w1;

    return weights;
  }
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp