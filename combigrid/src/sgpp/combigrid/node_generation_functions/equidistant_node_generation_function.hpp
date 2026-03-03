#pragma once

#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/simpson_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/trapezoidal_quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class EquidistantNodeGenFunc : public NodeGenFunc {
 public:
  base::DataVector genGPs(const size_t nNodes) const override {
    const double denominator = static_cast<double>(nNodes + 1);
    base::DataVector nodes(nNodes);

    for (GPCntType i = 1; i <= nNodes; i++) {
      const double numerator = static_cast<double>(i);

      nodes[i - 1] = numerator / denominator;
    }

    return nodes;
  }

  QuadRule* getQuadRule(const size_t nNodes) const override {
    // if (nNodes & 1) {  // nNodes is odd
    //   return std::unique_ptr<QuadRule>(new SimpsonQuadRule());
    // } else {  // nNodes is even
    //   return std::unique_ptr<QuadRule>(new TrapezoidalQuadRule());
    // }
    throw base::not_implemented_exception("");
  }

  bool operator==(const NodeGenFunc& other) const override {
    return typeid(*this) == typeid(other);
  }
};

}  // namespace combigrid
}  // namespace sgpp
