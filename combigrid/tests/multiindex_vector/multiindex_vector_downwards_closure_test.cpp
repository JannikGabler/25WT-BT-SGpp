// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_downwards_closure_test.cpp
 * @brief Boost.Test cases for the downwards-closure construction
 * (@c MIVec::downwardsClosure / @c tools::genMIVecDownwardsClosure).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <algorithm>
#include <cstdint>
#include <set>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <vector>

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

namespace {

using RawMI = std::vector<LvlType>;

/// Deterministic pseudo-random numbers (splitmix64) for fixed, platform-independent test sets.
class SplitMix64 {
 public:
  explicit SplitMix64(const uint64_t seed) : state(seed) {}

  uint64_t next() {
    uint64_t z = (state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  }

  size_t index(const size_t n) { return static_cast<size_t>(next() % n); }

 private:
  uint64_t state;
};

/// Colexicographic order (last dimension most significant): the order in which the bounding box
/// of the input is traversed (first dimension fastest), i.e. the output order of the closure.
bool colexLess(const RawMI& a, const RawMI& b) {
  return std::lexicographical_compare(a.rbegin(), a.rend(), b.rbegin(), b.rend());
}

/// Reference closure: union of the predecessor boxes of all input MIs in colexicographic order.
std::vector<RawMI> referenceClosure(const LvlMIVec& input) {
  std::set<RawMI, decltype(&colexLess)> closure(&colexLess);

  for (size_t miIdx = 0; miIdx < input.nMI(); miIdx++) {
    const LvlMI upper = input[miIdx];
    RawMI cur(input.nDim(), 0);

    while (true) {
      closure.insert(cur);
      size_t dim = 0;
      while (dim < cur.size() && cur[dim] == upper[dim]) {
        cur[dim] = 0;
        dim++;
      }
      if (dim == cur.size()) {
        break;
      }
      cur[dim]++;
    }
  }

  return std::vector<RawMI>(closure.begin(), closure.end());
}

/// Checks dimensionality, size and order of the closure of @p input against the reference.
void checkClosureMatchesReference(const LvlMIVec& input) {
  const std::vector<RawMI> expected = referenceClosure(input);
  const LvlMIVec closure = input.downwardsClosure();

  BOOST_REQUIRE_EQUAL(closure.nDim(), input.nDim());
  BOOST_REQUIRE_EQUAL(closure.nMI(), expected.size());
  for (size_t i = 0; i < expected.size(); i++) {
    BOOST_REQUIRE_MESSAGE(closure[i] == LvlMI(expected[i].begin(), expected[i].end()),
                          "Closure differs from the reference at position " << i);
  }
}

/// Checks the closure of @p input against an explicitly given sequence.
void checkClosureEquals(const LvlMIVec& input, const std::vector<RawMI>& expected) {
  const LvlMIVec closure = input.downwardsClosure();

  BOOST_REQUIRE_EQUAL(closure.nDim(), input.nDim());
  BOOST_REQUIRE_EQUAL(closure.nMI(), expected.size());
  for (size_t i = 0; i < expected.size(); i++) {
    BOOST_REQUIRE_MESSAGE(closure[i] == LvlMI(expected[i].begin(), expected[i].end()),
                          "Closure differs from the expected sequence at position " << i);
  }
}

/// Uniformly random MIs with entries in [0, maxVal].
LvlMIVec uniformMIs(const size_t nDim, const size_t nMI, const LvlType maxVal,
                    const uint64_t seed) {
  SplitMix64 rng(seed);
  LvlMIVec mis(nDim, nMI);
  for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
    for (size_t dim = 0; dim < nDim; dim++) {
      mis(miIdx, dim) = static_cast<LvlType>(rng.index(maxVal + 1));
    }
  }
  return mis;
}

/**
 * Anisotropic, sparse MIs as in the perf suite: 1-norm at most @p maxSum, dimension k is
 * incremented with a probability that decreases with k.
 */
LvlMIVec anisotropicMIs(const size_t nDim, const size_t nMI, const LvlType maxSum,
                        const uint64_t seed) {
  SplitMix64 rng(seed);
  LvlMIVec mis(nDim, nMI);
  for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
    const size_t sum = rng.index(maxSum + 1);
    for (size_t unit = 0; unit < sum; unit++) {
      size_t dim = 0;
      while (dim + 1 < nDim && rng.index(5) < 3) {
        dim += 1 + rng.index(2);
      }
      mis(miIdx, std::min(dim, nDim - 1))++;
    }
  }
  return mis;
}

}  // namespace

/*
 * The order of the closure is observable (it determines the order of the tensor grids of an
 * MIVecSGGenInstr): the closure lists its multi-indices in colexicographic order, i.e. in the
 * traversal order of the bounding box (first dimension fastest), without duplicates.
 */
BOOST_AUTO_TEST_SUITE(MIVec_downwardsClosureOrder)

BOOST_AUTO_TEST_CASE(ExplicitSequences) {
  checkClosureEquals(LvlMIVec{{1, 1}, {2, 0}}, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}});
  checkClosureEquals(LvlMIVec{{1, 0, 1}, {0, 2, 0}},
                     {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 2, 0}, {0, 0, 1}, {1, 0, 1}});
  checkClosureEquals(LvlMIVec{{3}, {1}}, {{0}, {1}, {2}, {3}});
  checkClosureEquals(LvlMIVec{{0, 2}}, {{0, 0}, {0, 1}, {0, 2}});
}

BOOST_AUTO_TEST_CASE(ZeroMI) {
  checkClosureEquals(LvlMIVec{{0, 0, 0}}, {{0, 0, 0}});
  checkClosureEquals(LvlMIVec{{0}}, {{0}});
}

BOOST_AUTO_TEST_CASE(EmptyInputKeepsDimensionality) {
  const LvlMIVec closure = LvlMIVec(4, 0).downwardsClosure();
  BOOST_TEST(closure.nDim() == 4);
  BOOST_TEST(closure.nMI() == 0);

  // Zero-dimensional multi-indices yield an empty closure.
  const LvlMIVec zeroDimClosure = LvlMIVec(0, 3).downwardsClosure();
  BOOST_TEST(zeroDimClosure.nDim() == 0);
  BOOST_TEST(zeroDimClosure.nMI() == 0);
}

BOOST_AUTO_TEST_CASE(SingleMI) {
  checkClosureMatchesReference(LvlMIVec{{2, 1, 3}});
  checkClosureMatchesReference(LvlMIVec{{0, 4, 0, 1}});
  checkClosureMatchesReference(LvlMIVec{{7}});
}

BOOST_AUTO_TEST_CASE(Duplicates) {
  checkClosureMatchesReference(LvlMIVec{{1, 2}, {1, 2}, {2, 0}, {2, 0}, {0, 0}});
  checkClosureMatchesReference(LvlMIVec{{1, 0, 2}, {1, 0, 2}, {1, 0, 2}});
}

BOOST_AUTO_TEST_CASE(AlreadyClosedInputIsReordered) {
  // The output order does not depend on the input order.
  checkClosureEquals(LvlMIVec{{1, 1}, {0, 1}, {1, 0}, {0, 0}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}});

  for (size_t nDim = 1; nDim <= 4; nDim++) {
    const LvlMIVec complete = CompleteSGGenInstr(5, nDim).genFullMIVec();
    BOOST_TEST_CONTEXT("Complete set, nDim = " << nDim) {
      checkClosureMatchesReference(complete);
      BOOST_TEST(complete.downwardsClosure().nMI() == complete.nMI());
    }
  }
}

BOOST_AUTO_TEST_CASE(RandomUniformSets) {
  const size_t nDims[] = {1, 2, 3, 4};
  const size_t nMIs[] = {1, 5, 50};
  uint64_t seed = 100;
  for (const size_t nDim : nDims) {
    for (const size_t nMI : nMIs) {
      BOOST_TEST_CONTEXT("nDim = " << nDim << ", nMI = " << nMI << ", seed = " << seed) {
        checkClosureMatchesReference(uniformMIs(nDim, nMI, 4, seed++));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(RandomSparseHighDimSets) {
  // {nDim, maxSum, nMI}: sparse anisotropic sets like the mivec cases of the perf suite.
  const size_t configs[][3] = {{5, 12, 200}, {10, 9, 150}, {15, 7, 60}, {20, 6, 60}};
  for (const auto& config : configs) {
    const uint64_t seed = 2000 + config[0];
    BOOST_TEST_CONTEXT("nDim = " << config[0] << ", maxSum = " << config[1]
                                 << ", nMI = " << config[2]) {
      checkClosureMatchesReference(
          anisotropicMIs(config[0], config[2], static_cast<LvlType>(config[1]), seed));
    }
  }
}

BOOST_AUTO_TEST_CASE(RandomTimeSeededSet) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(12);
    const size_t nMI = 1 + randGen.getUniformIndexRN(100);
    const LvlType maxSum = static_cast<LvlType>(randGen.getUniformIndexRN(9));
    const uint64_t seed = randGen.getUniformIndexRN(1000000);
    BOOST_TEST_CONTEXT("nDim = " << nDim << ", nMI = " << nMI << ", maxSum = " << maxSum) {
      checkClosureMatchesReference(anisotropicMIs(nDim, nMI, maxSum, seed));
    }
  }
}

BOOST_AUTO_TEST_CASE(IndependentOfThreadCount) {
  const LvlMIVec input = anisotropicMIs(8, 120, 9, 7);
  const int maxThreads = omp_get_max_threads();

  omp_set_num_threads(1);
  const LvlMIVec serial = input.downwardsClosure();
  omp_set_num_threads(3);
  const LvlMIVec three = input.downwardsClosure();
  omp_set_num_threads(maxThreads);
  const LvlMIVec all = input.downwardsClosure();

  BOOST_REQUIRE_EQUAL(serial.nMI(), three.nMI());
  BOOST_REQUIRE_EQUAL(serial.nMI(), all.nMI());
  for (size_t i = 0; i < serial.nMI(); i++) {
    BOOST_REQUIRE(serial[i] == three[i]);
    BOOST_REQUIRE(serial[i] == all[i]);
  }
}

BOOST_AUTO_TEST_SUITE_END()
