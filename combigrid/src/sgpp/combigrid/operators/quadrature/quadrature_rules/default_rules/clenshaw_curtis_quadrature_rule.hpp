// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

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
#include <span>

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
   * (SCons flags @c USE_EIGEN / @c USE_ARMADILLO). Public to allow cross-checking against
   * @ref genWeightsFFT.
   *
   * @param nNodes Number of 1D nodes (@c >= 2).
   * @return Newly allocated weight vector.
   */
  base::DataVector genWeightsNaive(const size_t nNodes) const;

  /**
   * @brief @ref genWeightsNaive "genWeightsNaive(out.size())" writing into @p out.
   * @param out Output: the weights (at least 2).
   */
  void genWeightsNaiveInplace(const std::span<double> out) const;

  /**
   * @brief Waldvogel's FFT-based weight formula (requires Eigen or Armadillo).
   *
   * Faster (@f$O(n \log n)@f$) than @ref genWeightsNaive for large @p nNodes: one real FFT of
   * length @p nNodes - 1.
   * If both are set, Eigen's FFT is used (measured 1.2-3.7x faster than Armadillo's).
   *
   * @param nNodes Number of 1D nodes (@c >= 2).
   * @return Newly allocated weight vector.
   */
  base::DataVector genWeightsFFT(const size_t nNodes) const;

  /**
   * @brief @ref genWeightsFFT "genWeightsFFT(out.size())" writing into @p out.
   * @param out Output: the weights (at least 2).
   */
  void genWeightsFFTInplace(std::span<double> out) const;
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
