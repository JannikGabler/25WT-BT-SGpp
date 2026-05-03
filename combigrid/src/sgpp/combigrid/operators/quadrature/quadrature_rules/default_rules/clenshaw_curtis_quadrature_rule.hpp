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

  /// @copydoc QuadRule::getWeights
  base::DataVector getWeights(const GPCntType nNodes) const override;

  /// @copydoc QuadRule::genWeightsInplace
  void genWeightsInplace(const GPCntType nNodes, base::DataVector& out,
                         size_t startIdx = 0) const override;

 private:
  /**
   * @brief Naive @f$O(n^2)@f$ weight computation.
   *
   * Used as a fallback when neither Eigen nor Armadillo is available at
   * build time.
   *
   * @param nNodes Number of 1D nodes.
   * @return Newly allocated weight vector.
   */
  base::DataVector genWeightsNaive(const size_t nNodes) const;

  /**
   * @brief Waldvogel's FFT-based weight formula (requires Eigen or Armadillo).
   *
   * Faster (@f$O(n \log n)@f$) than @ref genWeightsNaive for large
   * @p nNodes.
   *
   * @param nNodes Number of 1D nodes.
   * @return Vector of @p nNodes weights.
   */
  std::vector<double> clenshawCurtisWeightsFFT(const size_t nNodes);
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp
