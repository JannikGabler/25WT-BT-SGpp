// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/node_generation_functions/default_functions/equidistant_node_generation_function.hpp>
#include <sgpp/combigrid/node_generation_functions/default_functions/first_type_chebyshev_node_generation_function.hpp>
#include <sgpp/combigrid/node_generation_functions/default_functions/second_type_chebyshev_node_generation_function.hpp>

using namespace sgpp::combigrid;
using namespace sgpp::base;

namespace {
constexpr double EPS = 1e-12;

void checkInteriorAndSorted(const DataVector& v, std::size_t expectedSize) {
  BOOST_CHECK_EQUAL(v.size(), expectedSize);

  for (std::size_t i = 0; i < v.size(); ++i) {
    // strikt in (0,1) mit sehr kleinem Sicherheits-EPS
    BOOST_CHECK_GT(v[i], 0.0 + EPS);
    BOOST_CHECK_LT(v[i], 1.0 - EPS);
    if (i + 1 < v.size()) {
      BOOST_CHECK_LT(v[i], v[i + 1] - EPS);  // strikt aufsteigend
    }
  }
}
}  // namespace

BOOST_AUTO_TEST_SUITE(op_genEquidistantNodes)

BOOST_AUTO_TEST_CASE(ZeroNodes) {
  const size_t n = 0;
  const node_gen_funcs::EquidistantNodeGenFunc nodeGenFunc;
  const DataVector nodes = nodeGenFunc.genGPs(n);

  BOOST_CHECK_EQUAL(nodes.size(), 0u);
}

BOOST_AUTO_TEST_CASE(BasicPropertiesSmallN) {
  const size_t n = 4;
  const node_gen_funcs::EquidistantNodeGenFunc nodeGenFunc;
  const DataVector nodes = nodeGenFunc.genGPs(n);

  checkInteriorAndSorted(nodes, n);

  for (size_t i = 0; i < n; ++i) {
    double expected = double(i + 1) / double(n + 1);
    BOOST_CHECK_CLOSE(nodes[i], expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(UniformSpacingLargerN) {
  const size_t n = 10;
  const node_gen_funcs::EquidistantNodeGenFunc nodeGenFunc;
  const DataVector nodes = nodeGenFunc.genGPs(n);

  // Adjacent differences should be equal to 1/(n+1)
  double expectedDiff = 1.0 / double(n + 1);
  for (std::size_t i = 0; i + 1 < n; ++i) {
    double diff = nodes[i + 1] - nodes[i];

    BOOST_CHECK_CLOSE(diff, expectedDiff, 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(op_genFirstTypeChebyshevNodes)

BOOST_AUTO_TEST_CASE(ZeroNodes) {
  const size_t n = 0;
  const node_gen_funcs::FirstTypeChebyshevNodeGenFunc nodeGenFunc;
  const DataVector nodes = nodeGenFunc.genGPs(n);

  BOOST_CHECK_EQUAL(nodes.size(), 0u);
}

BOOST_AUTO_TEST_CASE(BasicPropertiesAndValues) {
  const size_t n = 3;
  const node_gen_funcs::FirstTypeChebyshevNodeGenFunc nodeGenFunc;
  const DataVector nodes = nodeGenFunc.genGPs(n);

  checkInteriorAndSorted(nodes, n);

  std::vector<double> expected;
  for (std::size_t k = 0; k < n; ++k) {
    double angle = (2.0 * double(k) + 1.0) * M_PI / (2.0 * double(n));
    double x = std::cos(angle);
    expected.push_back((1.0 + x) / 2.0);
  }
  std::sort(expected.begin(), expected.end());  // Should be sorted

  for (std::size_t i = 0; i < n; ++i) {
    BOOST_CHECK_CLOSE(nodes[i], expected[i], 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(op_genSecondTypeChebyshevNodes)

BOOST_AUTO_TEST_CASE(ZeroNodes) {
  const size_t n = 0;
  const node_gen_funcs::SecondTypeChebyshevNodeGenFunc nodeGenFunc;
  const DataVector nodes = nodeGenFunc.genGPs(n);

  BOOST_CHECK_EQUAL(nodes.size(), 0u);
}

BOOST_AUTO_TEST_CASE(BasicPropertiesAndValues) {
  const size_t n = 3;
  const node_gen_funcs::SecondTypeChebyshevNodeGenFunc nodeGenFunc;
  const DataVector nodes = nodeGenFunc.genGPs(n);

  checkInteriorAndSorted(nodes, n);

  std::vector<double> expected;
  std::size_t N = n + 1;
  for (std::size_t k = 1; k <= N - 1; ++k) {
    double angle = double(k) * M_PI / double(N);
    double x = std::cos(angle);
    expected.push_back((1.0 + x) / 2.0);
  }
  std::sort(expected.begin(), expected.end());

  for (std::size_t i = 0; i < n; ++i) {
    BOOST_CHECK_CLOSE(nodes[i], expected[i], 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()