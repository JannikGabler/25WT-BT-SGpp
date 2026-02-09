// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cmath>
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <random>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/multiindices/paretoMaxima.hpp>
#include <sgpp/globaldef.hpp>
#include <sgpp_base.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(OP_paretoMaximum)

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
        } else if (sgpp::combigrid::miDominatesMI(input, miIdx2, miIdx1)) {
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

  const std::vector<size_t> result = miVec.paretoMaximum();

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result.begin(),
                                result.end());
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

  const std::vector<size_t> result = miVec.paretoMaximum();

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result.begin(),
                                result.end());
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
      miVec(miIdx, dimIdx) = static_cast<size_t>(dis(gen));
    }
  }

  // Generating the pareto maxima
  const std::vector<size_t> expectedResult = naivParetoMaximum(miVec);

  const std::vector<size_t> result = miVec.paretoMaximum();

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result.begin(),
                                result.end());
}

BOOST_AUTO_TEST_CASE(RandomTestConcurrent) {
  constexpr size_t N_MI = sgpp::combigrid::ParetoMaxima::MIN_MIVEC_LENGTH_FOR_CONCURRENCY;

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

  const std::vector<size_t> result = miVec.paretoMaximum();

  BOOST_CHECK_EQUAL_COLLECTIONS(expectedResult.begin(), expectedResult.end(), result.begin(),
                                result.end());
}

BOOST_AUTO_TEST_SUITE_END()
