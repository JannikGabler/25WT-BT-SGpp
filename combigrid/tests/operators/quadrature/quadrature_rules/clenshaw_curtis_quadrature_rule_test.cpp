// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <vector>
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include "sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp"
#include "sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp"

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

static QuadRule* clenshawCurtisQuadRule = getClenshawCurtisQuadRule();

namespace {}  // namespace

BOOST_AUTO_TEST_CASE(zero_nodes) {
  const size_t nNodes = 0;
  const DataVector weights = clenshawCurtisQuadRule->getWeights(nNodes);

  BOOST_CHECK(weights.size() == 0);
}

BOOST_AUTO_TEST_CASE(one_node) {
  const size_t nNodes = 1;
  const DataVector weights = clenshawCurtisQuadRule->getWeights(nNodes);

  BOOST_REQUIRE(weights.size() == 1);
  BOOST_CHECK_CLOSE(weights[0], static_cast<double>(1.0), 1e-12);
}

BOOST_AUTO_TEST_CASE(two_nodes_trapezoid) {
  const size_t nNodes = 2;
  const DataVector weights = clenshawCurtisQuadRule->getWeights(nNodes);

  BOOST_REQUIRE(weights.size() == 2);
  // trapezoidal rule on [0,1]
  BOOST_CHECK_CLOSE(weights[0], 0.5, 1e-12);
  BOOST_CHECK_CLOSE(weights[1], 0.5, 1e-12);
}

BOOST_AUTO_TEST_CASE(three_nodes_simpson) {
  const size_t nNodes = 3;
  const DataVector weights = clenshawCurtisQuadRule->getWeights(nNodes);

  BOOST_REQUIRE(weights.size() == 3);
  // 3 point Clenshaw-Surtis equals the Simpson rule
  // weights = {1/6, 4/6, 1/6}
  BOOST_CHECK_CLOSE(weights[0], 1.0 / 6.0, 1e-12);
  BOOST_CHECK_CLOSE(weights[1], 4.0 / 6.0, 1e-12);
  BOOST_CHECK_CLOSE(weights[2], 1.0 / 6.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(weights_sum_and_symmetry_various_n) {
  const std::vector<size_t> Ns = {2u, 3u, 5u, 10u, 50u};

  for (size_t nNodes : Ns) {
    const DataVector weights = clenshawCurtisQuadRule->getWeights(nNodes);
    BOOST_REQUIRE(weights.size() == nNodes);

    // Sum der Gewichte muss 1 ergeben (Integral von f(x)=1 über [0,1]).
    double sum = 0.0;
    for (size_t i = 0; i < weights.size(); ++i) {
      sum += weights[i];
    }
    BOOST_CHECK_CLOSE(sum, 1.0, 1e-12);

    // Symmetrie: w_i == w_{N-1-i}
    for (size_t i = 0; i < weights.size(); ++i) {
      size_t j = weights.size() - 1 - i;
      BOOST_CHECK_CLOSE(weights[i], weights[j], 1e-10);
    }
  }
}

BOOST_AUTO_TEST_CASE(positive_weights_up_to_large_n) {
  const size_t maxN = 200;  // prüfe größere N auf numerische Stabilität
  for (size_t nNodes = 1; nNodes <= maxN; ++nNodes) {
    const DataVector weights = clenshawCurtisQuadRule->getWeights(nNodes);
    BOOST_REQUIRE(weights.size() == nNodes);

    // numerische Rundungsfehler können sehr kleine negative Werte erzeugen;
    // wir erlauben eine sehr kleine negative Schranke.
    const double eps = 1e-14;
    for (size_t i = 0; i < weights.size(); ++i) {
      BOOST_CHECK_GE(weights[i], -eps);
    }
  }
}

BOOST_AUTO_TEST_CASE(integrates_linear_function) {
  // Prüfe, ob die Quadratur das Integral von f(x)=x korrekt approximiert.
  // Das exakte Integral von x über [0,1] ist 1/2.
  const std::vector<size_t> Ns = {2u, 3u, 5u, 10u, 50u};

  for (size_t nNodes : Ns) {
    const DataVector weights = clenshawCurtisQuadRule->getWeights(nNodes);
    BOOST_REQUIRE(weights.size() == nNodes);

    double integralApprox = 0.0;
    if (nNodes == 1) {
      // nur ein Knoten bei 0.5 wäre ungewöhnlich — bereits durch existing tests abgedeckt
      // (falls implementiert, handle es robust)
      integralApprox = weights[0] * 0.5;
    } else {
      // Clenshaw-Curtis Knoten auf [0,1]: x_i = (1 - cos(pi * i / (N-1))) / 2
      const double denom = static_cast<double>(nNodes - 1);
      for (size_t i = 0; i < nNodes; ++i) {
        double xi = (1.0 - std::cos(M_PI * static_cast<double>(i) / denom)) / 2.0;
        integralApprox += weights[i] * xi;
      }
    }

    BOOST_CHECK_CLOSE(integralApprox, 0.5, 1e-12);
  }
}