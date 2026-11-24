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
#include <ranges>
#include <sgpp/combigrid/miscellaneous/cpp_concepts/cpp_concepts.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace numerics {

/**
 * TODO: Document (update)
 * @brief Returns the compensated sum (@ref CompensatedSum) of @p values in index order.
 *
 * @tparam R     Contiguous range of arithmetic values (converted to double), e.g.
 *               @c std::span<const double> or @c std::vector<int>.
 * @param values Terms.
 * @return The compensated sum (0 for no terms).
 */
template <misc::std_real_type OType, misc::static_castable_to<OType> IType>
OType compensatedSum(const std::span<const IType> values) {
  CompensatedSum<OType> sum;

  for (const IType value : values) {
    sum.add(static_cast<OType>(value));
  }

  return sum.value();
}

/**
 * TODO: Document (update)
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
template <misc::std_real_type OType, misc::static_castable_to<OType> IType1,
          misc::static_castable_to<OType> IType2>
OType compensatedDotProduct(const std::span<const IType1> a, const std::span<const IType2> b) {
  assert(std::ranges::size(a) == std::ranges::size(b));

  const size_t n = std::ranges::size(a);

  CompensatedSum<OType> sum;

  for (size_t i = 0; i < n; i++) {
    sum.addProduct(static_cast<OType>(a[i]), static_cast<OType>(b[i]));
  }

  return sum.value();
}

/**
 * TODO: Document (update)
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
template <misc::std_real_type OType, misc::static_castable_to<OType> IType1,
          misc::static_castable_to<OType> IType2>
void compensatedDotProducts(const std::span<const IType1> a, const std::span<const IType2> b,
                            const std::span<OType> result) {
  assert(std::ranges::size(b) == std::ranges::size(a) * result.size());

  const size_t n = std::ranges::size(a);
  const size_t nOutputs = result.size();

  std::vector<CompensatedSum<OType>> sums(nOutputs);

  for (size_t i = 0; i < n; i++) {
    const OType factor = static_cast<OType>(a[i]);
    const std::span<const IType2> row = b.subspan(i * nOutputs, nOutputs);

    for (size_t k = 0; k < nOutputs; k++) {
      sums[k].addProduct(factor, static_cast<OType>(row[k]));
    }
  }

  for (size_t k = 0; k < nOutputs; k++) {
    result[k] = sums[k].value();
  }
}

}  // namespace numerics
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
