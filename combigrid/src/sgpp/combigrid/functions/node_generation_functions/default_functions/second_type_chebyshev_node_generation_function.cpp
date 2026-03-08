#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/default_functions/second_type_chebyshev_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

SecondTypeChebyshevNodeGenFunc::SecondTypeChebyshevNodeGenFunc()
    : NodeGenFunc(tools::fnv1aHash("Second Type Chebyshev Node Generation Function")) {}

base::DataVector SecondTypeChebyshevNodeGenFunc::genGPs(const GPCntType nNodes,
                                                        const bool addBoundary) const {
  if (addBoundary) {
    return genGPsWithBoundary(nNodes);
  } else {
    return genGPsWithoutBoundary(nNodes);
  }
}

base::DataVector SecondTypeChebyshevNodeGenFunc::genGPsWithBoundary(const GPCntType nNodes) const {
  assert(nNodes >= 2);

  const GPCntType k = nNodes - 1;
  const double f1 = M_PI / static_cast<double>(k);
  base::DataVector nodes(nNodes);

  for (GPCntType i = 0; i <= k; i++) {
    const double f2 = static_cast<double>(k - i);  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    nodes[i] = nonTransformedNode / 2 + 0.5;
  }

  return nodes;
}

base::DataVector SecondTypeChebyshevNodeGenFunc::genGPsWithoutBoundary(
    const GPCntType nNodes) const {
  const double f1 = M_PI / static_cast<double>(nNodes + 1);
  base::DataVector nodes(nNodes);

  for (GPCntType i = 1; i <= nNodes; i++) {
    const double f2 = static_cast<double>(nNodes - i + 1);  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    nodes[i - 1] = nonTransformedNode / 2 + 0.5;
  }

  return nodes;
}

QuadRule* SecondTypeChebyshevNodeGenFunc::getQuadRule(const GPCntType nNodes) const {
  return getClenshawCurtisQuadRule();
}

InterpolationMethod* SecondTypeChebyshevNodeGenFunc::getInterpolationMethod(
    const GPCntType nNodes) const {
  return getOptBarycentricFormula();
}

bool SecondTypeChebyshevNodeGenFunc::operator==(const NodeGenFunc& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
