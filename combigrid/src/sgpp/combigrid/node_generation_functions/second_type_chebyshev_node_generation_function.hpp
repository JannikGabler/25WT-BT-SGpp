#pragma once

#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class SecondTypeChebyshevNodeGenFunc : public NodeGenFunc {
 public:
  base::DataVector operator()(const size_t nNodes) const override {
    const double f1 = M_PI / static_cast<double>(nNodes + 1);
    base::DataVector nodes(nNodes);

    for (GPCntType i = 1; i <= nNodes; i++) {
      const double f2 = static_cast<double>(nNodes - i + 1);  // Reverse order
      const double nonTransformedNode = std::cos(f1 * f2);
      nodes[i - 1] = nonTransformedNode / 2 + 0.5;
    }

    return nodes;
  }

  QuadRule getQuadRule(const size_t nNodes) const override {
    // TODO
    throw base::not_implemented_exception("Operation is not yet implemented!");
  }
};

}  // namespace combigrid
}  // namespace sgpp
