/**
 * @file barycentric_formula.hpp
 * @brief Generic barycentric Lagrange interpolation method.
 */
#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace global_interpolation {

/**
 * @brief Barycentric Lagrange interpolation (second form) for arbitrary
 * node families.
 *
 * The barycentric weights @f$w_i@f$ are derived from the node family
 * provided by the associated @c NodeGenFunc; given a query @f$x@f$ the
 * interpolant is evaluated as
 * @f[
 *   p(x) = \frac{\sum_i \tfrac{w_i}{x - x_i}\, f_i}
 *               {\sum_i \tfrac{w_i}{x - x_i}}.
 * @f]
 */
class BarycentricFormula : public InterpolationMethod {
 public:
  /**
   * @brief Constructs the formula and binds it to a node-generation function.
   * @param nodeGenFunc Non-owning pointer used as identity for cache keying.
   */
  BarycentricFormula(const NodeGenFunc* nodeGenFunc);

  /// @copydoc InterpolationMethod::interpolate
  double interpolate(const double pos, const std::vector<double>& nodes,
                     const std::vector<double>& values) const override;

 private:
  // TODO: Optimize (caching) (double long could be used in pre-computation)
  /**
   * @brief Evaluates the barycentric formula directly (no caching).
   * @param pos    Evaluation position.
   * @param nodes  1D node positions.
   * @param values Values at the nodes.
   * @return Interpolated value.
   */
  double evaluateFormula(const double pos, const std::vector<double>& nodes,
                         const std::vector<double>& values) const;
};

}  // namespace global_interpolation

}  // namespace combigrid
}  // namespace sgpp
