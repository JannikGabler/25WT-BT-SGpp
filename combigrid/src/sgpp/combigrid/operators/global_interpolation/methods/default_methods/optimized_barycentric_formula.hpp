/**
 * @file optimized_barycentric_formula.hpp
 * @brief Barycentric Lagrange formula specialized for Clenshaw-Curtis nodes.
 */
#pragma once

#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

namespace global_interpolation {

/**
 * @brief Barycentric Lagrange interpolation specialized for Clenshaw-Curtis
 * nodes.
 *
 * Uses the closed-form barycentric weights for Clenshaw-Curtis nodes
 * (alternating signs with halved magnitudes at the endpoints), which
 * removes the dependency on a specific @c NodeGenFunc instance and
 * avoids re-deriving weights at runtime.
 */
class OptBarycentricFormula : public InterpolationMethod {
 public:
  /// @brief Constructs the formula with a deterministic identity.
  OptBarycentricFormula();

  /// @copydoc InterpolationMethod::interpolate
  double interpolate(const double pos, const std::vector<double>& nodes,
                     const std::vector<double>& values) const override;

 private:
  /**
   * @brief Evaluates the specialized Clenshaw-Curtis barycentric formula.
   * @param pos    Evaluation position.
   * @param nodes  Clenshaw-Curtis node positions.
   * @param values Values at the nodes.
   * @return Interpolated value.
   */
  double barycentricFormula(const double pos, const std::vector<double>& nodes,
                            const std::vector<double>& values) const;
};

}  // namespace global_interpolation

}  // namespace combigrid
}  // namespace sgpp
