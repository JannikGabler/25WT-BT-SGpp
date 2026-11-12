// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file optimized_barycentric_formula.hpp
 * @brief Barycentric Lagrange formula specialized for Clenshaw-Curtis nodes.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <span>
#include <vector>

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

  /**
   * @brief Evaluates the 1D interpolant at @p pos (scalar formula).
   * @param pos    Evaluation position.
   * @param nodes  1D node positions (size @c n).
   * @param values Values at the corresponding nodes (size @c n).
   * @return Interpolated value at @p pos.
   */
  double interpolate(const double pos, const std::vector<double>& nodes,
                     const std::vector<double>& values) const override;

  /**
   * @brief Evaluates the 1D interpolants of m outputs at @p pos: the factors of the nodes are
   * computed once, then the outputs one after the other with the operations of the scalar
   * formula.
   *
   * @param pos      Evaluation position.
   * @param nodes    1D node positions (size @c n).
   * @param values   m values per node, node-major (@c values[i * m + k]; size @c n * m).
   * @param nOutputs Number of outputs m.
   * @param out      Output: the m interpolated values.
   */
  void interpolate(const double pos, const std::vector<double>& nodes,
                   const std::vector<double>& values, size_t nOutputs, double* out) const override;

  /**
   * @brief Computes the weights of the values at @p pos (the Lagrange basis polynomials) in one
   * pass: the factors @f$w_i / (pos - x_i)@f$ of the formula, each divided by their sum.
   * @param pos   Evaluation position.
   * @param nodes 1D node positions (size @c n).
   * @param out   Output: the n weights.
   */
  void basisValues(const double pos, const std::vector<double>& nodes,
                   std::span<double> out) const override;

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
