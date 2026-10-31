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

 private:
  const uint64_t id_;  ///< Unique identifier.
};

}  // namespace combigrid
}  // namespace sgpp
