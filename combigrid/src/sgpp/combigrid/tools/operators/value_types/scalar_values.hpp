// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file scalar_values.hpp
 * @brief Value type "one value per grid point" (@c SourceFunc) of the operator kernels.
 *
 * The operator kernels are written once as templates over a value type, which evaluates the
 * source function at a grid point and accumulates weighted values: @c tools::ScalarValues (this
 * file; one @c double, the instantiation compiles to the scalar kernel) and
 * @c tools::VectorValues (m values per grid point, see vector_values.hpp). Both are small views
 * that the kernels hold by value, so that their data stays in registers across the opaque calls
 * of the source function. Interface used by the kernels:
 * - @c Acc: accumulator of the values of all outputs, created by @c zeroAcc(slot);
 * - @c evaluate(point): the values at a point (in problem coordinates);
 * - @c addScaled(acc, weight, values): @c acc += weight * values (values: of @c evaluate or an
 *   accumulator);
 * - @c store(acc, out): writes the accumulated values of all outputs to @c out.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

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

  /// @brief @p acc += @p weight * @p value.
  static void addScaled(Acc& acc, const double weight, const double value) {
    acc += weight * value;
  }

  /// @brief Writes the accumulated value to @p out (one @c double).
  static void store(const Acc acc, double* const out) { *out = acc; }

 private:
  const SourceFunc* sourceFunc;  ///< Viewed source function.
};

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
