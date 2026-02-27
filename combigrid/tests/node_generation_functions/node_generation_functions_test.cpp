// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <cmath>
#include "sgpp/globaldef.hpp"
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_functions.hpp>

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
  DataVector v = genEquidistantNodes(0);
  BOOST_CHECK_EQUAL(v.size(), 0u);
}

BOOST_AUTO_TEST_CASE(BasicPropertiesSmallN) {
  const std::size_t n = 4;
  DataVector v = genEquidistantNodes(n);
  checkInteriorAndSorted(v, n);

  // erwartete äquidistante Innenpunkte: (i+1)/(n+1)
  for (std::size_t i = 0; i < n; ++i) {
    double expected = double(i + 1) / double(n + 1);
    BOOST_CHECK_CLOSE(v[i], expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(UniformSpacingLargerN) {
  const std::size_t n = 10;
  DataVector v = genEquidistantNodes(n);
  checkInteriorAndSorted(v, n);

  // Adjacent differences should be equal to 1/(n+1)
  double expectedDiff = 1.0 / double(n + 1);
  for (std::size_t i = 0; i + 1 < n; ++i) {
    double diff = v[i + 1] - v[i];
    // relative check
    BOOST_CHECK_CLOSE(diff, expectedDiff, 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(op_genFirstTypeChebyshevNodes)

BOOST_AUTO_TEST_CASE(ZeroNodes) {
  DataVector v = genFirstTypeChebyshevNodes(0);
  BOOST_CHECK_EQUAL(v.size(), 0u);
}

BOOST_AUTO_TEST_CASE(BasicPropertiesAndValues) {
  const std::size_t n = 3;
  DataVector v = genFirstTypeChebyshevNodes(n);
  checkInteriorAndSorted(v, n);

  // Chebyshev erste Art (Wurzeln) für N = n:
  // x_k = cos((2k+1) * pi / (2N)), k=0..N-1
  // Abbildung auf [0,1]: (1 + x_k)/2
  std::vector<double> expected;
  for (std::size_t k = 0; k < n; ++k) {
    double angle = (2.0 * double(k) + 1.0) * M_PI / (2.0 * double(n));
    double x = std::cos(angle);
    expected.push_back((1.0 + x) / 2.0);
  }
  std::sort(expected.begin(), expected.end());  // Generator soll aufsteigend liefern

  // Vergleich elementweise
  for (std::size_t i = 0; i < n; ++i) {
    BOOST_CHECK_CLOSE(v[i], expected[i], 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(op_genSecondTypeChebyshevNodes)

BOOST_AUTO_TEST_CASE(ZeroNodes) {
  DataVector v = genSecondTypeChebyshevNodes(0);
  BOOST_CHECK_EQUAL(v.size(), 0u);
}

BOOST_AUTO_TEST_CASE(BasicPropertiesAndValues) {
  const std::size_t n = 3;  // Anzahl innerer Stützstellen
  DataVector v = genSecondTypeChebyshevNodes(n);
  checkInteriorAndSorted(v, n);

  // Chebyshev zweite Art (Extrema) mit N = n + 1 (weil N+1 Punkte incl. Ränder)
  // x_k = cos(k * pi / N), k = 0..N
  // innere k = 1..N-1 => genau n Punkte
  // Abbildung auf [0,1]: (1 + x_k)/2
  std::vector<double> expected;
  std::size_t N = n + 1;
  for (std::size_t k = 1; k <= N - 1; ++k) {
    double angle = double(k) * M_PI / double(N);
    double x = std::cos(angle);
    expected.push_back((1.0 + x) / 2.0);
  }
  std::sort(expected.begin(), expected.end());

  for (std::size_t i = 0; i < n; ++i) {
    BOOST_CHECK_CLOSE(v[i], expected[i], 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(ClenshawCurtisAliasMatchesSecondType) {
  const std::size_t n = 7;
  DataVector v2 = genSecondTypeChebyshevNodes(n);
  DataVector vc = genClenshawCurtisNodes(n);

  BOOST_CHECK_EQUAL(v2.size(), vc.size());
  for (std::size_t i = 0; i < v2.size(); ++i) {
    BOOST_CHECK_CLOSE(v2[i], vc[i], 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()