/**
 * @file first_type_chebyshev_nodes.hpp
 * @brief Chebyshev nodes of the first kind (Gauss-Chebyshev nodes).
 */
#pragma once

#include <sgpp/combigrid/node_families/node_family.hpp>

namespace sgpp {
namespace combigrid {

namespace node_families {

/**
 * @brief Chebyshev nodes of the first kind, mapped to @f$[0,1]@f$.
 *
 * The @f$n@f$ inner nodes are
 * @f$x_i = \tfrac12(1 - \cos(\tfrac{(2i+1)\pi}{2n}))@f$, the roots of the
 * @f$n@f$-th Chebyshev polynomial of the first kind shifted onto the
 * unit interval. These nodes are not nested across different @c nNodes.
 */
class FirstTypeChebyshevNodes : public NodeFamily {
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
  /**
   * @brief Returns the interpolatory rule on the @p nNodes nodes
   * (@c quadrature_rules::FejerFirstQuadRule): Fejér's first rule without the boundary nodes, the
   * interpolatory rule on 0, the inner nodes and 1 with them.
   */
  QuadRule* getQuadRule(GPCntType nNodes, bool includeBoundary) const override;

  /// @copydoc NodeFamily::getInterpolationMethod
  InterpolationMethod* getInterpolationMethod(const GPCntType nNodes) const override;

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
