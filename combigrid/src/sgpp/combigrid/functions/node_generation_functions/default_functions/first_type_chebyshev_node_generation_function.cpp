#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/default_functions/first_type_chebyshev_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

/***********
Constructors
***********/

FirstTypeChebyshevNodeGenFunc::FirstTypeChebyshevNodeGenFunc()
    : NodeGenFunc(tools::fnv1aHash("First Type Chebyshev Node Generation Function")) {}

/**************
Node generation
**************/

void FirstTypeChebyshevNodeGenFunc::genNodesWithoutBoundary(const GPCntType nNodes,
                                                            base::DataVector& out,
                                                            size_t startIdx) const {
  assert(out.size() >= startIdx + nNodes);

  const double f1 = M_PI / (2 * static_cast<double>(nNodes));

  for (GPCntType i = 0; i < nNodes; i++) {
    const double f2 = 2 * static_cast<double>(nNodes - 1 - i) + 1;  // Reverse order
    const double nonTransformedNode = std::cos(f1 * f2);
    out[startIdx++] = nonTransformedNode / 2 + 0.5;
  }
}

/***********
SG Operators
***********/

QuadRule* FirstTypeChebyshevNodeGenFunc::getQuadRule(const GPCntType nNodes) const {
  // TODO
  throw base::not_implemented_exception("Operation is not yet implemented!");
}

InterpolationMethod* FirstTypeChebyshevNodeGenFunc::getInterpolationMethod(
    const GPCntType nNodes) const {
  return getBarycentricFormula(this);
}

/********
Operators
********/

bool FirstTypeChebyshevNodeGenFunc::operator==(const NodeGenFunc& other) const {
  return typeid(*this) == typeid(other);
}

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
