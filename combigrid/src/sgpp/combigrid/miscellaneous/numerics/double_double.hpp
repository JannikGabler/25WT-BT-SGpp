// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file double_double.hpp
 * @brief Defines @c DoubleDouble, a number stored as the unevaluated sum of two doubles, for sums
 * whose terms cancel.
 */
#pragma once

#include <cmath>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Unevaluated sum @c hi + @c lo of two doubles (double-double), with about twice the
 * precision of a double.
 *
 * For sums of many products that cancel: e.g. the combination of 1D weights with the
 * combination-technique coefficients, which are large integers of alternating sign. Accumulated
 * in double, their round-off can exceed the result by far; in double-double, the result
 * (@ref value) is correct to about one rounding. The operations use error-free transformations
 * (FMA for the product, TwoSum for the sum) and assume round-to-nearest without contraction of
 * the other multiplications and additions.
 *
 * An aggregate: @c DoubleDouble{x, 0.0} represents the double @c x.
 */
struct DoubleDouble {
  double hi;  ///< High part (the sum rounded to double, up to the round-off of the operations).
  double lo;  ///< Low part (the rounding error of @c hi).

  /**
   * @brief Product with the double @p factor; the rounding error of @c hi * @p factor is exact
   * (FMA).
   * @param factor Factor.
   * @return The product.
   *
   * @note Defined in the header for inlining.
   */
  DoubleDouble operator*(const double factor) const {
    const double product = hi * factor;
    return {product, std::fma(hi, factor, -product) + lo * factor};
  }

  /**
   * @brief Adds @p term; the rounding error of the sum of the high parts is exact (TwoSum).
   * @param term Term to add.
   * @return @c *this.
   *
   * @note Defined in the header for inlining.
   */
  DoubleDouble& operator+=(const DoubleDouble& term) {
    // TwoSum of the high parts: sum + error == hi + term.hi exactly
    const double sum = hi + term.hi;
    const double termPart = sum - hi;
    const double error = (hi - (sum - termPart)) + (term.hi - termPart);
    hi = sum;
    lo += error + term.lo;
    return *this;
  }

  /// @brief Returns the value rounded to double (@c hi + @c lo).
  double value() const { return hi + lo; }
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
