// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file combitech_coefficients_test.cpp
 * @brief Boost.Test cases for the combination-technique coefficient
 * routines (@c tools::computeCTCoeffs and friends).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

BOOST_AUTO_TEST_SUITE(Tools_computeCTCoeffs)

BOOST_AUTO_TEST_CASE(Simple1DExample) {
  const LvlMIVec miVec{{0}, {1}, {2}};

  BOOST_CHECK(miVec.isDownwardsClosed());

  const std::vector<int> result1 = tools::computeCTCoeffsNaive(miVec);
  const std::vector<int> result2 = tools::computeCTCoeffs(miVec);

  const std::vector<int> expected{0, 0, 1};

  BOOST_CHECK_EQUAL_COLLECTIONS(result1.begin(), result1.end(), expected.begin(), expected.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(result2.begin(), result2.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(Simple2DExample) {
  const LvlMIVec miVec{{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0},
                       {1, 1}, {1, 2}, {2, 0}, {3, 0}, {4, 0}};

  BOOST_CHECK(miVec.isDownwardsClosed());

  const std::vector<int> result1 = tools::computeCTCoeffsNaive(miVec);
  const std::vector<int> result2 = tools::computeCTCoeffs(miVec);

  const std::vector<int> expected{0, 0, -1, 1, -1, 0, 1, 0, 0, 1};

  BOOST_CHECK_EQUAL_COLLECTIONS(result1.begin(), result1.end(), expected.begin(), expected.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(result2.begin(), result2.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(Simple3DExample) {
  const LvlMIVec miVec{{0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 1, 0},
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
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl =
        constants::ct_coefficients::MIN_MIS_FOR_CONCURRENCY + (LvlType)randGen.getUniformIndexRN(5);
    const size_t nDim = 2 + randGen.getUniformIndexRN(1);

    const LvlMIVec miVec = tools::genMIVecForFullSG(maxLvl, nDim);

    const std::vector<int> result1 = tools::computeCTCoeffsNaive(miVec);
    const std::vector<int> result2 = tools::computeCTCoeffs(miVec);

    BOOST_CHECK_EQUAL_COLLECTIONS(result1.begin(), result1.end(), result2.begin(), result2.end());
  }
}

BOOST_AUTO_TEST_SUITE_END()
