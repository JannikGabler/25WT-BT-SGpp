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

/***********
Constructors
***********/

SecondTypeChebyshevNodeGenFunc::SecondTypeChebyshevNodeGenFunc()
    : NodeGenFunc(tools::fnv1aHash("Second Type Chebyshev Node Generation Function")) {}

/**************
Node generation
**************/

void SecondTypeChebyshevNodeGenFunc::genNodesInplace(const GPCntType nNodes, base::DataVector& out,
                                                     size_t startIdx) const {
  assert(out.size() - startIdx >= nNodes);

  const double f1 = M_PI / static_cast<double>(nNodes + 1);

  for (GPCntType i = 1; i <= nNodes; i++) {
    const double f2 = static_cast<double>(nNodes - i + 1);  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    out[startIdx++] = nonTransformedNode / 2 + 0.5;
  }
}

/***********
SG Operators
***********/

QuadRule* SecondTypeChebyshevNodeGenFunc::getQuadRule(const GPCntType nNodes) const {
  return getClenshawCurtisQuadRule();
}

InterpolationMethod* SecondTypeChebyshevNodeGenFunc::getInterpolationMethod(
    const GPCntType nNodes) const {
  return getOptBarycentricFormula();
}

/********
Operators
********/

bool SecondTypeChebyshevNodeGenFunc::operator==(const NodeGenFunc& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
