/**
 * @file clenshaw_curtis_nodes.hpp
 * @brief Chebyshev nodes of the second kind (Clenshaw-Curtis-style).
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace node_families {

/**
 * @brief Chebyshev nodes of the second kind on @f$[0,1]@f$ (also known as
 * Chebyshev-Lobatto / extrema nodes).
 *
 * The matching 1D quadrature is Clenshaw-Curtis and the matching
 * interpolation method is the optimized barycentric Lagrange formula.
 */
class ClenshawCurtisNodes : public NodeFamily {
 public:
  /***********
  Constructors
  ***********/
  /// @brief Constructs the node family with a deterministic id derived from its name.
  ClenshawCurtisNodes();

  /// @name Node generation
  /// @{

  /// @copydoc NodeFamily::genNodesWithoutBoundary
  void genNodesWithoutBoundary(GPCntType nNodes, base::DataVector& out,
                               size_t startIdx = 0) const override;

  /// @copydoc NodeFamily::genDeltaNodesWithoutBoundary
  void genDeltaNodesWithoutBoundary(LvlType lvl, Lvl2GPCntFunc lvl2GPCntFunc,
                                    std::span<double> out) const override;

  /// @}

  /// @name Getters
  /// @{

  /// @copydoc NodeFamily::getQuadRule
  QuadRule* getQuadRule(GPCntType nNodes, bool includeBoundary) const override;

  /// @copydoc NodeFamily::getInterpolationMethod
  InterpolationMethod* getInterpolationMethod(const GPCntType nNodes) const override;

  /// @copydoc NodeFamily::isNested
  bool isNested(Lvl2GPCntFunc lvl2GPCntFunc) const override;

  /// @}

  /// @name Utilities
  /// @{

  /**
   * TODO: Document
   */
  NodeIdxLvlMapFunc getNodeIdxMapBetweenLvls(Lvl2GPCntFunc lvl2GPCntFunc) const override;

  /**
   * TODO: Document
   */
  NodeIdxLvlMapFunc getNodeIdxMapFromDeltaTGLvlToRegularLvl(
      Lvl2GPCntFunc lvl2GPCntFunc) const override;

  /// @}

  /// @name Operators
  /// @{

  /// @copydoc NodeFamily::operator==(const NodeFamily&) const
  bool operator==(const NodeFamily& other) const override;

  /// @}
};

/// The Clenshaw-Curtis nodes are the Chebyshev nodes of the second kind.
using SecondTypeChebyshevNodes = ClenshawCurtisNodes;

}  // namespace node_families

}  // namespace combigrid
}  // namespace sgpp
