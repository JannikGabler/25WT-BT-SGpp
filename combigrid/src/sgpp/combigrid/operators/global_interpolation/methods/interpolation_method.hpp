// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file interpolation_method.hpp
 * @brief Abstract interface for 1D interpolation methods used by the global
 * tensor-product interpolation operator.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Abstract base class for 1D interpolation methods (e.g.
 * Lagrange, barycentric Lagrange).
 *
 * A concrete subclass implements @ref interpolate, which evaluates the
 * Lagrange (or equivalent) interpolant defined by @c (nodes, values) at
 * the requested position. A unique numeric id identifies equivalent
 * methods so that the higher-level cache structures can deduplicate.
 *
 * For vector-valued source functions, the operators interpolate m outputs at once with the
 * overload taking @c nOutputs. Its default applies the scalar @ref interpolate to every output;
 * a subclass may override it to share the work per node between the outputs (the built-in
 * barycentric formulas do).
 *
 * The interpolant is assumed to be linear in the values (as every Lagrange-type rule):
 * @ref basisValues returns its weights @f$L_i(x)@f$, so that the interpolant equals
 * @f$\sum_i L_i(x) f_i@f$ up to round-off. The global interpolation on a @c DeltasSparseGrid
 * combines these weights across tensor grids instead of interpolating the values of every tensor
 * grid.
 */
class InterpolationMethod {
 public:
  /**
   * @brief Constructs the method with the given identifier.
   * @param id Unique numeric id for this method.
   */
  InterpolationMethod(uint64_t id);

  virtual ~InterpolationMethod() = default;

  /**
   * @brief Evaluates the 1D interpolant at @p pos.
   *
   * @param pos    Evaluation position.
   * @param nodes  1D node positions (size @c n).
   * @param values Values at the corresponding nodes (size @c n).
   * @return Interpolated value at @p pos.
   */
  virtual double interpolate(double pos, const std::vector<double>& nodes,
                             const std::vector<double>& values) const = 0;

  /**
   * @brief Evaluates the 1D interpolants of m outputs at @p pos.
   *
   * Output @c k of the result is the interpolant of the values @c values[i * m + k] of the nodes
   * @c i, evaluated at @p pos. The default applies the scalar @ref interpolate to every output
   * (gathering its values first); overrides must equal it up to round-off.
   *
   * @note A subclass that overrides only the scalar overload hides this one for calls through
   * the subclass type (add <tt>using InterpolationMethod::interpolate;</tt>); calls through an
   * @c InterpolationMethod pointer or reference are unaffected.
   *
   * @param pos      Evaluation position.
   * @param nodes    1D node positions (size @c n).
   * @param values   m values per node, node-major (@c values[i * m + k]; size @c n * m).
   * @param nOutputs Number of outputs m.
   * @param out      Output: the m interpolated values.
   */
  virtual void interpolate(double pos, const std::vector<double>& nodes,
                           const std::vector<double>& values, size_t nOutputs, double* out) const;

  /**
   * @brief Computes the weights of the values in the 1D interpolant at @p pos (for a Lagrange
   * rule, the Lagrange basis polynomials at @p pos): @c interpolate(pos, nodes, values) equals
   * @f$\sum_i@f$ @c out[i] * @c values[i] up to round-off, for all @p values.
   *
   * The default probes the scalar @ref interpolate with every unit vector, i.e. costs n
   * interpolations (@f$O(n^2)@f$ for a rule with @f$O(n)@f$ work); the built-in barycentric
   * formulas override it with one pass over the nodes. An override must handle the cases of
   * @ref hasTrivialResult like @ref interpolate (see @ref hasTrivialBasisValues).
   *
   * @param pos   Evaluation position.
   * @param nodes 1D node positions (size @c n).
   * @param out   Output: the n weights (size @c n).
   */
  virtual void basisValues(double pos, const std::vector<double>& nodes,
                           std::span<double> out) const;

  /// @brief Returns the unique identifier of this method.
  uint64_t id() const;

 protected:
  /**
   * @brief Detects trivial cases where the result follows immediately from
   * the inputs.
   *
   * Currently handles two cases:
   * - @p nodes contains a single entry (the result is that entry's value);
   * - @p pos coincides (up to floating-point tolerance) with one of the
   *   @p nodes (the result is the corresponding value).
   *
   * @param pos    Evaluation position.
   * @param nodes  1D node positions.
   * @param values Values at the nodes.
   * @param out    Out parameter receiving the trivial result, if any.
   * @return @c true if a trivial result was written into @p out.
   */
  bool hasTrivialResult(double pos, const std::vector<double>& nodes,
                        const std::vector<double>& values, double& out) const;

  /**
   * @brief @ref hasTrivialResult for m values per node: detects the same cases and writes the m
   * results of output @c k = 0, ..., m - 1 (the same operations per output as the scalar
   * overload).
   *
   * @param pos      Evaluation position.
   * @param nodes    1D node positions.
   * @param values   m values per node, node-major (@c values[i * m + k]).
   * @param nOutputs Number of outputs m.
   * @param out      Output receiving the m trivial results, if any.
   * @return @c true if trivial results were written into @p out.
   */
  bool hasTrivialResult(double pos, const std::vector<double>& nodes,
                        const std::vector<double>& values, size_t nOutputs, double* out) const;

  /**
   * @brief @ref hasTrivialResult for the weights of @ref basisValues: detects the same cases in
   * the same order and writes the weights of their results (one node: 1; two nodes: the weights
   * of the linear interpolant; @p pos on a node: the unit vector of that node).
   *
   * @param pos   Evaluation position.
   * @param nodes 1D node positions.
   * @param out   Output receiving the n weights, if the case is trivial.
   * @return @c true if the weights were written into @p out.
   */
  bool hasTrivialBasisValues(double pos, const std::vector<double>& nodes,
                             std::span<double> out) const;

 private:
  const uint64_t id_;  ///< Unique identifier.
};

}  // namespace combigrid
}  // namespace sgpp
