// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file fejer_first_quadrature_rule_test.cpp
 * @brief Boost.Test cases for the quadrature rule of the first-kind Chebyshev nodes (Fejér's
 * first rule without boundary nodes, the interpolatory rule with the boundary nodes 0 and 1).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/fejer_first_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/fejer_first_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::GPCntType;
using sgpp::combigrid::NodeGenFunc;
using sgpp::combigrid::QuadRule;
using sgpp::combigrid::quadrature_rules::FejerFirstQuadRule;

namespace {

/// Weights on [0, 1] (nodes in increasing order) by mpmath at 60 digits: the solution of the
/// moment system of the Chebyshev polynomials on the family's nodes (no / with boundary nodes).
const std::vector<double> FEJER_N3{0.22222222222222222, 0.55555555555555556, 0.22222222222222222};
const std::vector<double> FEJER_N6{0.059330510690617928, 0.18888888888888889, 0.25178060042049318,
                                   0.25178060042049318,  0.18888888888888889, 0.059330510690617928};
const std::vector<double> BOUNDARY_N3{0.16666666666666667, 0.66666666666666667,
                                      0.16666666666666667};
const std::vector<double> BOUNDARY_N4{-0.16666666666666667, 0.66666666666666667,
                                      0.66666666666666667, -0.16666666666666667};
const std::vector<double> BOUNDARY_N9{
    -0.011111111111111111, 0.05762462752982134, 0.13882402923050569,
    0.20264431376121492,   0.22403628117913832, 0.20264431376121492,
    0.13882402923050569,   0.05762462752982134, -0.011111111111111111};

const FejerFirstQuadRule WITHOUT_BOUNDARY(false);
const FejerFirstQuadRule WITH_BOUNDARY(true);

std::vector<double> weightsOf(const QuadRule& rule, const size_t nNodes) {
  std::vector<double> weights(nNodes);
  rule.genWeightsInplace(weights);
  return weights;
}

/// The nodes of the first-kind Chebyshev family on [0, 1] (increasing), without / with boundary.
DataVector nodesOf(const size_t nNodes, const bool includeBoundary) {
  return sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc()->genNodes(
      static_cast<GPCntType>(nNodes), includeBoundary);
}

/// The integral of T_k(2x - 1) over [0, 1].
double chebyshevMoment(const size_t k) {
  if (k % 2 == 1) return 0.0;
  const double kDouble = static_cast<double>(k);
  return 1.0 / (1.0 - kDouble * kDouble);
}

/// The largest error of the rule on T_0(2x - 1), ..., T_maxDegree(2x - 1) (Chebyshev recurrence,
/// whose own round-off grows with the degree: O(maxDegree * eps)).
double maxMomentError(const std::vector<double>& weights, const DataVector& nodes,
                      const size_t maxDegree) {
  std::vector<double> sums(maxDegree + 1, 0.0);
  for (size_t i = 0; i < weights.size(); i++) {
    const double t = 2.0 * nodes[i] - 1.0;
    double previous = 1.0, current = t;
    sums[0] += weights[i];
    if (maxDegree >= 1) sums[1] += weights[i] * t;
    for (size_t k = 2; k <= maxDegree; k++) {
      const double next = 2.0 * t * current - previous;
      previous = current;
      current = next;
      sums[k] += weights[i] * current;
    }
  }
  double maxError = 0.0;
  for (size_t k = 0; k <= maxDegree; k++) {
    maxError = std::max(maxError, std::abs(sums[k] - chebyshevMoment(k)));
  }
  return maxError;
}

void checkClose(const std::vector<double>& actual, const std::vector<double>& expected,
                const double tol, const std::string& what) {
  BOOST_REQUIRE_EQUAL(actual.size(), expected.size());
  for (size_t i = 0; i < actual.size(); i++) {
    BOOST_CHECK_MESSAGE(std::abs(actual[i] - expected[i]) <= tol,
                        what << " weight " << i << ": " << actual[i] << " vs. " << expected[i]);
  }
}

bool symmetricBitwise(const std::vector<double>& weights) {
  return std::equal(weights.begin(), weights.end(), weights.rbegin());
}

}  // namespace

BOOST_AUTO_TEST_SUITE(fejer_first_quadrature_rule)

BOOST_AUTO_TEST_CASE(small_rules) {
  BOOST_CHECK(weightsOf(WITHOUT_BOUNDARY, 0).empty());
  BOOST_CHECK(weightsOf(WITH_BOUNDARY, 0).empty());
  for (const FejerFirstQuadRule* rule : {&WITHOUT_BOUNDARY, &WITH_BOUNDARY}) {
    BOOST_CHECK(weightsOf(*rule, 1) == std::vector<double>{1.0});         // midpoint
    BOOST_CHECK(weightsOf(*rule, 2) == (std::vector<double>{0.5, 0.5}));  // symmetric pair
  }
  BOOST_CHECK(WITH_BOUNDARY.includesBoundary());
  BOOST_CHECK(!WITHOUT_BOUNDARY.includesBoundary());
  BOOST_CHECK_NE(WITH_BOUNDARY.id(), WITHOUT_BOUNDARY.id());
}

BOOST_AUTO_TEST_CASE(weights_match_high_precision_reference) {
  const double tol = 4e-16;
  checkClose(weightsOf(WITHOUT_BOUNDARY, 3), FEJER_N3, tol, "Fejer n3");
  checkClose(weightsOf(WITHOUT_BOUNDARY, 6), FEJER_N6, tol, "Fejer n6");
  checkClose(weightsOf(WITH_BOUNDARY, 3), BOUNDARY_N3, tol, "boundary n3");
  checkClose(weightsOf(WITH_BOUNDARY, 4), BOUNDARY_N4, tol, "boundary n4");
  checkClose(weightsOf(WITH_BOUNDARY, 9), BOUNDARY_N9, tol, "boundary n9");
}

BOOST_AUTO_TEST_CASE(exact_on_polynomials_up_to_degree_n_minus_1) {
  for (size_t n = 1; n <= 80; n++) {
    for (const bool boundary : {false, true}) {
      const std::vector<double> w = weightsOf(boundary ? WITH_BOUNDARY : WITHOUT_BOUNDARY, n);
      const double error = maxMomentError(w, nodesOf(n, boundary), n - 1);
      BOOST_CHECK_MESSAGE(error <= 1e-14, "n = " << n << (boundary ? " with" : " without")
                                                 << " boundary: moment error " << error);
    }
  }
}

BOOST_AUTO_TEST_CASE(positive_symmetric_and_normalized) {
  std::vector<size_t> sizes{1, 2, 3, 4, 5, 7, 8, 31, 64, 127, 255, 1023};
#ifdef SGPP_COMBIGRID_HAVE_FFT
  sizes.insert(sizes.end(), {8191, 32769, 131071});  // O(n^2) without an FFT backend
#endif
  for (const size_t n : sizes) {
    const std::vector<double> fejer = weightsOf(WITHOUT_BOUNDARY, n);
    const std::vector<double> boundary = weightsOf(WITH_BOUNDARY, n);
    BOOST_CHECK_MESSAGE(symmetricBitwise(fejer) && symmetricBitwise(boundary), "n = " << n);
    BOOST_CHECK_MESSAGE(*std::ranges::min_element(fejer) > 0.0, "Fejer n = " << n);
    // With boundary nodes: positive inner weights; the endpoint weights are negative from 4 on
    if (n >= 3) {
      BOOST_CHECK_MESSAGE(*std::min_element(boundary.begin() + 1, boundary.end() - 1) > 0.0,
                          "boundary n = " << n);
    }
    long double sumFejer = 0.0, sumBoundary = 0.0;  // Summation error below the tolerance
    for (size_t i = 0; i < n; i++) {
      sumFejer += fejer[i];
      sumBoundary += boundary[i];
    }
    BOOST_CHECK_SMALL(static_cast<double>(sumFejer - 1.0L), 1e-14);
    BOOST_CHECK_SMALL(static_cast<double>(sumBoundary - 1.0L), 1e-14);
  }
}

#ifdef SGPP_COMBIGRID_HAVE_FFT
BOOST_AUTO_TEST_CASE(fft_weights_match_naive_weights) {
  // Prime (8191, 131071 = 2^17 - 1 are Mersenne primes), odd, even, powers of two (+ 1)
  for (const size_t n : {3, 4, 5, 16, 17, 255, 256, 257, 1000, 2047, 8191}) {
    std::vector<double> fft(n), naive(n);
    FejerFirstQuadRule::genFejerWeightsFFTInplace(fft);
    FejerFirstQuadRule::genFejerWeightsNaiveInplace(naive);
    checkClose(fft, naive, 1e-16 * 4.0 / static_cast<double>(n) + 1e-17,
               "FFT vs. naive n" + std::to_string(n));
  }
}
#endif

BOOST_AUTO_TEST_CASE(gen_weights_matches_gen_weights_inplace) {
  for (const FejerFirstQuadRule* rule : {&WITHOUT_BOUNDARY, &WITH_BOUNDARY}) {
    for (const GPCntType n : {0u, 1u, 3u, 10u, 33u}) {
      const DataVector weights = rule->genWeights(n);
      const std::vector<double> inplace = weightsOf(*rule, n);
      BOOST_CHECK(std::equal(weights.begin(), weights.end(), inplace.begin(), inplace.end()));
    }
  }
}

BOOST_AUTO_TEST_CASE(node_families_return_their_rules) {
  const NodeGenFunc* const cheb1 = sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc();
  for (const GPCntType n : {1u, 3u, 5u, 17u}) {
    BOOST_CHECK(cheb1->getQuadRule(n, false) == sgpp::combigrid::getFejerFirstQuadRule(false));
    BOOST_CHECK(cheb1->getQuadRule(n, true) == sgpp::combigrid::getFejerFirstQuadRule(true));
    // The overload without the flag: the node set with the boundary nodes, as for all families
    BOOST_CHECK(cheb1->getQuadRule(n) == cheb1->getQuadRule(n, true));
    // The other families: one rule regardless of the boundary nodes (unchanged)
    for (const NodeGenFunc* family : {sgpp::combigrid::getEquidistantNodeGenFunc(),
                                      sgpp::combigrid::getClenshawCurtisNodeGenFunc()}) {
      BOOST_CHECK(family->getQuadRule(n, false) == family->getQuadRule(n));
      BOOST_CHECK(family->getQuadRule(n, true) == family->getQuadRule(n));
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
