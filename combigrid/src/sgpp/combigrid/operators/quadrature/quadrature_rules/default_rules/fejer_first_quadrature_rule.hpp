// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file fejer_first_quadrature_rule.hpp
 * @brief 1D interpolatory quadrature rule on Chebyshev nodes of the first kind (Fejér's first
 * rule), optionally with the boundary nodes 0 and 1.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

/**
 * @brief Interpolatory quadrature on the Chebyshev nodes of the first kind
 * (@c FirstTypeChebyshevNodes), on @f$[0,1]@f$.
 *
 * Without boundary nodes the @f$n@f$ nodes are @f$x_k = \tfrac12(1 + \cos\theta_k)@f$,
 * @f$\theta_k = \tfrac{(2k+1)\pi}{2n}@f$, and the weights are those of Fejér's first rule,
 * @f[
 *   w_k = \frac1n \Bigl(1 - 2 \sum_{j=1}^{\lfloor n/2 \rfloor}
 *         \frac{\cos(2j\theta_k)}{4j^2 - 1}\Bigr),
 * @f]
 * positive, summing to 1 and exact for polynomials of degree @f$n - 1@f$ (@f$n@f$ for odd
 * @f$n@f$, by symmetry).
 *
 * With boundary nodes (@c NodeFamily::genNodesWithBoundary: 0, the @f$m = n - 2@f$ first-kind
 * nodes, 1) the rule is the interpolatory rule on all @f$n@f$ nodes, i.e. the integral of the
 * polynomial interpolant of degree @f$n - 1@f$ through them (exact for degree @f$n - 1@f$, @f$n@f$
 * for odd @f$n@f$). Writing the interpolant as the one through the inner nodes plus
 * @f$T_m(2x - 1)(a + b x)@f$ gives the weights in closed form from Fejér's weights of the inner
 * nodes: both boundary nodes get @f$W = \tfrac14 \int_{-1}^1 (1 + t) T_m(t)\,dt@f$, which is
 * @f$O(m^{-2})@f$ and negative for @f$m \ge 2@f$, and inner node @f$k@f$ gets Fejér's weight minus
 * @f$W (\ell_k(1) + \ell_k(-1))@f$, with the Lagrange polynomials @f$\ell_k@f$ of the inner nodes
 * on @f$[-1, 1]@f$. The sum of the absolute weights is @f$1 + O(m^{-2})@f$. One and two nodes with
 * boundary are the midpoint (0.5) and the endpoints {0, 1}: weights 1 and {1/2, 1/2}.
 *
 * Weights are returned in the order of the nodes of the family (increasing) and are symmetric
 * bitwise.
 */
class FejerFirstQuadRule : public QuadRule {
 public:
  /**
   * @brief Constructs the rule for the node set without or with the boundary nodes 0 and 1; the
   * two variants have distinct ids.
   * @param includeBoundary Whether the nodes include 0 and 1 (as
   * @c NodeFamily::genNodesWithBoundary).
   */
  explicit FejerFirstQuadRule(bool includeBoundary);

  /// @copydoc QuadRule::genWeights
  base::DataVector genWeights(GPCntType nNodes) const override;

  /// @copydoc QuadRule::genWeightsInplace
  void genWeightsInplace(std::span<double> out) const override;

  /// @brief Returns whether the rule is the one for the node set with the boundary nodes.
  bool includesBoundary() const noexcept { return includeBoundary_; }

  /**
   * @brief Fejér's first rule (no boundary nodes) for @c out.size() nodes by the direct
   * @f$O(n^2)@f$ cosine sum (arguments reduced exactly in integer arithmetic).
   *
   * Used by @ref genWeightsInplace when neither Eigen nor Armadillo is enabled (SCons flags
   * @c USE_EIGEN / @c USE_ARMADILLO). Public to allow cross-checking against
   * @ref genFejerWeightsFFTInplace.
   *
   * @param out Output: the weights on [0, 1].
   */
  static void genFejerWeightsNaiveInplace(std::span<double> out);

  /**
   * @brief Fejér's first rule (no boundary nodes) for @c out.size() nodes in
   * @f$O(n \log n)@f$ (requires Eigen or Armadillo, else throws @c base::dependency_exception).
   *
   * The cosine sums are a discrete Fourier transform of length @f$n@f$, evaluated by
   * Bluestein's algorithm as a cyclic convolution of power-of-two length @f$\ge n@f$ (three
   * complex FFTs), so the cost does not depend on the prime factors of @f$n@f$ (the doubling
   * growth gives @f$n = 2^{\ell+1} - 1@f$, e.g. the prime 131071 at level 16).
   *
   * @param out Output: the weights on [0, 1].
   */
  static void genFejerWeightsFFTInplace(std::span<double> out);

 private:
  /// @brief Weights of the node set with the boundary nodes (see the class documentation).
  static void genWeightsWithBoundaryInplace(std::span<double> out);

  /// @brief Fejér's first rule by the FFT if available, else by the direct sum.
  static void genFejerWeightsInplace(std::span<double> out);

  const bool includeBoundary_;  ///< Whether the node set includes 0 and 1.
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
