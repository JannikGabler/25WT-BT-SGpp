#pragma once

#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/default_functions/equidistant_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/simpson_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/trapezoidal_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

EquidistantNodeGenFunc::EquidistantNodeGenFunc()
    : NodeGenFunc(tools::fnv1aHash("Equidistant Node Generation Function")) {}

base::DataVector EquidistantNodeGenFunc::genGPs(const GPCntType nNodes,
                                                const bool addBoundary) const {
  if (addBoundary) {
    return genGPsWithBoundary(nNodes);
  } else {
    return genGPsWithoutBoundary(nNodes);
  }
}

base::DataVector EquidistantNodeGenFunc::genGPsWithBoundary(const GPCntType nNodes) const {
  assert(nNodes >= 2);

  const double factor = 1.0 / static_cast<double>(nNodes - 1);
  base::DataVector nodes(nNodes);

  for (GPCntType i = 0; i < nNodes; i++) {
    nodes[i] = static_cast<double>(i) * factor;
  }

  return nodes;
}
base::DataVector EquidistantNodeGenFunc::genGPsWithoutBoundary(const GPCntType nNodes) const {
  const double factor = 1.0 / static_cast<double>(nNodes + 1);
  base::DataVector nodes(nNodes);

  for (GPCntType i = 1; i <= nNodes; i++) {
    nodes[i - 1] = static_cast<double>(i) * factor;
  }

  return nodes;
}

QuadRule* EquidistantNodeGenFunc::getQuadRule(const GPCntType nNodes) const {
  if (nNodes & 1) {  // nNodes is odd
    return getSimpsonQuadRule();
  } else {  // nNodes is even
    return getTrapezoidalQuadRule();
  }
}

InterpolationMethod* EquidistantNodeGenFunc::getInterpolationMethod(const GPCntType nNodes) const {
  return getBarycentricFormula(this);
}

/********
Operators
********/

bool EquidistantNodeGenFunc::operator==(const NodeGenFunc& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
