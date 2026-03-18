// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cstddef>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/tools/multiindex_domination.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

BOOST_AUTO_TEST_SUITE(op_componentWiseMax)

BOOST_AUTO_TEST_CASE(Basic2D) {
  // Multiindizes:
  // (1,2)
  // (3,1)
  // (2,5)
  const std::vector<LvlMI> mis = {LvlMI{1, 2}, LvlMI{3, 1}, LvlMI{2, 5}};
  const LvlMIVec vec(mis);

  const auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);                  // shared_ptr darf nicht null sein
  BOOST_CHECK_EQUAL(result->nDim(), 2u);  // Dimension korrekt

  BOOST_CHECK_EQUAL((*result)[0], 3u);  // max(1,3,2)
  BOOST_CHECK_EQUAL((*result)[1], 5u);  // max(2,1,5)
}

BOOST_AUTO_TEST_CASE(Basic3D) {
  std::vector<LvlMI> mis = {LvlMI{0, 7, 1}, LvlMI{5, 2, 9}, LvlMI{3, 4, 6}};

  LvlMIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL(result->nDim(), 3u);

  BOOST_CHECK_EQUAL((*result)[0], 5u);
  BOOST_CHECK_EQUAL((*result)[1], 7u);
  BOOST_CHECK_EQUAL((*result)[2], 9u);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_SingleMI) {
  std::vector<LvlMI> mis = {LvlMI{4, 8, 15}};

  LvlMIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 4u);
  BOOST_CHECK_EQUAL((*result)[1], 8u);
  BOOST_CHECK_EQUAL((*result)[2], 15u);
}

BOOST_AUTO_TEST_CASE(AllEqual) {
  std::vector<LvlMI> mis = {LvlMI{2, 2}, LvlMI{2, 2}, LvlMI{2, 2}};

  LvlMIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 2u);
  BOOST_CHECK_EQUAL((*result)[1], 2u);
}

BOOST_AUTO_TEST_CASE(DistributedMaxima) {
  const std::vector<LvlMI> mis = {LvlMI{10, 1, 1}, LvlMI{1, 10, 1}, LvlMI{1, 1, 10}};

  const LvlMIVec vec(mis);
  const auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 10u);
  BOOST_CHECK_EQUAL((*result)[1], 10u);
  BOOST_CHECK_EQUAL((*result)[2], 10u);
}

BOOST_AUTO_TEST_CASE(OrderIndependence) {
  const std::vector<LvlMI> mis1 = {LvlMI{1, 3}, LvlMI{4, 2}};
  const std::vector<LvlMI> mis2 = {LvlMI{4, 2}, LvlMI{1, 3}};

  const LvlMIVec vec1(mis1);
  const LvlMIVec vec2(mis2);

  const auto r1 = vec1.componentWiseMax();
  const auto r2 = vec2.componentWiseMax();

  BOOST_REQUIRE(r1);
  BOOST_REQUIRE(r2);

  BOOST_CHECK_EQUAL((*r1)[0], (*r2)[0]);
  BOOST_CHECK_EQUAL((*r1)[1], (*r2)[1]);
}

BOOST_AUTO_TEST_CASE(RepeatedCallsConsistent) {
  const std::vector<LvlMI> mis = {LvlMI{2, 5}, LvlMI{7, 1}};

  const LvlMIVec vec(mis);

  const auto r1 = vec.componentWiseMax();
  const auto r2 = vec.componentWiseMax();

  BOOST_REQUIRE(r1);
  BOOST_REQUIRE(r2);

  BOOST_CHECK_EQUAL((*r1)[0], (*r2)[0]);
  BOOST_CHECK_EQUAL((*r1)[1], (*r2)[1]);
}

BOOST_AUTO_TEST_CASE(LargeValues) {
  std::vector<LvlMI> mis = {LvlMI{1000000, 1}, LvlMI{2, 999999}};

  const LvlMIVec vec(mis);
  const auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 1000000u);
  BOOST_CHECK_EQUAL((*result)[1], 999999u);
}

BOOST_AUTO_TEST_CASE(ConcurrencyRandom) {
  const size_t nDim = 5;
  const size_t nMI = constants::mi_vec::CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    std::vector<LvlMI> mis;
    mis.reserve(nMI);

    for (size_t i = 0; i < nMI; ++i) {
      LvlMI mi(nDim);
      for (size_t d = 0; d < nDim; ++d) {
        mi[d] = (LvlType)randGen.getUniformIndexRN(1000);
      }
      mis.push_back(mi);
    }

    const LvlMIVec vec(mis);

    const auto result = vec.componentWiseMax();
    BOOST_REQUIRE(result);

    // Prüfen, dass jede Komponente >= allen Einträgen ist
    bool failure = false;
    for (size_t d = 0; d < nDim; ++d) {
      LvlType maxVal = (*result)[d];
      for (size_t i = 0; i < nMI; ++i) {
        if (!(maxVal >= vec(i, d))) {
          failure = true;
          break;
        }
      }
      if (failure) break;
    }

    // Wenn der Test fehlschlägt, den Seed ausgeben
    BOOST_CHECK(!failure);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(op_paretoMaximum)

namespace {

std::vector<size_t> naivParetoMaximum(const LvlMIVec& input) {
  std::vector<size_t> skyline;

  for (size_t miIdx1 = 0; miIdx1 < input.nMI(); miIdx1++) {
    bool dominated = false;

    for (size_t miIdx2 = 0; miIdx2 < input.nMI(); miIdx2++) {
      if (miIdx1 != miIdx2) {
        if (input[miIdx1] == input[miIdx2]) {
          if (miIdx2 < miIdx1) {
            dominated = true;
            break;
          }
        } else if (tools::miDominatesMI(input, miIdx2, miIdx1)) {
          dominated = true;
          break;
        }
      }
    }

    if (!dominated) {
      skyline.push_back(miIdx1);
    }
  }

  return skyline;
}

}  // namespace

BOOST_AUTO_TEST_CASE(SmallSequential) {
  constexpr size_t N_MI = 8;
  constexpr size_t N_DIM = 2;

  LvlMIVec miVec(N_DIM, N_MI);

  miVec(0, 0) = 1;
  miVec(0, 1) = 6;

  miVec(1, 0) = 2;
  miVec(1, 1) = 5;

  miVec(2, 0) = 3;
  miVec(2, 1) = 5;

  miVec(3, 0) = 4;
  miVec(3, 1) = 5;

  miVec(4, 0) = 4;
  miVec(4, 1) = 4;

  miVec(5, 0) = 5;
  miVec(5, 1) = 3;

  miVec(6, 0) = 5;
  miVec(6, 1) = 2;

  miVec(7, 0) = 6;
  miVec(7, 1) = 1;

  // Generating the pareto maxima
  const std::vector<size_t> expectedResult = naivParetoMaximum(miVec);

  const std::shared_ptr<std::vector<size_t>> resultNonDWC = miVec.paretoMaxima(false);
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), resultNonDWC->begin(),
                                resultNonDWC->end());

  const std::shared_ptr<std::vector<size_t>> resultDWC = miVec.paretoMaxima(true);
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), resultDWC->begin(),
                                resultDWC->end());
}

BOOST_AUTO_TEST_CASE(DuplicateSequential) {
  constexpr size_t N_MI = 8;
  constexpr size_t N_DIM = 2;

  LvlMIVec miVec(N_DIM, N_MI);

  miVec(0, 0) = 1;
  miVec(0, 1) = 6;

  miVec(1, 0) = 2;
  miVec(1, 1) = 5;

  miVec(2, 0) = 4;
  miVec(2, 1) = 4;

  miVec(3, 0) = 4;
  miVec(3, 1) = 5;

  miVec(4, 0) = 4;
  miVec(4, 1) = 4;

  miVec(5, 0) = 5;
  miVec(5, 1) = 3;

  miVec(6, 0) = 5;
  miVec(6, 1) = 2;

  miVec(7, 0) = 6;
  miVec(7, 1) = 1;

  // Generating the pareto maxima
  const std::vector<size_t> expectedResult = naivParetoMaximum(miVec);

  const std::shared_ptr<std::vector<size_t>> result = miVec.paretoMaxima();

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result->begin(),
                                result->end());
}

BOOST_AUTO_TEST_CASE(RandomSequential) {
  constexpr size_t N_MI = 20;
  constexpr size_t N_DIM = 4;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    // Generating a random MIVec
    LvlMIVec miVec(N_DIM, N_MI);

    for (size_t miIdx = 0; miIdx < N_MI; miIdx++) {
      for (size_t dimIdx = 0; dimIdx < N_DIM; dimIdx++) {
        miVec(miIdx, dimIdx) = (LvlType)randGen.getUniformIndexRN(100);
      }
    }

    // Generating the pareto maxima
    const std::vector<size_t> expectedResult = naivParetoMaximum(miVec);

    const std::shared_ptr<std::vector<size_t>> result = miVec.paretoMaxima();

    BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result->begin(),
                                  result->end());
  }
}

BOOST_AUTO_TEST_CASE(RandomTestConcurrent) {
  constexpr size_t N_MI = constants::mi_vec::PM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY;
  constexpr size_t N_DIM = 2;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    // Generating a random MIVec
    LvlMIVec miVec(N_DIM, N_MI);

    for (size_t miIdx = 0; miIdx < N_MI; miIdx++) {
      for (size_t dimIdx = 0; dimIdx < N_DIM; dimIdx++) {
        miVec(miIdx, dimIdx) = (LvlType)randGen.getUniformIndexRN(1000);
      }
    }

    // Generating the pareto maxima
    const std::vector<size_t> expectedResult = naivParetoMaximum(miVec);

    const std::shared_ptr<std::vector<size_t>> result = miVec.paretoMaxima();

    BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result->begin(),
                                  result->end());
  }
}

BOOST_AUTO_TEST_SUITE_END()
