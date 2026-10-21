/**
 * @file clenshaw_curtis_quadrature_rule.hpp
 * @brief 1D Clenshaw-Curtis quadrature rule on Chebyshev-Lobatto nodes.
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>

// An FFT backend for ClenshawCurtisQuadRule::genWeightsFFT is available if Eigen or Armadillo is
// enabled via the SCons flags USE_EIGEN / USE_ARMADILLO
#if defined(USE_EIGEN) || defined(USE_ARMADILLO)
#define SGPP_COMBIGRID_HAVE_FFT 1
#endif

namespace sgpp {
namespace combigrid {

/**
 * @brief Concrete @c QuadRule implementations.
 */
namespace quadrature_rules {

/**
 * @brief Clenshaw-Curtis quadrature on Chebyshev-Lobatto nodes.
 *
 * Clenshaw-Curtis is the canonical quadrature for nested Chebyshev-type
 * node families and is the default quadrature rule used together with the
 * Clenshaw-Curtis node generator.
 */
class ClenshawCurtisQuadRule : public QuadRule {
 public:
  /// @brief Constructs the rule with a deterministic id derived from its name.
  ClenshawCurtisQuadRule() : QuadRule(tools::fnv1aHash("Clenshaw Curtis Quadrature Rule")) {}

  /// @copydoc QuadRule::genWeights
  base::DataVector genWeights(const GPCntType nNodes) const override;

  /// @copydoc QuadRule::genWeightsInplace
  void genWeightsInplace(std::span<double> out) const override;

  /**
   * @brief Naive @f$O(n^2)@f$ weight computation.
   *
   * Used by @ref genWeights when neither Eigen nor Armadillo is enabled
   * (SCons flags @c USE_EIGEN / @c USE_ARMADILLO). Public to allow cross-checking against @ref genWeightsFFT.
   *
   * @param nNodes Number of 1D nodes (@c >= 2).
   * @return Newly allocated weight vector.
   */
  base::DataVector genWeightsNaive(const size_t nNodes) const;

#ifdef SGPP_COMBIGRID_HAVE_FFT
  /**
   * @brief Waldvogel's FFT-based weight formula (requires Eigen or Armadillo).
   *
   * Faster (@f$O(n \log n)@f$) than @ref genWeightsNaive for large
   * @p nNodes. Only declared if @c SGPP_COMBIGRID_HAVE_FFT is defined, i.e. if
   * SG++ is built with @c USE_EIGEN=1 or @c USE_ARMADILLO=1. If both are set,
   * Eigen's FFT is used (measured 1.2-3.7x faster than Armadillo's).
   *
   * @param nNodes Number of 1D nodes (@c >= 2).
   * @return Newly allocated weight vector.
   */
  base::DataVector genWeightsFFT(const size_t nNodes) const;
#endif
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
