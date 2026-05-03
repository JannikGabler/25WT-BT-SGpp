/**
 * @file first_type_chebyshev_node_generation_function.hpp
 * @brief Chebyshev nodes of the first kind (Gauss-Chebyshev nodes).
 */
#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>

namespace sgpp {
namespace combigrid {

namespace node_gen_funcs {

/**
 * @brief Chebyshev nodes of the first kind, mapped to @f$[0,1]@f$.
 *
 * The @f$n@f$ inner nodes are
 * @f$x_i = \tfrac12(1 - \cos(\tfrac{(2i+1)\pi}{2n}))@f$, the roots of the
 * @f$n@f$-th Chebyshev polynomial of the first kind shifted onto the
 * unit interval. These nodes are not nested across different @c nNodes.
 */
class FirstTypeChebyshevNodeGenFunc : public NodeGenFunc {
 public:
  /***********
  Constructors
  ***********/
  /// @brief Constructs the function with a deterministic id derived from its name.
  FirstTypeChebyshevNodeGenFunc();

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
