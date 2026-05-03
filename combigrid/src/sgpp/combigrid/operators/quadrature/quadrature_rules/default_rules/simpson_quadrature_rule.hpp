/**
 * @file simpson_quadrature_rule.hpp
 * @brief 1D composite Simpson quadrature on @f$[0,1]@f$.
 */
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

/**
 * @brief Composite Simpson quadrature on equidistant nodes in @f$[0,1]@f$.
 *
 * Requires an odd number of nodes (at least 3). The weight pattern is
 * @f$h/3, 4h/3, 2h/3, 4h/3, \ldots, 2h/3, 4h/3, h/3@f$ with
 * @f$h = 1/(\mathrm{nNodes}-1)@f$.
 */
class SimpsonQuadRule : public QuadRule {
 public:
  /// @brief Constructs the rule with a deterministic id derived from its name.
  SimpsonQuadRule() : QuadRule(tools::fnv1aHash("Simpson Quadrature Rule")) {}

  /// @copydoc QuadRule::getWeights
  base::DataVector getWeights(const GPCntType nNodes) const override {
    base::DataVector weights(nNodes);

    genWeightsInplace(nNodes, weights);

    return weights;
  }

  /**
   * @copydoc QuadRule::genWeightsInplace
   * @throws std::invalid_argument if @p nNodes is even.
   */
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