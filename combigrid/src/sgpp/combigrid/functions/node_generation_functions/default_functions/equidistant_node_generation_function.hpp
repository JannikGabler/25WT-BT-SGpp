/**
 * @file equidistant_node_generation_function.hpp
 * @brief Equidistant 1D node generator on @f$[0,1]@f$.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/simpson_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/trapezoidal_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Concrete @c NodeGenFunc implementations.
 */
namespace node_gen_funcs {

/**
 * @brief Equidistant nodes on @f$[0,1]@f$.
 *
 * The @f$n@f$ inner nodes are placed at @f$x_i = (i+1)/(n+1)@f$. The
 * default 1D quadrature rule is composite Simpson when @c nNodes is odd
 * (and @c >=3) and trapezoidal otherwise. The default 1D interpolation
 * method is the generic barycentric Lagrange formula.
 */
class EquidistantNodeGenFunc : public NodeGenFunc {
 public:
  /***********
  Constructors
  ***********/
  /// @brief Constructs the function with a deterministic id derived from its name.
  EquidistantNodeGenFunc();

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
  QuadRule* getQuadRule(GPCntType nNodes) const override;

  /// @copydoc NodeGenFunc::getInterpolationMethod
  InterpolationMethod* getInterpolationMethod(GPCntType nNodes) const override;

  /********
  Operators
  ********/
  /// @copydoc NodeGenFunc::operator==(const NodeGenFunc&) const
  bool operator==(const NodeGenFunc& other) const override;
};

}  // namespace node_gen_funcs

}  // namespace combigrid
}  // namespace sgpp
