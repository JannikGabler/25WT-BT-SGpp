// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file compensated_sum.hpp
 * @brief Compensated (Neumaier) summation in double: the accumulator @c CompensatedSum and, built
 * on it, compensated sums and dot products of spans, e.g. the combination
 * @f$\sum_\ell c_\ell v_\ell@f$ of per-tensor-grid results, accurate also when the terms cancel.
 *
 * Header-only: the accumulator's steps are inlined into the caller's loop (one term costs a few
 * floating-point operations, a call per term would dominate), and the span functions are
 * templates for every contiguous range of arithmetic values.
 */
#pragma once

#include <cassert>
#include <cmath>
#include <ranges>
#include <sgpp/combigrid/tools/concepts/arithmetic.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Accumulator of a compensated sum in double: Neumaier's summation, for products with the
 * exact rounding error of the product (FMA).
 *
 * The terms are summed in the order of the calls. The rounded sum and a second sum of the
 * rounding errors (of the additions: the smaller operand loses the bits, Neumaier; of the
 * products: FMA) are kept and added in @ref value. The result is as accurate as if the sum were
 * computed in about twice the double precision and then rounded: its relative error is about one
 * rounding unit plus @f$\varepsilon^2@f$ times the condition number
 * @f$\sum_i |t_i| / |\sum_i t_i|@f$ of the terms @f$t_i@f$. A plain loop has an error of up to
 * @f$n\,\varepsilon@f$ times the condition number. Example: the combination-technique
 * coefficients are large integers of alternating sign; on complete sparse grids in d = 15
 * dimensions with about @f$5 \cdot 10^4@f$ tensor grids the condition number of the combination
 * reaches about @f$10^6@f$.
 *
 * The result depends only on the terms and their order (deterministic, independent of FMA
 * contraction by the compiler). Cost: about twice a plain loop (latency-bound chain of additions;
 * about 2.2 ns per product at 2 GHz). Assumes round-to-nearest and no value-changing
 * optimizations (e.g. @c -ffast-math).
 */
class CompensatedSum {
 public:
  /// @brief Adds @p term.
  void add(const double term) {
    const double newSum = sum_ + term;
    compensation_ +=
        (std::abs(sum_) >= std::abs(term)) ? (sum_ - newSum) + term : (term - newSum) + sum_;
    sum_ = newSum;
  }

  /// @brief Adds the product @p factor1 * @p factor2, including its exact rounding error.
  void addProduct(const double factor1, const double factor2) {
    const double product = factor1 * factor2;
    const double productError = std::fma(factor1, factor2, -product);
    const double newSum = sum_ + product;
    const double sumError = (std::abs(sum_) >= std::abs(product)) ? (sum_ - newSum) + product
                                                                  : (product - newSum) + sum_;
    compensation_ += productError + sumError;
    sum_ = newSum;
  }

  /// @brief Returns the compensated sum of the terms added so far (0 if none).
  double value() const { return sum_ + compensation_; }

 private:
  double sum_ = 0.0;           ///< Sum of the rounded terms, rounded.
  double compensation_ = 0.0;  ///< Sum of the rounding errors.
};

/**
 * @brief Returns the compensated sum (@ref CompensatedSum) of @p values in index order.
 *
 * @tparam R     Contiguous range of arithmetic values (converted to double), e.g.
 *               @c std::span<const double> or @c std::vector<int>.
 * @param values Terms.
 * @return The compensated sum (0 for no terms).
 */
template <ArithmeticRange R>
double compensatedSum(const R& values) {
  CompensatedSum sum;
  for (const auto value : values) {
    sum.add(static_cast<double>(value));
  }
  return sum.value();
}

/**
 * @brief Returns @f$\sum_i a_i b_i@f$ with compensated summation (@ref CompensatedSum) in index
 * order, e.g. the combination of per-tensor-grid results @p b with the combination-technique
 * coefficients @p a.
 *
 * @tparam RA Contiguous range of arithmetic values (converted to double; integers beyond
 *            @f$2^{53}@f$ in magnitude are rounded), e.g. @c std::span<const CTCoeffType>.
 * @tparam RB Contiguous range of arithmetic values (converted to double).
 * @param a   Factors @f$a_i@f$ (size n).
 * @param b   Factors @f$b_i@f$ (size n).
 * @return The compensated sum (0 for n = 0).
 */
template <ArithmeticRange RA, ArithmeticRange RB>
double compensatedDotProduct(const RA& a, const RB& b) {
  const size_t n = std::ranges::size(a);
  assert(std::ranges::size(b) == n);
  const auto* const aData = std::ranges::data(a);
  const auto* const bData = std::ranges::data(b);

  CompensatedSum sum;
  for (size_t i = 0; i < n; i++) {
    sum.addProduct(static_cast<double>(aData[i]), static_cast<double>(bData[i]));
  }
  return sum.value();
}

/**
 * @brief @ref compensatedDotProduct for m outputs in one pass over @p b: @p result[k] =
 * @f$\sum_i a_i b_{i m + k}@f$ for k < m.
 *
 * Every output equals @ref compensatedDotProduct of @p a with the values of that output
 * bitwise. One pass reads @p b once (m separate dot products with stride m would read it m times).
 *
 * @tparam RA    Contiguous range of arithmetic values (converted to double).
 * @tparam RB    Contiguous range of arithmetic values (converted to double).
 * @param a      Factors @f$a_i@f$ (size n).
 * @param b      Factors, index-major: the m values of index i at i m, ..., i m + m - 1 (size n m).
 * @param result Output: the m compensated sums (size m).
 */
template <ArithmeticRange RA, ArithmeticRange RB>
void compensatedDotProducts(const RA& a, const RB& b, const std::span<double> result) {
  const size_t n = std::ranges::size(a);
  const size_t nOutputs = result.size();
  assert(std::ranges::size(b) == n * nOutputs);
  const auto* const aData = std::ranges::data(a);
  const auto* const bData = std::ranges::data(b);

  std::vector<CompensatedSum> sums(nOutputs);
  for (size_t i = 0; i < n; i++) {
    const double factor = static_cast<double>(aData[i]);
    const auto* const row = bData + i * nOutputs;
    for (size_t k = 0; k < nOutputs; k++) {
      sums[k].addProduct(factor, static_cast<double>(row[k]));
    }
  }
  for (size_t k = 0; k < nOutputs; k++) {
    result[k] = sums[k].value();
  }
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
