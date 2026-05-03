// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file test_base.cpp
 * @brief Boost.Test entry point that defines the @c SGppCombigridModule
 * test executable.
 *
 * Every test translation unit in @c combigrid/tests is linked into the
 * same test driver; the driver itself lives here. Includes a workaround
 * for a clang/Boost.Test compatibility issue.
 */

// #define BOOST_TEST_DYN_LINK
// #include <boost/test/tools/old/interface.hpp>
// #define BOOST_TEST_MODULE SGppCombigridModule

// #include <boost/test/included/unit_test.hpp>

// #include <sgpp/globaldef.hpp>
// #include <sgpp_base.hpp>

// // fix for clang (from https://stackoverflow.com/a/33755176)
// #ifdef __clang__
// #include <string>

// namespace boost {
// namespace unit_test {
// namespace ut_detail {

// std::string normalize_test_case_name(const_string name) {
//   return ((name[0] == '&') ? std::string(name.begin() + 1, name.size() - 1)
//                            : std::string(name.begin(), name.size()));
// }

// }  // namespace ut_detail
// }  // namespace unit_test
// }  // namespace boost
// #endif

// BOOST_AUTO_TEST_SUITE(dummySuite)

// BOOST_AUTO_TEST_CASE(dummyTestCase) { BOOST_CHECK(true); }

// BOOST_AUTO_TEST_SUITE_END()

// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SGppCombigridModule
#include <boost/test/unit_test.hpp>

// fix for clang (from https://stackoverflow.com/a/33755176)
#ifdef __clang__
#include <string>

namespace boost {
namespace unit_test {
namespace ut_detail {

std::string normalize_test_case_name(const_string name) {
  return ((name[0] == '&') ? std::string(name.begin() + 1, name.size() - 1)
                           : std::string(name.begin(), name.size()));
}

}  // namespace ut_detail
}  // namespace unit_test
}  // namespace boost
#endif
