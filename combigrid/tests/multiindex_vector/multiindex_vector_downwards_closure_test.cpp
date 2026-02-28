// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/type_defs.hpp>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

BOOST_AUTO_TEST_SUITE(MIVec_downwardsClosure)

BOOST_AUTO_TEST_CASE(EmptyMIVec) {
  const LvlMIVec vec(3, 0);  // 3 dimensions, 0 multi-indices
  const LvlMIVec closure = vec.downwardsClosure();

  BOOST_TEST(closure.nMI() == 0);
  BOOST_TEST(closure.isDownwardsClosed());
}

BOOST_AUTO_TEST_CASE(SingleMultiIndex) {
  const LvlMIVec vec{{2, 1, 3}};  // single MI
  const LvlMIVec closure = vec.downwardsClosure();

  BOOST_TEST(closure.isDownwardsClosed());

  // The closure should contain all indices <= {2,1,3} element-wise
  BOOST_TEST(closure.nMI() == (2 + 1) * (1 + 1) * (3 + 1));  // simple counting formula
}

BOOST_AUTO_TEST_CASE(AlreadyDownwardsClosed) {
  const LvlMIVec vec{{0, 0}, {1, 0}, {0, 1}, {1, 1}};  // Is downwards-closed
  const LvlMIVec closure = vec.downwardsClosure();

  BOOST_TEST(closure.isDownwardsClosed());
  BOOST_TEST(closure.nMI() == vec.nMI());  // should be unchanged
}

BOOST_AUTO_TEST_CASE(SimpleNonClosed) {
  const LvlMIVec vec{{1, 1}, {2, 0}};  // Not downwards-closed
  const LvlMIVec closure = vec.downwardsClosure();

  BOOST_TEST(closure.isDownwardsClosed());

  // Check that original MIs are included
  for (size_t i = 0; i < vec.nMI(); ++i) {
    bool found = false;
    for (size_t j = 0; j < closure.nMI(); ++j) {
      if (vec[i] == closure[j]) {
        found = true;
        break;
      }
    }
    BOOST_TEST(found);
  }
}

BOOST_AUTO_TEST_CASE(RandomizedMIVec) {
  // std::random_device rd;
  // const auto seed = rd();
  // std::mt19937 gen(seed);
  // std::uniform_int_distribution<size_t> dimDist(2, 5);   // number of dimensions
  // std::uniform_int_distribution<size_t> miDist(1, 100);  // number of MIs
  // std::uniform_int_distribution<LvlType> valDist(0, 5);

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 2 + randGen.getUniformIndexRN(4);
    const size_t nMI = 1 + randGen.getUniformIndexRN(100);

    std::vector<std::vector<LvlType>> miVec(nMI, std::vector<LvlType>(nDim));
    for (size_t i = 0; i < nMI; ++i) {
      for (size_t d = 0; d < nDim; ++d) {
        miVec[i][d] = (LvlType)randGen.getUniformIndexRN(6);
      }
    }

    const LvlMIVec vec(miVec);
    const LvlMIVec closure = vec.downwardsClosure();

    // closure should be downwards-closed
    BOOST_CHECK(closure.isDownwardsClosed());

    // original MIs must be present in closure
    for (size_t i = 0; i < vec.nMI(); ++i) {
      bool found = false;
      for (size_t j = 0; j < closure.nMI(); ++j) {
        if (vec[i] == closure[j]) {
          found = true;
          break;
        }
      }
      BOOST_CHECK(found);
    }
  }
}

BOOST_AUTO_TEST_CASE(MultiDimZeros) {
  const LvlMIVec vec{{0, 0, 0}, {0, 1, 0}, {1, 0, 1}};
  const LvlMIVec closure = vec.downwardsClosure();

  BOOST_TEST(closure.isDownwardsClosed());

  // Closure must contain the zero vector
  bool zeroFound = false;
  for (size_t i = 0; i < closure.nMI(); ++i) {
    if (closure[i] == LvlMI({0, 0, 0})) {
      zeroFound = true;
    }
  }

  BOOST_TEST(zeroFound);
}

BOOST_AUTO_TEST_SUITE_END()
