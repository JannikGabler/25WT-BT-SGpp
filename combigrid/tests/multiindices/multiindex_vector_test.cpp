// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <boost/test/unit_test_suite.hpp>
#include <cmath>
#include <memory>
#include "sgpp/combigrid/multiindices/multiindex.hpp"
#include "sgpp/combigrid/tools/multiindex_domination.hpp"
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <random>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <sgpp/globaldef.hpp>
#include <sgpp_base.hpp>
#include <vector>

using MIType = sgpp::combigrid::MIType;
using MI = sgpp::combigrid::MI;
using MIVec = sgpp::combigrid::MIVec;

BOOST_AUTO_TEST_SUITE(op_componentWiseMax)

BOOST_AUTO_TEST_CASE(ComponentWiseMax_Basic2D) {
  // Multiindizes:
  // (1,2)
  // (3,1)
  // (2,5)
  const std::vector<MI> mis = {MI{1, 2}, MI{3, 1}, MI{2, 5}};
  const MIVec vec(mis);

  const auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);                  // shared_ptr darf nicht null sein
  BOOST_CHECK_EQUAL(result->nDim(), 2u);  // Dimension korrekt

  BOOST_CHECK_EQUAL((*result)[0], 3u);  // max(1,3,2)
  BOOST_CHECK_EQUAL((*result)[1], 5u);  // max(2,1,5)
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_Basic3D) {
  std::vector<MI> mis = {MI{0, 7, 1}, MI{5, 2, 9}, MI{3, 4, 6}};

  MIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL(result->nDim(), 3u);

  BOOST_CHECK_EQUAL((*result)[0], 5u);
  BOOST_CHECK_EQUAL((*result)[1], 7u);
  BOOST_CHECK_EQUAL((*result)[2], 9u);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_SingleMI) {
  std::vector<MI> mis = {MI{4, 8, 15}};

  MIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 4u);
  BOOST_CHECK_EQUAL((*result)[1], 8u);
  BOOST_CHECK_EQUAL((*result)[2], 15u);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_AllEqual) {
  std::vector<MI> mis = {MI{2, 2}, MI{2, 2}, MI{2, 2}};

  MIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 2u);
  BOOST_CHECK_EQUAL((*result)[1], 2u);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_DistributedMaxima) {
  std::vector<MI> mis = {MI{10, 1, 1}, MI{1, 10, 1}, MI{1, 1, 10}};

  MIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 10u);
  BOOST_CHECK_EQUAL((*result)[1], 10u);
  BOOST_CHECK_EQUAL((*result)[2], 10u);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_OrderIndependence) {
  std::vector<MI> mis1 = {MI{1, 3}, MI{4, 2}};

  std::vector<MI> mis2 = {MI{4, 2}, MI{1, 3}};

  MIVec vec1(mis1);
  MIVec vec2(mis2);

  auto r1 = vec1.componentWiseMax();
  auto r2 = vec2.componentWiseMax();

  BOOST_REQUIRE(r1);
  BOOST_REQUIRE(r2);

  BOOST_CHECK_EQUAL((*r1)[0], (*r2)[0]);
  BOOST_CHECK_EQUAL((*r1)[1], (*r2)[1]);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_RepeatedCallsConsistent) {
  std::vector<MI> mis = {MI{2, 5}, MI{7, 1}};

  MIVec vec(mis);

  auto r1 = vec.componentWiseMax();
  auto r2 = vec.componentWiseMax();

  BOOST_REQUIRE(r1);
  BOOST_REQUIRE(r2);

  BOOST_CHECK_EQUAL((*r1)[0], (*r2)[0]);
  BOOST_CHECK_EQUAL((*r1)[1], (*r2)[1]);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_LargeValues) {
  std::vector<MI> mis = {MI{1000000, 1}, MI{2, 999999}};

  MIVec vec(mis);
  auto result = vec.componentWiseMax();

  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL((*result)[0], 1000000u);
  BOOST_CHECK_EQUAL((*result)[1], 999999u);
}

BOOST_AUTO_TEST_CASE(ComponentWiseMax_ConcurrencyRandom) {
  const size_t nDim = 5;
  const size_t nMI = sgpp::combigrid::mi_vec::MIN_MIVEC_LENGTH_FOR_CONCURRENCY;

  // Zufälliger Seed generieren
  std::random_device rd;
  unsigned int seed = rd();
  std::mt19937 rng(seed);
  std::uniform_int_distribution<MIType> dist(0, 1000);

  std::vector<MI> mis;
  mis.reserve(nMI);

  for (size_t i = 0; i < nMI; ++i) {
    MI mi(nDim);
    for (size_t d = 0; d < nDim; ++d) {
      mi[d] = dist(rng);
    }
    mis.push_back(mi);
  }

  MIVec vec(mis);

  auto result = vec.componentWiseMax();
  BOOST_REQUIRE(result);

  // Prüfen, dass jede Komponente >= allen Einträgen ist
  bool failure = false;
  for (size_t d = 0; d < nDim; ++d) {
    MIType maxVal = (*result)[d];
    for (size_t i = 0; i < nMI; ++i) {
      if (!(maxVal >= vec(i, d))) {
        failure = true;
        break;
      }
    }
    if (failure) break;
  }

  // Wenn der Test fehlschlägt, den Seed ausgeben
  BOOST_CHECK_MESSAGE(!failure, "componentWiseMax failed with random seed: " << seed);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(op_paretoMaximum)

namespace {

std::vector<size_t> naivParetoMaximum(const sgpp::combigrid::MIVec input) {
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
        } else if (sgpp::combigrid::tools::miDominatesMI(input, miIdx2, miIdx1)) {
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

  sgpp::combigrid::MIVec miVec(N_DIM, N_MI);

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

  sgpp::combigrid::MIVec miVec(N_DIM, N_MI);

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

  // Random generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 100.0);

  // Generating a random MIVec
  sgpp::combigrid::MIVec miVec(N_DIM, N_MI);

  for (size_t miIdx = 0; miIdx < N_MI; miIdx++) {
    for (size_t dimIdx = 0; dimIdx < N_DIM; dimIdx++) {
      miVec(miIdx, dimIdx) = static_cast<sgpp::combigrid::MIType>(dis(gen));
    }
  }

  // Generating the pareto maxima
  const std::vector<size_t> expectedResult = naivParetoMaximum(miVec);

  const std::shared_ptr<std::vector<size_t>> result = miVec.paretoMaxima();

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result->begin(),
                                result->end());
}

BOOST_AUTO_TEST_CASE(RandomTestConcurrent) {
  constexpr size_t N_MI = sgpp::combigrid::pareto_maxima::MIN_MIVEC_LENGTH_FOR_CONCURRENCY;

  constexpr size_t N_DIM = 2;

  // Random generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1000.0);

  // Generating a random MIVec
  sgpp::combigrid::MIVec miVec(N_DIM, N_MI);

  for (size_t miIdx = 0; miIdx < N_MI; miIdx++) {
    for (size_t dimIdx = 0; dimIdx < N_DIM; dimIdx++) {
      miVec(miIdx, dimIdx) = static_cast<size_t>(dis(gen));
    }
  }

  // Generating the pareto maxima
  const std::vector<size_t> expectedResult = naivParetoMaximum(miVec);

  const std::shared_ptr<std::vector<size_t>> result = miVec.paretoMaxima();

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result->begin(),
                                result->end());
}

BOOST_AUTO_TEST_SUITE_END()
