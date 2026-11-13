// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file double_double_test.cpp
 * @brief Boost.Test cases for @c misc::DoubleDouble: the exact rounding errors of the product
 * (FMA) and of the sum (TwoSum), the low part of a factor, sums that cancel, and @c value.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <cmath>
#include <sgpp/combigrid/miscellaneous/numerics/double_double.hpp>

using sgpp::combigrid::misc::DoubleDouble;

BOOST_AUTO_TEST_SUITE(double_double)

BOOST_AUTO_TEST_CASE(product_keeps_rounding_error) {
  // (1 + 2^-30)^2 = 1 + 2^-29 + 2^-60: the last term is below the precision of a double
  const double x = 1.0 + std::ldexp(1.0, -30);
  const DoubleDouble product = DoubleDouble{x, 0.0} * x;

  BOOST_CHECK_EQUAL(product.hi, 1.0 + std::ldexp(1.0, -29));
  BOOST_CHECK_EQUAL(product.lo, std::ldexp(1.0, -60));
}

BOOST_AUTO_TEST_CASE(product_scales_low_part) {
  const DoubleDouble product = DoubleDouble{1.0, std::ldexp(1.0, -60)} * 3.0;

  BOOST_CHECK_EQUAL(product.hi, 3.0);
  BOOST_CHECK_EQUAL(product.lo, 3.0 * std::ldexp(1.0, -60));
}

BOOST_AUTO_TEST_CASE(sum_keeps_rounding_error) {
  DoubleDouble sum{1.0, 0.0};
  DoubleDouble& result = (sum += DoubleDouble{std::ldexp(1.0, -60), 0.0});

  BOOST_CHECK_EQUAL(&result, &sum);
  BOOST_CHECK_EQUAL(sum.hi, 1.0);
  BOOST_CHECK_EQUAL(sum.lo, std::ldexp(1.0, -60));
  BOOST_CHECK_EQUAL(sum.value(), 1.0);
}

BOOST_AUTO_TEST_CASE(sum_adds_low_parts) {
  DoubleDouble sum{2.0, std::ldexp(1.0, -60)};
  sum += DoubleDouble{1.0, std::ldexp(1.0, -61)};

  BOOST_CHECK_EQUAL(sum.hi, 3.0);
  BOOST_CHECK_EQUAL(sum.lo, 3.0 * std::ldexp(1.0, -61));
}

BOOST_AUTO_TEST_CASE(cancelling_sum) {
  // In double, 1e16 + 1 rounds to 1e16 (spacing 2) and the sum below is 0
  const double large = 1e16;
  BOOST_REQUIRE_EQUAL((large + 1.0) - large, 0.0);

  DoubleDouble sum{0.0, 0.0};
  sum += DoubleDouble{large, 0.0};
  sum += DoubleDouble{1.0, 0.0};
  sum += DoubleDouble{-large, 0.0};

  BOOST_CHECK_EQUAL(sum.value(), 1.0);
}

BOOST_AUTO_TEST_CASE(value_of_a_double) {
  BOOST_CHECK_EQUAL((DoubleDouble{0.1, 0.0}).value(), 0.1);
  BOOST_CHECK_EQUAL((DoubleDouble{-2.5, 0.0} * 4.0).value(), -10.0);
}

BOOST_AUTO_TEST_SUITE_END()
