// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file clenshaw_curtis_quadrature_rule_test.cpp
 * @brief Boost.Test cases for the Clenshaw-Curtis quadrature rule weights.
 */

#include <vector>
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <algorithm>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <sgpp/combigrid/type_defs.hpp>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

static QuadRule* clenshawCurtisQuadRule = getClenshawCurtisQuadRule();

namespace {

/// Clenshaw-Curtis nodes on [0,1] in ascending order: x_i = (1 - cos(pi * i / (N-1))) / 2
std::vector<double> ccNodes(const size_t nNodes) {
  std::vector<double> nodes(nNodes);
  const double denom = static_cast<double>(nNodes - 1);
  for (size_t i = 0; i < nNodes; ++i) {
    nodes[i] = (1.0 - std::cos(M_PI * static_cast<double>(i) / denom)) / 2.0;
  }
  return nodes;
}

/// Checks that the weights are symmetric, sum to 1 and integrate all monomials x^p with
/// p < nNodes exactly over [0,1] (exactness degree of N-point Clenshaw-Curtis is >= N-1)
void checkWeightsExactForPolynomials(const DataVector& weights, const size_t nNodes) {
  BOOST_REQUIRE_EQUAL(weights.size(), nNodes);
  const std::vector<double> nodes = ccNodes(nNodes);

  for (size_t i = 0; i < nNodes; ++i) {
    BOOST_CHECK_SMALL(weights[i] - weights[nNodes - 1 - i], 1e-14);
  }

  for (size_t p = 0; p < nNodes; ++p) {
    double integralApprox = 0.0;
    for (size_t i = 0; i < nNodes; ++i) {
      integralApprox += weights[i] * std::pow(nodes[i], static_cast<double>(p));
    }
    const double integralExact = 1.0 / static_cast<double>(p + 1);
    BOOST_CHECK_MESSAGE(std::abs(integralApprox - integralExact) <= 1e-13,
                        "nNodes = " << nNodes << ", p = " << p << ": " << integralApprox
                                    << " != " << integralExact);
  }
}

}  // namespace

BOOST_AUTO_TEST_CASE(zero_nodes) {
  const size_t nNodes = 0;
  const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);

  BOOST_CHECK(weights.size() == 0);
}

BOOST_AUTO_TEST_CASE(one_node) {
  const size_t nNodes = 1;
  const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);

  BOOST_REQUIRE(weights.size() == 1);
  BOOST_CHECK_CLOSE(weights[0], static_cast<double>(1.0), 1e-12);
}

BOOST_AUTO_TEST_CASE(two_nodes_trapezoid) {
  const size_t nNodes = 2;
  const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);

  BOOST_REQUIRE(weights.size() == 2);
  // trapezoidal rule on [0,1]
  BOOST_CHECK_CLOSE(weights[0], 0.5, 1e-12);
  BOOST_CHECK_CLOSE(weights[1], 0.5, 1e-12);
}

BOOST_AUTO_TEST_CASE(three_nodes_simpson) {
  const size_t nNodes = 3;
  const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);

  BOOST_REQUIRE(weights.size() == 3);
  // 3 point Clenshaw-Surtis equals the Simpson rule
  // weights = {1/6, 4/6, 1/6}
  BOOST_CHECK_CLOSE(weights[0], 1.0 / 6.0, 1e-12);
  BOOST_CHECK_CLOSE(weights[1], 4.0 / 6.0, 1e-12);
  BOOST_CHECK_CLOSE(weights[2], 1.0 / 6.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(weights_sum_and_symmetry_various_n) {
  const std::vector<GPCntType> Ns = {2u, 3u, 5u, 10u, 50u};

  for (GPCntType nNodes : Ns) {
    const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);
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
  const GPCntType maxN = 200;  // prüfe größere N auf numerische Stabilität
  for (GPCntType nNodes = 1; nNodes <= maxN; ++nNodes) {
    const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);
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
  const std::vector<GPCntType> Ns = {2u, 3u, 5u, 10u, 50u};

  for (GPCntType nNodes : Ns) {
    const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);
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

BOOST_AUTO_TEST_CASE(gen_weights_integrates_polynomials_exactly) {
  // Covers odd and even N - 1 (relevant for the FFT path) and the doubling growth sizes
  for (size_t nNodes = 2; nNodes <= 40; ++nNodes) {
    checkWeightsExactForPolynomials(clenshawCurtisQuadRule->genWeights(nNodes), nNodes);
  }
  for (size_t nNodes : {63u, 64u, 65u, 129u}) {
    checkWeightsExactForPolynomials(clenshawCurtisQuadRule->genWeights(nNodes), nNodes);
  }
}

BOOST_AUTO_TEST_CASE(naive_weights_integrate_polynomials_exactly) {
  const quadrature_rules::ClenshawCurtisQuadRule rule;
  for (size_t nNodes = 2; nNodes <= 40; ++nNodes) {
    checkWeightsExactForPolynomials(rule.genWeightsNaive(nNodes), nNodes);
  }
}

#ifdef SGPP_COMBIGRID_HAVE_FFT
BOOST_AUTO_TEST_CASE(fft_weights_integrate_polynomials_exactly) {
  const quadrature_rules::ClenshawCurtisQuadRule rule;
  for (size_t nNodes = 2; nNodes <= 40; ++nNodes) {
    checkWeightsExactForPolynomials(rule.genWeightsFFT(nNodes), nNodes);
  }
}

BOOST_AUTO_TEST_CASE(fft_weights_match_naive_weights) {
  const quadrature_rules::ClenshawCurtisQuadRule rule;
  for (size_t nNodes = 2; nNodes <= 300; ++nNodes) {
    const DataVector naive = rule.genWeightsNaive(nNodes);
    const DataVector fft = rule.genWeightsFFT(nNodes);
    BOOST_REQUIRE_EQUAL(fft.size(), nNodes);
    for (size_t i = 0; i < nNodes; ++i) {
      BOOST_CHECK_MESSAGE(
          std::abs(fft[i] - naive[i]) <= 1e-14,
          "nNodes = " << nNodes << ", i = " << i << ": " << fft[i] << " != " << naive[i]);
    }
  }
}

BOOST_AUTO_TEST_CASE(gen_weights_uses_fft) {
  const quadrature_rules::ClenshawCurtisQuadRule rule;
  for (size_t nNodes : {2u, 3u, 16u, 17u}) {
    const DataVector weights = rule.genWeights(nNodes);
    const DataVector fft = rule.genWeightsFFT(nNodes);
    BOOST_REQUIRE_EQUAL(weights.size(), fft.size());
    for (size_t i = 0; i < nNodes; ++i) {
      BOOST_CHECK_EQUAL(weights[i], fft[i]);
    }
  }
}
#endif

BOOST_AUTO_TEST_CASE(gen_weights_inplace_matches_gen_weights) {
  for (size_t nNodes = 0; nNodes <= 33; ++nNodes) {
    const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);
    std::vector<double> out(nNodes, -1.0);
    clenshawCurtisQuadRule->genWeightsInplace(out);
    for (size_t i = 0; i < nNodes; ++i) {
      BOOST_CHECK_EQUAL(out[i], weights[i]);
    }
  }
}

namespace {

/**
 * Clenshaw-Curtis weights on [0, 1] from the closed form (Waldvogel, 2006) in long double:
 * w_k = c_k / m * (1 - sum_{j=1}^{m/2} b_j / (4 j^2 - 1) * cos(2 pi j k / m)) on [-1, 1] with
 * m = nNodes - 1, c_0 = c_m = 1, else 2, b_{m/2} = 1 (even m), else 2; the cosine argument is
 * reduced exactly (jk mod m). O(nNodes^2).
 */
std::vector<long double> ccWeightsLongDouble(const size_t nNodes) {
  const size_t m = nNodes - 1;
  const long double pi = 3.141592653589793238462643383279502884L;
  std::vector<long double> wcc(nNodes);
  for (size_t k = 0; k <= m; k++) {
    long double s = 0.0L;
    for (size_t j = 1; j <= m / 2; j++) {
      const long double bj = (2 * j == m) ? 1.0L : 2.0L;
      const long double jl = static_cast<long double>(j);
      const long double angle = 2.0L * pi * static_cast<long double>((j * k) % m) / m;
      s += bj / (4.0L * jl * jl - 1.0L) * std::cos(angle);
    }
    const long double ck = (k == 0 || k == m) ? 1.0L : 2.0L;
    wcc[k] = ck / static_cast<long double>(m) * (1.0L - s);
  }
  std::vector<long double> w01(nNodes);
  for (size_t i = 0; i <= m; i++) {
    w01[i] = 0.5L * wcc[m - i];
  }
  return w01;
}

/**
 * Checks @p weights (nNodes = m + 1 Clenshaw-Curtis weights on [0, 1]) for symmetry, sum 1 and
 * exactness on the Chebyshev polynomials T_p(2x - 1) for the degrees in @p degrees (all
 * <= m): at the node x_i = (1 - cos(pi i / m)) / 2, T_p(2 x_i - 1) = (-1)^p cos(pi p i / m), and
 * the integral over [0, 1] is 1 / (1 - p^2) for even p, else 0. Tolerances relative to the
 * weight scale 1 / m.
 */
void checkLargeRule(const DataVector& weights, const std::vector<size_t>& degrees) {
  const size_t m = weights.size() - 1;
  const double md = static_cast<double>(m);
  const double eps = 2.220446049250313e-16;

  double sum = 0.0;
  for (size_t i = 0; i <= m; i++) {
    sum += weights[i];
    BOOST_CHECK_MESSAGE(std::abs(weights[i] - weights[m - i]) <= 4 * eps / md,
                        "m = " << m << ", i = " << i);
  }
  BOOST_CHECK_SMALL(sum - 1.0, 1e-13);

  for (const size_t p : degrees) {
    BOOST_REQUIRE(p <= m);
    double integral = 0.0;
    for (size_t i = 0; i <= m; i++) {
      const double angle = M_PI * static_cast<double>((p * i) % (2 * m)) / md;
      integral += weights[i] * ((p % 2 == 0) ? 1.0 : -1.0) * std::cos(angle);
    }
    const double pd = static_cast<double>(p);
    const double exact = (p % 2 == 0) ? 1.0 / (1.0 - pd * pd) : 0.0;
    BOOST_CHECK_MESSAGE(std::abs(integral - exact) <= 1e-13,
                        "m = " << m << ", p = " << p << ": " << integral << " != " << exact);
  }
}

}  // namespace

BOOST_AUTO_TEST_CASE(weights_match_long_double_closed_form) {
  // Small and odd / even m, the doubling sizes 2^k + 1 and their neighbors; the error bound is a
  // few units of the weight scale 1 / m (the FFT sums O(m) terms of size <= 1)
  const double eps = 2.220446049250313e-16;
  for (const GPCntType nNodes :
       {2u,  3u,  4u,  5u,   6u,   7u,   8u,   9u,   10u,  16u,   17u,   18u,   32u,  33u,
        34u, 64u, 65u, 100u, 127u, 128u, 129u, 257u, 513u, 1023u, 1024u, 1025u, 2049u}) {
    const DataVector weights = clenshawCurtisQuadRule->genWeights(nNodes);
    const std::vector<long double> reference = ccWeightsLongDouble(nNodes);
    const double scale = 1.0 / static_cast<double>(nNodes - 1);
    double maxErr = 0.0;
    for (size_t i = 0; i < nNodes; i++) {
      maxErr = std::max(maxErr, static_cast<double>(std::abs(weights[i] - reference[i])));
    }
    BOOST_CHECK_MESSAGE(maxErr <= 8 * eps * scale, "nNodes = " << nNodes << ": max error "
                                                               << maxErr / (eps * scale)
                                                               << " eps / m");
  }
}

BOOST_AUTO_TEST_CASE(large_rules_are_symmetric_and_exact_on_chebyshev_polynomials) {
  std::vector<GPCntType> sizes{4096u, 4097u, 4098u};
#ifdef SGPP_COMBIGRID_HAVE_FFT
  sizes.insert(sizes.end(), {32769u, 131073u});  // O(n^2) without an FFT backend
#endif
  for (const GPCntType nNodes : sizes) {
    const size_t m = nNodes - 1;
    std::vector<size_t> degrees = {0, 1, 2, 3, 4, 5, 10, 11, 100, 101, m / 2, m - 2, m - 1, m};
    BOOST_TEST_CONTEXT("nNodes = " << nNodes) {
      checkLargeRule(clenshawCurtisQuadRule->genWeights(nNodes), degrees);
    }
  }
}
