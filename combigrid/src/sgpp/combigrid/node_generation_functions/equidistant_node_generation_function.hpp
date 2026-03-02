#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/simpson_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/trapezoidal_quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class EquidistantNodeGenFunc : public NodeGenFunc {
 public:
  base::DataVector operator()(const size_t nNodes) const override {
    const double denominator = static_cast<double>(nNodes + 1);
    base::DataVector nodes(nNodes);

    for (GPCntType i = 1; i <= nNodes; i++) {
      const double numerator = static_cast<double>(i);

      nodes[i - 1] = numerator / denominator;
    }

    return nodes;
  }

  QuadRule getQuadRule(const size_t nNodes) const override {
    if (nNodes & 1) {  // nNodes is odd
      return SimpsonQuadRule();
    } else {  // nNodes is even
      return TrapezoidalQuadRule();
    }
  }
};

}  // namespace combigrid
}  // namespace sgpp
