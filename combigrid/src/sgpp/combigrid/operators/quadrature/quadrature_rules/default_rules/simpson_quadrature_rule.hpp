#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

class SimpsonQuadRule : public QuadRule {
 public:
  SimpsonQuadRule() : QuadRule(tools::fnv1aHash("Simpson Quadrature Rule")) {}

  base::DataVector getWeights(const GPCntType nNodes) const override {
    base::DataVector weights(nNodes);

    genWeightsInplace(nNodes, weights);

    return weights;
  }

  void genWeightsInplace(const GPCntType nNodes, base::DataVector& out,
                         size_t startIdx = 0) const override {
    assert(out.size() - startIdx >= nNodes);

    if ((nNodes & 1) == 0) {  // Is nNodes even?
      throw std::invalid_argument("The argument 'nNodes' has to be odd.");
    } else if (nNodes == 1) {
      out[startIdx] = 1.0;
      return;
    }

    const double w1 = 1 / (3 * static_cast<double>(nNodes - 1));
    const double w2 = 4 * w1;
    const double w3 = 2 * w1;

    // out[startIdx++] = w1;
    // for (size_t i = 1; i < nNodes - 1; i += 2) {  // i is always odd
    //   out[startIdx++] = w2;
    //   out[startIdx++] = w3;
    // }
    // out[startIdx + nNodes - 1] = w1;
    out[startIdx++] = w1;
    for (size_t i = 0; i < (nNodes - 3) / 2; i++) {
      out[startIdx++] = w2;
      out[startIdx++] = w3;
    }
    out[startIdx++] = w2;
    out[startIdx] = w1;
  }
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp