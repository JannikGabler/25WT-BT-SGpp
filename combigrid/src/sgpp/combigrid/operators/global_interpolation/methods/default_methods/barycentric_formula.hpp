// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file barycentric_formula.hpp
 * @brief Generic barycentric Lagrange interpolation method.
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
 * @brief Barycentric Lagrange interpolation (second form) for arbitrary
 * node families.
 *
 * The barycentric weights @f$w_i@f$ are computed from the given nodes; given a query @f$x@f$ the
 * interpolant is evaluated as
 * @f[
 *   p(x) = \frac{\sum_i \tfrac{w_i}{x - x_i}\, f_i}
 *               {\sum_i \tfrac{w_i}{x - x_i}}.
 * @f]
 */
class BarycentricFormula : public InterpolationMethod {
 public:
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
