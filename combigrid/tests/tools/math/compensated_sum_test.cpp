// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file compensated_sum_test.cpp
 * @brief Boost.Test cases for the compensated dot products of @c
 * tools::compensatedDotProduct<double, CTCoeffType, double>(s): exact cancellation, exact product
 * errors, accuracy on ill-conditioned combinations of alternating integer coefficients (against a
 * double-double reference), the one-pass multi-output variant, other element / range types and the
 * accumulator @c CompensatedSum.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <bit>
#include <cmath>
#include <cstdint>
#include <random>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/numerics/double_double.hpp>
#include <sgpp/combigrid/tools/numerics/compensated_sum_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace {

using sgpp::combigrid::CTCoeffType;
using sgpp::combigrid::misc::DoubleDouble;
using sgpp::combigrid::tools::numerics::compensatedDotProduct;
using sgpp::combigrid::tools::numerics::compensatedDotProducts;
using sgpp::combigrid::tools::numerics::CompensatedSum;
using sgpp::combigrid::tools::numerics::compensatedSum;

uint64_t bitsOf(const double x) { return std::bit_cast<uint64_t>(x); }

/// Ill-conditioned combination: large integer coefficients of alternating sign that cancel in
/// pairs, times values close to each other (as the estimates of the tensor grids of a sparse
/// grid): condition number sum |c v| / |sum c v| of about 1e6 or more.
void makeCombination(const size_t n, const unsigned seed, std::vector<CTCoeffType>& coefficients,
                     std::vector<double>& values) {
  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<CTCoeffType> magnitude(1, 5000);
  std::uniform_real_distribution<double> noise(-1e-6, 1e-6);
  coefficients.resize(n);
  values.resize(n);
  CTCoeffType pairMagnitude = 0;
  for (size_t i = 0; i < n; i++) {
    if (i % 2 == 0) pairMagnitude = magnitude(rng);
    coefficients[i] = (i % 2 == 0 ? 1 : -1) * pairMagnitude;
    values[i] = 1.7 + noise(rng);
  }
}

/// Double-double reference (TwoSum and FMA products; about twice the double precision).
double referenceSum(const std::vector<CTCoeffType>& coefficients,
                    const std::vector<double>& values) {
  DoubleDouble sum{0.0, 0.0};
  for (size_t i = 0; i < values.size(); i++) {
    sum += DoubleDouble{values[i], 0.0} * static_cast<double>(coefficients[i]);
  }
  return sum.value();
}

}  // namespace

BOOST_AUTO_TEST_SUITE(compensated_sum)

BOOST_AUTO_TEST_CASE(Empty_And_Single_Term) {
  BOOST_CHECK_EQUAL((compensatedDotProduct<double, CTCoeffType, double>(
                        std::span<const CTCoeffType>(), std::span<const double>())),
                    0.0);
  BOOST_CHECK_EQUAL((compensatedSum<double, double>(std::vector<double>())), 0.0);
  BOOST_CHECK_EQUAL(CompensatedSum<double>().value(), 0.0);
  const std::vector<CTCoeffType> c{3};
  const std::vector<double> v{0.1};
  BOOST_CHECK_EQUAL(bitsOf(compensatedDotProduct<double, CTCoeffType, double>(c, v)),
                    bitsOf(3.0 * 0.1));
}

BOOST_AUTO_TEST_CASE(Cancellation_Is_Exact) {
  // A plain loop gives 0: 1e100 + 1 rounds to 1e100
  const std::vector<CTCoeffType> c{1, 1, -1};
  const std::vector<double> v{1e100, 1.0, 1e100};
  BOOST_CHECK_EQUAL((compensatedDotProduct<double, CTCoeffType, double>(c, v)), 1.0);
}

BOOST_AUTO_TEST_CASE(Product_Errors_Are_Kept) {
  // 3 * 0.1 - fl(3 * 0.1) is the rounding error of the product, lost by a plain loop
  const double rounded = 3.0 * 0.1;
  const std::vector<CTCoeffType> c{3, -1};
  const std::vector<double> v{0.1, rounded};
  const double expected = std::fma(3.0, 0.1, -rounded);
  BOOST_REQUIRE_NE(expected, 0.0);
  BOOST_CHECK_EQUAL(bitsOf(compensatedDotProduct<double, CTCoeffType, double>(c, v)),
                    bitsOf(expected));
}

BOOST_AUTO_TEST_CASE(Ill_Conditioned_Combinations_Are_Accurate) {
  for (const size_t n : {10u, 1000u, 50000u}) {
    for (unsigned seed = 0; seed < 5; seed++) {
      std::vector<CTCoeffType> c;
      std::vector<double> v;
      makeCombination(n, seed, c, v);
      const double reference = referenceSum(c, v);
      const double result = compensatedDotProduct<double, CTCoeffType, double>(c, v);
      double plain = 0.0;
      for (size_t i = 0; i < n; i++) plain += c[i] * v[i];
      BOOST_TEST_CONTEXT("n " << n << " seed " << seed << ": " << result << " vs. " << reference
                              << " (plain loop " << plain << ")") {
        // Within two rounding units of the double-double reference
        BOOST_CHECK_LE(std::abs(result - reference), 4.5e-16 * std::abs(reference));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Multiple_Outputs_Equal_Single_Outputs_Bitwise) {
  constexpr size_t N_OUTPUTS = 3;
  for (const size_t n : {0u, 1u, 7u, 5000u}) {
    std::vector<CTCoeffType> c;
    std::vector<double> v0, v1, v2;
    makeCombination(n, 11, c, v0);
    makeCombination(n, 12, c, v1);
    makeCombination(n, 13, c, v2);
    for (double& x : v2) x *= -1e5;
    std::vector<double> interleaved;
    for (size_t i = 0; i < n; i++) interleaved.insert(interleaved.end(), {v0[i], v1[i], v2[i]});

    std::vector<double> result(N_OUTPUTS, -1.0);
    compensatedDotProducts<double, CTCoeffType, double>(c, interleaved, result);
    BOOST_TEST_CONTEXT("n " << n) {
      BOOST_CHECK_EQUAL(bitsOf(result[0]),
                        bitsOf(compensatedDotProduct<double, CTCoeffType, double>(c, v0)));
      BOOST_CHECK_EQUAL(bitsOf(result[1]),
                        bitsOf(compensatedDotProduct<double, CTCoeffType, double>(c, v1)));
      BOOST_CHECK_EQUAL(bitsOf(result[2]),
                        bitsOf(compensatedDotProduct<double, CTCoeffType, double>(c, v2)));
    }
  }
}

BOOST_AUTO_TEST_CASE(Other_Element_And_Range_Types) {
  // The result depends only on the values converted to double: equal bitwise for int / double /
  // float factors (exactly representable) and span / vector / array / DataVector ranges
  std::vector<CTCoeffType> c;
  std::vector<double> v;
  makeCombination(1000, 7, c, v);
  const double expected = compensatedDotProduct<double, CTCoeffType, double>(c, v);

  const std::vector<double> cDouble(c.begin(), c.end());
  const std::vector<float> cFloat(c.begin(), c.end());
  const std::vector<int64_t> cInt64(c.begin(), c.end());
  sgpp::base::DataVector vData(v.size());
  for (size_t i = 0; i < v.size(); i++) vData[i] = v[i];

  BOOST_CHECK_EQUAL(bitsOf(compensatedDotProduct<double, double, double>(cDouble, v)),
                    bitsOf(expected));
  BOOST_CHECK_EQUAL(bitsOf(compensatedDotProduct<double, float, double>(cFloat, v)),
                    bitsOf(expected));
  BOOST_CHECK_EQUAL(bitsOf(compensatedDotProduct<double, int64_t, double>(cInt64, vData)),
                    bitsOf(expected));
  BOOST_CHECK_EQUAL(bitsOf(compensatedDotProduct<double, CTCoeffType, double>(
                        std::span<const CTCoeffType>(c), std::span<const double>(v))),
                    bitsOf(expected));
  // Commutative in the factors (the product and its error are symmetric)
  BOOST_CHECK_EQUAL(bitsOf(compensatedDotProduct<double, double, CTCoeffType>(v, c)),
                    bitsOf(expected));
}

BOOST_AUTO_TEST_CASE(Sum_And_Accumulator_Agree_With_The_Dot_Product) {
  std::vector<CTCoeffType> c;
  std::vector<double> v;
  makeCombination(5000, 3, c, v);

  // Terms that are exact products (31-bit integer times 2^-30 times a coefficient below 2^13):
  // the sum of the terms equals the dot product bitwise, also through the accumulator
  std::vector<double> scaled(v.size()), terms(v.size());
  for (size_t i = 0; i < v.size(); i++) {
    scaled[i] = std::ldexp(std::round(std::ldexp(v[i], 30)), -30);
    terms[i] = c[i] * scaled[i];
  }
  const double dot = compensatedDotProduct<double, CTCoeffType, double>(c, scaled);
  BOOST_CHECK_EQUAL(bitsOf(compensatedSum<double, double>(terms)), bitsOf(dot));

  CompensatedSum<double> accumulator;
  for (size_t i = 0; i < c.size(); i++) accumulator.addProduct(c[i], scaled[i]);
  BOOST_CHECK_EQUAL(bitsOf(accumulator.value()), bitsOf(dot));

  // The compensated sum of the cancelling terms is accurate (double-double reference)
  BOOST_CHECK_LE(std::abs(compensatedSum<double, double>(terms) - referenceSum(c, scaled)),
                 4.5e-16 * std::abs(referenceSum(c, scaled)));
}

BOOST_AUTO_TEST_SUITE_END()
