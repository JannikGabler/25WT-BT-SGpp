// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <random>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients.hpp>
#include <vector>

using namespace sgpp::combigrid;

BOOST_AUTO_TEST_SUITE(Tools_computeCTCoeffs)

BOOST_AUTO_TEST_CASE(Simple1DExample) {
  const MIVec miVec{{0}, {1}, {2}};

  BOOST_CHECK(miVec.isDownwardsClosed());

  const std::vector<int> result1 = tools::computeCTCoeffsNaive(miVec);
  const std::vector<int> result2 = tools::computeCTCoeffs(miVec);

  const std::vector<int> expected{0, 0, 1};

  BOOST_CHECK_EQUAL_COLLECTIONS(result1.begin(), result1.end(), expected.begin(), expected.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(result2.begin(), result2.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(Simple2DExample) {
  const MIVec miVec{{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {3, 0}, {4, 0}};

  BOOST_CHECK(miVec.isDownwardsClosed());

  const std::vector<int> result1 = tools::computeCTCoeffsNaive(miVec);
  const std::vector<int> result2 = tools::computeCTCoeffs(miVec);

  const std::vector<int> expected{0, 0, -1, 1, -1, 0, 1, 0, 0, 1};

  BOOST_CHECK_EQUAL_COLLECTIONS(result1.begin(), result1.end(), expected.begin(), expected.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(result2.begin(), result2.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(Simple3DExample) {
  const MIVec miVec{{0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 1, 0},
                    {0, 1, 1}, {0, 1, 2}, {0, 2, 0}, {1, 0, 0}};

  BOOST_CHECK(miVec.isDownwardsClosed());

  const std::vector<int> result1 = tools::computeCTCoeffsNaive(miVec);
  const std::vector<int> result2 = tools::computeCTCoeffs(miVec);

  const std::vector<int> expected{-1, 0, 0, -1, 0, 1, 1, 1};

  BOOST_CHECK_EQUAL_COLLECTIONS(result1.begin(), result1.end(), expected.begin(), expected.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(result2.begin(), result2.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(RandomComplexExample) {}

BOOST_AUTO_TEST_CASE(RandomParallelExample) {
  std::random_device rd;
  const auto seed = rd();
  std::mt19937 gen(seed);
  std::uniform_int_distribution<size_t> dimDist(1, 5);  // number of dimensions
  std::uniform_int_distribution<MIType> maxLvlDist(
      constants::ct_coefficients::MIN_MIS_FOR_CONCURRENCY,
      constants::ct_coefficients::MIN_MIS_FOR_CONCURRENCY + 5);

  const size_t nDim = dimDist(gen);
  const MIType maxLvl = maxLvlDist(gen);

  // TODO
}

BOOST_AUTO_TEST_SUITE_END()
