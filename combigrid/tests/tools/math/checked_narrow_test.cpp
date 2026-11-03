// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file checked_narrow_test.cpp
 * @brief Boost.Test cases for @c tools::checkedNarrow, the bound of the 32-bit indices of the
 * construction of the @c misc::CTCoeffTrie.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <sgpp/combigrid/tools/math/checked_narrow.hpp>
#include <stdexcept>
#include <string>

using sgpp::combigrid::tools::checkedNarrow;

BOOST_AUTO_TEST_SUITE(checked_narrow)

BOOST_AUTO_TEST_CASE(Values_That_Fit_Are_Returned) {
  constexpr size_t max32 = std::numeric_limits<std::uint32_t>::max();

  BOOST_CHECK_EQUAL(checkedNarrow<std::uint32_t>(0, "unused"), 0u);
  BOOST_CHECK_EQUAL(checkedNarrow<std::uint32_t>(12345, "unused"), 12345u);
  BOOST_CHECK_EQUAL(checkedNarrow<std::uint32_t>(max32, "unused"), max32);
  BOOST_CHECK_EQUAL(checkedNarrow<std::uint16_t>(65535, "unused"), 65535u);
}

BOOST_AUTO_TEST_CASE(Values_Beyond_The_Maximum_Throw) {
  constexpr size_t max32 = std::numeric_limits<std::uint32_t>::max();

  BOOST_CHECK_THROW(checkedNarrow<std::uint32_t>(max32 + 1, "too large"), std::overflow_error);
  BOOST_CHECK_THROW(checkedNarrow<std::uint32_t>(std::numeric_limits<size_t>::max(), "too large"),
                    std::overflow_error);
  BOOST_CHECK_THROW(checkedNarrow<std::uint16_t>(65536, "too large"), std::overflow_error);

  try {
    checkedNarrow<std::uint32_t>(max32 + 1, "the message");
    BOOST_ERROR("no exception");
  } catch (const std::overflow_error& e) {
    BOOST_CHECK_EQUAL(std::string(e.what()), "the message");
  }
}

BOOST_AUTO_TEST_SUITE_END()
