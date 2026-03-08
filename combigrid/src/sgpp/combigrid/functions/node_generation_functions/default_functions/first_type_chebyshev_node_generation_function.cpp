#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/default_functions/first_type_chebyshev_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

FirstTypeChebyshevNodeGenFunc::FirstTypeChebyshevNodeGenFunc()
    : NodeGenFunc(tools::fnv1aHash("First Type Chebyshev Node Generation Function")) {}

base::DataVector FirstTypeChebyshevNodeGenFunc::genGPs(const GPCntType nNodes,
                                                       const bool addBoundary) const {
  if (addBoundary) {
    return genGPsWithBoundary(nNodes);
  } else {
    return genGPsWithoutBoundary(nNodes);
  }
}

QuadRule* FirstTypeChebyshevNodeGenFunc::getQuadRule(const GPCntType nNodes) const {
  // TODO
  throw base::not_implemented_exception("Operation is not yet implemented!");
}

InterpolationMethod* FirstTypeChebyshevNodeGenFunc::getInterpolationMethod(
    const GPCntType nNodes) const {
  return getBarycentricFormula(this);
}

bool FirstTypeChebyshevNodeGenFunc::operator==(const NodeGenFunc& other) const {
  return typeid(*this) == typeid(other);
}

base::DataVector FirstTypeChebyshevNodeGenFunc::genGPsWithBoundary(const GPCntType nNodes) const {
  assert(nNodes >= 2);

  base::DataVector nodes(nNodes);

  if (nNodes >= 3) {
    const GPCntType m = nNodes - 2;
    const double f1 = M_PI / (2 * static_cast<double>(m));

    for (GPCntType i = 0; i < m; i++) {
      const double f2 = 2 * static_cast<double>(m - 1 - i) + 1;  // Reverse order
      const double nonTransformedNode = std::cos(f1 * f2);
      nodes[i + 1] = nonTransformedNode / 2 + 0.5;
    }
  }

  nodes[nNodes - 1] = 1.0;
  return nodes;
}

base::DataVector FirstTypeChebyshevNodeGenFunc::genGPsWithoutBoundary(
    const GPCntType nNodes) const {
  const double f1 = M_PI / (2 * static_cast<double>(nNodes));
  base::DataVector nodes(nNodes);

  for (GPCntType i = 0; i < nNodes; i++) {
    const double f2 = 2 * static_cast<double>(nNodes - 1 - i) + 1;  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    nodes[i] = nonTransformedNode / 2 + 0.5;
  }

  return nodes;
}

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
