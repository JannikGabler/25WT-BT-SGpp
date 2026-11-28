// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file node_family_singletons_test.cpp
 * @brief Boost.Test cases for the node-family singletons and the interpolation methods they
 * return (one shared generic barycentric formula, the optimized one for Clenshaw-Curtis).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/second_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/type_defs.hpp>

#include <vector>

using namespace sgpp::combigrid;

BOOST_AUTO_TEST_SUITE(NodeFamily_singletons)

BOOST_AUTO_TEST_CASE(getters_return_stable_singletons) {
  BOOST_CHECK_EQUAL(getEquidistantNodes(), getEquidistantNodes());
  BOOST_CHECK_EQUAL(getFirstTypeChebyshevNodes(), getFirstTypeChebyshevNodes());
  // Clenshaw-Curtis nodes are the Chebyshev nodes of the second kind: one singleton
  BOOST_CHECK_EQUAL(getClenshawCurtisNodes(), getSecondTypeChebyshevNodes());
  BOOST_CHECK_NE(getEquidistantNodes(), getFirstTypeChebyshevNodes());
  BOOST_CHECK_NE(getEquidistantNodes(), getClenshawCurtisNodes());
  BOOST_CHECK_NE(getFirstTypeChebyshevNodes(), getClenshawCurtisNodes());
  BOOST_CHECK(!(*getEquidistantNodes() == *getClenshawCurtisNodes()));
  BOOST_CHECK(*getClenshawCurtisNodes() == *getSecondTypeChebyshevNodes());
}

BOOST_AUTO_TEST_CASE(interpolation_method_per_family_and_node_count) {
  const InterpolationMethod* const generic = getBarycentricFormula();
  BOOST_REQUIRE(generic != nullptr);
  BOOST_CHECK_EQUAL(getBarycentricFormula(), generic);
  BOOST_CHECK_NE(getOptBarycentricFormula(), generic);

  for (const GPCntType nNodes : {1u, 2u, 3u, 7u, 65u}) {
    BOOST_TEST_CONTEXT("n" << nNodes) {
      BOOST_CHECK_EQUAL(getEquidistantNodes()->getInterpolationMethod(nNodes), generic);
      BOOST_CHECK_EQUAL(getFirstTypeChebyshevNodes()->getInterpolationMethod(nNodes), generic);
      BOOST_CHECK_EQUAL(getClenshawCurtisNodes()->getInterpolationMethod(nNodes),
                        getOptBarycentricFormula());
    }
  }
}

BOOST_AUTO_TEST_CASE(concurrent_lookup_returns_the_same_instance) {
  const int nThreads = 8;
  std::vector<const InterpolationMethod*> seen(nThreads, nullptr);

#pragma omp parallel for num_threads(nThreads)
  for (int t = 0; t < nThreads; t++) {
    seen[t] = getFirstTypeChebyshevNodes()->getInterpolationMethod(5);
  }

  for (const InterpolationMethod* const method : seen) {
    BOOST_CHECK_EQUAL(method, getBarycentricFormula());
  }
}

BOOST_AUTO_TEST_SUITE_END()
