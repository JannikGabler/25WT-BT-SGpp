// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file compensated_sum.hpp
 * @brief Class for compensated (Neumaier) summation
 */
#pragma once

#include <cassert>
#include <cmath>
#include <sgpp/combigrid/miscellaneous/cpp_concepts/cpp_concepts.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace numerics {

/**
 * @brief Accumulator of a compensated sum: Neumaier's summation, for products with the
 * exact rounding error of the product (FMA).
 *
 * The terms are summed in the order of the calls. The rounded sum and a second sum of the
 * rounding errors (of the additions: the smaller operand loses the bits, Neumaier; of the
 * products: FMA) are kept and added in @ref value. The result is as accurate as if the sum were
 * computed in about twice the precision and then rounded: its relative error is about one
 * rounding unit plus @f$\varepsilon^2@f$ times the condition number
 * @f$\sum_i |t_i| / |\sum_i t_i|@f$ of the terms @f$t_i@f$. A plain loop has an error of up to
 * @f$n\,\varepsilon@f$ times the condition number. Example: the combination-technique
 * coefficients are large integers of alternating sign; on complete sparse grids in d = 15
 * dimensions with about @f$5 \cdot 10^4@f$ tensor grids the condition number of the combination
 * reaches about @f$10^6@f$.
 *
 * The result depends only on the terms and their order (deterministic, independent of FMA
 * contraction by the compiler). Cost: about twice a plain loop. Assumes round-to-nearest and no
 * value-changing optimizations (e.g. @c -ffast-math).
 */
template <misc::std_real_type T>
class CompensatedSum {
 public:
  /// @brief Adds @p term.
  void add(const T term) {
    const T newSum = sum_ + term;

    compensation_ +=
        (std::abs(sum_) >= std::abs(term)) ? (sum_ - newSum) + term : (term - newSum) + sum_;

    sum_ = newSum;
  }

  /// @brief Adds the product @p factor1 * @p factor2, including its exact rounding error.
  void addProduct(const T factor1, const T factor2) {
    const T product = factor1 * factor2;
    const T productError = std::fma(factor1, factor2, -product);
    const T newSum = sum_ + product;
    const T sumError = (std::abs(sum_) >= std::abs(product)) ? (sum_ - newSum) + product
                                                             : (product - newSum) + sum_;

    compensation_ += productError + sumError;
    sum_ = newSum;
  }

  /// @brief Returns the compensated sum of the terms added so far (0 if none).
  T value() const { return sum_ + compensation_; }

 private:
  T sum_ = 0.0;           ///< Sum of the rounded terms, rounded.
  T compensation_ = 0.0;  ///< Sum of the rounding errors.
};

}  // namespace numerics
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
