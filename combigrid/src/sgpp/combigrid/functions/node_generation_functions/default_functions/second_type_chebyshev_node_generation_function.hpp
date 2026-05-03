/**
 * @file second_type_chebyshev_node_generation_function.hpp
 * @brief Chebyshev nodes of the second kind (Clenshaw-Curtis-style).
 */
#pragma once

#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

/**
 * @brief Chebyshev nodes of the second kind on @f$[0,1]@f$ (also known as
 * Chebyshev-Lobatto / extrema nodes).
 *
 * The matching 1D quadrature is Clenshaw-Curtis and the matching
 * interpolation method is the optimized barycentric Lagrange formula.
 */
class SecondTypeChebyshevNodeGenFunc : public NodeGenFunc {
 public:
  /***********
  Constructors
  ***********/
  /// @brief Constructs the function with a deterministic id derived from its name.
  SecondTypeChebyshevNodeGenFunc();

  /**************
  Node generation
  **************/
  /// @copydoc NodeGenFunc::genNodesWithoutBoundary
  void genNodesWithoutBoundary(GPCntType nNodes, base::DataVector& out,
                               size_t startIdx = 0) const override;

  /***********
  SG Operators
  ***********/
  /// @copydoc NodeGenFunc::getQuadRule
  QuadRule* getQuadRule(const GPCntType nNodes) const override;

  /// @copydoc NodeGenFunc::getInterpolationMethod
  InterpolationMethod* getInterpolationMethod(const GPCntType nNodes) const override;

  /********
  Operators
  ********/
  /// @copydoc NodeGenFunc::operator==(const NodeGenFunc&) const
  bool operator==(const NodeGenFunc& other) const override;
};

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
