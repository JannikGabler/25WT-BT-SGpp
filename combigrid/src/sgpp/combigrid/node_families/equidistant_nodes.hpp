/**
 * @file equidistant_nodes.hpp
 * @brief Equidistant 1D node generator on @f$[0,1]@f$.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/simpson_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/trapezoidal_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Concrete @c NodeFamily implementations.
 */
namespace node_families {

/**
 * @brief Equidistant nodes on @f$[0,1]@f$.
 *
 * The @f$n@f$ inner nodes are placed at @f$x_i = (i+1)/(n+1)@f$. The
 * default 1D quadrature rule is composite Simpson when @c nNodes is odd
 * (and @c >=3) and trapezoidal otherwise. The default 1D interpolation
 * method is the generic barycentric Lagrange formula.
 */
class EquidistantNodes : public NodeFamily {
 public:
  /***********
  Constructors
  ***********/

  /**************
  Node generation
  **************/
  /// @copydoc NodeFamily::genNodesWithoutBoundary
  void genNodesWithoutBoundary(GPCntType nNodes, base::DataVector& out,
                               size_t startIdx = 0) const override;

  /// @copydoc NodeFamily::genDeltaNodesWithoutBoundary
  void genDeltaNodesWithoutBoundary(LvlType lvl, Lvl2GPCntFunc lvl2GPCntFunc,
                                    std::span<double> out) const override;

  /******
  Getters
  ******/
  /// @copydoc NodeFamily::getQuadRule
  QuadRule* getQuadRule(GPCntType nNodes, bool includeBoundary) const override;

  /// @copydoc NodeFamily::getInterpolationMethod
  InterpolationMethod* getInterpolationMethod(GPCntType nNodes) const override;

  /// @copydoc NodeFamily::isNested
  bool isNested(Lvl2GPCntFunc lvl2GPCntFunc) const override;

  /********
  Utilities
  ********/
  /**
   * TODO: Document
   */
  NodeIdxLvlMapFunc getNodeIdxMapBetweenLvls(Lvl2GPCntFunc lvl2GPCntFunc) const override;

  /**
   * TODO: Document
   */
  NodeIdxLvlMapFunc getNodeIdxMapFromDeltaTGLvlToRegularLvl(
      Lvl2GPCntFunc lvl2GPCntFunc) const override;

  /********
  Operators
  ********/
  /// @copydoc NodeFamily::operator==(const NodeFamily&) const
  bool operator==(const NodeFamily& other) const override;
};

}  // namespace node_families

}  // namespace combigrid
}  // namespace sgpp
