// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file scalar_values.hpp
 * @brief Value type "one value per grid point" (@c SourceFunc) of the operator kernels.
 *
 * The operator kernels are written once as templates over a value type, which evaluates the
 * source function at a grid point and accumulates weighted values: @c misc::ScalarValues (this
 * file; one @c double, the instantiation compiles to the scalar kernel) and
 * @c misc::VectorValues (m values per grid point, see vector_values.hpp). Both are small views
 * that the kernels hold by value, so that their data stays in registers across the opaque calls
 * of the source function. Interface used by the kernels:
 * - @c Acc: accumulator of the values of all outputs, created by @c zeroAcc(slot);
 * - @c evaluate(point): the values at a point (in problem coordinates);
 * - @c evaluateNormalizedInPlace(point, area): the values at a normalized point (mapped onto
 *   @c area in place, as @c SourceFunc::evaluateNormalizedInPlace);
 * - @c evaluateNormalizedInPlace(point, area, dst, idx): the same, written to slot @c idx of the
 *   vector @c dst (m doubles per slot; the scalar instantiation is @c dst[idx] = value);
 * - @c addScaled(acc, weight, values): @c acc += weight * values (values: of @c evaluate or an
 *   accumulator);
 * - @c store(acc, out): writes the accumulated values of all outputs to @c out;
 * - @c interpolate(method, pos, nodes, values, dst, idx): the 1D interpolants of all outputs
 *   (values: m per node, node-major) at @c pos, written to slot @c idx of @c dst.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Value type of the operator kernels for a scalar source function (one value per grid
 * point; see scalar_values.hpp).
 */
class ScalarValues {
 public:
  /// @brief Accumulator: a plain @c double (kept in a register by the kernels).
  using Acc = double;

  /**
   * @brief Creates the view.
   * @param sourceFunc Source function (must outlive the view).
   */
  explicit ScalarValues(const SourceFunc& sourceFunc) : sourceFunc(&sourceFunc) {}

  /// @brief Number of outputs (1).
  static constexpr size_t nOutputs() { return 1; }

  /// @brief Returns a zero accumulator (the slot is used by @c VectorValues only).
  static constexpr Acc zeroAcc(size_t /*slot*/) { return 0.0; }

  /**
   * @brief Evaluates the source function.
   * @param point Point in problem coordinates.
   * @return Function value.
   */
  double evaluate(const base::DataVector& point) const { return sourceFunc->evaluate(point); }

  /**
   * @brief Evaluates the source function at a normalized point.
   * @param point Point in @f$[0,1]^d@f$, overwritten with its image in @p area.
   * @param area  Domain.
   * @return Function value.
   */
  double evaluateNormalizedInPlace(base::DataVector& point, const HyperCubeArea& area) const {
    return sourceFunc->evaluateNormalizedInPlace(point, area);
  }

  /// @brief As @ref evaluateNormalizedInPlace(base::DataVector&, const HyperCubeArea&) const,
  /// writing the value to slot @p idx of @p dst (@c dst[idx]).
  void evaluateNormalizedInPlace(base::DataVector& point, const HyperCubeArea& area,
                                 std::vector<double>& dst, const size_t idx) const {
    dst[idx] = sourceFunc->evaluateNormalizedInPlace(point, area);
  }

  /// @brief @p acc += @p weight * @p value.
  static void addScaled(Acc& acc, const double weight, const double value) {
    acc += weight * value;
  }

  /// @brief Writes the accumulated value to @p out (one @c double).
  static void store(const Acc acc, double* const out) { *out = acc; }

  /// @brief Writes the 1D interpolant of @p values at @p pos (scalar @p method) to slot @p idx
  /// of @p dst (@c dst[idx]).
  static void interpolate(const InterpolationMethod& method, const double pos,
                          const std::vector<double>& nodes, const std::vector<double>& values,
                          std::vector<double>& dst, const size_t idx) {
    dst[idx] = method.interpolate(pos, nodes, values);
  }

 private:
  const SourceFunc* sourceFunc;  ///< Viewed source function.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
