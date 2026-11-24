// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file compensated_sum_tools.hpp
 * @brief Compensated sums and dot products of contiguous ranges, built on the accumulator
 * @c CompensatedSum (compensated_sum.hpp), e.g. the combination @f$\sum_\ell c_\ell v_\ell@f$
 * of per-tensor-grid results, accurate also when the terms cancel.
 *
 * Header-only templates: the accumulator's steps are inlined into the loop (one term costs a few
 * floating-point operations, a call per term would dominate). The floating-point type of the
 * summation is always given explicitly (e.g. @c compensatedDotProduct<double>(a, b)); the range
 * types are deduced.
 */
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/miscellaneous/cpp_concepts/cpp_concepts.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace numerics {

/**
 * @brief Returns the compensated sum (@ref CompensatedSum) of @p values in index order.
 *
 * @tparam OType Floating-point type of the summation and the result (given explicitly).
 * @tparam R     Contiguous range of numbers, each converted to @p OType, e.g.
 *               @c std::vector<double>, @c std::span<const int> or @c base::DataVector.
 * @param values Terms.
 * @return The compensated sum (0 for no terms).
 */
template <misc::StdFloatingPoint OType, misc::ContiguousRangeCastableTo<OType> R>
OType compensatedSum(const R& values) {
  CompensatedSum<OType> sum;

  for (const auto value : std::span(values)) {
    sum.add(static_cast<OType>(value));
  }

  return sum.value();
}

/**
 * @brief Returns @f$\sum_i a_i b_i@f$ with compensated summation (@ref CompensatedSum) in index
 * order, e.g. the combination of per-tensor-grid results @p b with the combination-technique
 * coefficients @p a.
 *
 * @tparam OType Floating-point type of the summation and the result (given explicitly).
 * @tparam RA    Contiguous range of numbers, each converted to @p OType (integers beyond the
 *               precision of @p OType, e.g. @f$2^{53}@f$ for @c double, are rounded), e.g.
 *               @c std::span<const CTCoeffType>.
 * @tparam RB    Contiguous range of numbers, each converted to @p OType.
 * @param a      Factors @f$a_i@f$ (size n).
 * @param b      Factors @f$b_i@f$ (size n).
 * @return The compensated sum (0 for n = 0).
 */
template <misc::StdFloatingPoint OType, misc::ContiguousRangeCastableTo<OType> RA,
          misc::ContiguousRangeCastableTo<OType> RB>
OType compensatedDotProduct(const RA& a, const RB& b) {
  const std::span aSpan(a);
  const std::span bSpan(b);
  assert(aSpan.size() == bSpan.size());

  CompensatedSum<OType> sum;

  for (size_t i = 0; i < aSpan.size(); i++) {
    sum.addProduct(static_cast<OType>(aSpan[i]), static_cast<OType>(bSpan[i]));
  }

  return sum.value();
}

/// @brief Number of accumulators that @ref compensatedDotProducts and @ref compensatedSums keep
/// on the stack per pass over the terms (more outputs take further passes).
inline constexpr size_t COMPENSATED_SUMS_PER_PASS = 8;

/**
 * @brief Common loop of @ref compensatedDotProducts and @ref compensatedSums: for every block of
 * up to @ref COMPENSATED_SUMS_PER_PASS outputs, one pass over the n rows of the span @p b (m
 * values each, m = @p result.size()) calls @p addTerm(sum of output k, row i, value of output k
 * in row i) and writes the values of the sums to @p result.
 */
template <misc::StdFloatingPoint OType, typename BSpan, typename AddTerm>
void compensatedSumsOfRows(const BSpan b, const size_t nRows, const std::span<OType> result,
                           const AddTerm& addTerm) {
  const size_t nOutputs = result.size();
  assert(b.size() == nRows * nOutputs);

  for (size_t first = 0; first < nOutputs; first += COMPENSATED_SUMS_PER_PASS) {
    const size_t nSums = std::min(COMPENSATED_SUMS_PER_PASS, nOutputs - first);
    std::array<CompensatedSum<OType>, COMPENSATED_SUMS_PER_PASS> sums{};

    for (size_t i = 0; i < nRows; i++) {
      const auto* const row = b.data() + i * nOutputs + first;

      for (size_t k = 0; k < nSums; k++) {
        addTerm(sums[k], i, static_cast<OType>(row[k]));
      }
    }

    for (size_t k = 0; k < nSums; k++) {
      result[first + k] = sums[k].value();
    }
  }
}

/**
 * @brief @ref compensatedDotProduct for m outputs: @p result[k] = @f$\sum_i a_i b_{i m + k}@f$
 * for k < m.
 *
 * Every output equals @ref compensatedDotProduct of @p a with the values of that output bitwise.
 * Up to @ref COMPENSATED_SUMS_PER_PASS outputs are summed in one pass over @p b (m separate dot
 * products with stride m would read it m times); no heap allocation.
 *
 * @tparam OType Floating-point type of the summation and the results (given explicitly, or
 *               deduced from @p result if it is a @c std::span<OType>).
 * @tparam RA    Contiguous range of numbers, each converted to @p OType.
 * @tparam RB    Contiguous range of numbers, each converted to @p OType, e.g. a
 *               @c FlatNestedVec<double, 2> with one sub-vector of m values per index i.
 * @param a      Factors @f$a_i@f$ (size n).
 * @param b      Factors, index-major: the m values of index i at i m, ..., i m + m - 1 (size n m).
 * @param result Output: the m compensated sums (size m).
 */
template <misc::StdFloatingPoint OType, misc::ContiguousRangeCastableTo<OType> RA,
          misc::ContiguousRangeCastableTo<OType> RB>
void compensatedDotProducts(const RA& a, const RB& b, const std::span<OType> result) {
  const std::span aSpan(a);
  const std::span bSpan(b);

  compensatedSumsOfRows<OType>(bSpan, aSpan.size(), result,
                               [&](CompensatedSum<OType>& sum, const size_t i, const OType value) {
                                 sum.addProduct(static_cast<OType>(aSpan[i]), value);
                               });
}

/**
 * @brief @ref compensatedSum for m outputs: @p result[k] = @f$\sum_i b_{i m + k}@f$ for k < m.
 *
 * Every output equals @ref compensatedSum of the values of that output bitwise; one pass over
 * @p b per @ref COMPENSATED_SUMS_PER_PASS outputs, no heap allocation.
 *
 * @tparam OType Floating-point type of the summation and the results (given explicitly, or
 *               deduced from @p result if it is a @c std::span<OType>).
 * @tparam R     Contiguous range of numbers, each converted to @p OType.
 * @param values Terms, index-major: the m values of index i at i m, ..., i m + m - 1 (size n m).
 * @param result Output: the m compensated sums (size m).
 */
template <misc::StdFloatingPoint OType, misc::ContiguousRangeCastableTo<OType> R>
void compensatedSums(const R& values, const std::span<OType> result) {
  const std::span valueSpan(values);
  const size_t nRows = result.empty() ? 0 : valueSpan.size() / result.size();

  compensatedSumsOfRows<OType>(
      valueSpan, nRows, result,
      [](CompensatedSum<OType>& sum, size_t, const OType value) { sum.add(value); });
}

}  // namespace numerics
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
