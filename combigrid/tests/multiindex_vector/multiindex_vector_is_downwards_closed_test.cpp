// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_is_downwards_closed_test.cpp
 * @brief Boost.Test cases for the downwards-closedness predicate
 * @c tools::isMIVecDownwardsClosed.
 */

#include <string>
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <random>
#include <set>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

// // Helper: create MIVec from a list of MI initializer lists
// LvlMIVec mivecFromList(size_t nDim, std::initializer_list<std::initializer_list<LvlType>> list) {
//   std::vector<LvlMI> vec;
//   for (const auto& l : list) {
//     LvlMI m{l};
//     // ensure correct dimension
//     if (m.size() != nDim) {
//       // If user supplied wrong dimension, pad with zeros (safe choice for tests)
//       if (m.size() < nDim)
//         m.resize(nDim, 0);
//       else
//         m.resize(nDim);
//     }
//     vec.push_back(m);
//   }
//   return LvlMIVec(vec);
// }

// Check existence of an MI in an MIVec via linear scan (uses MI::operator==)
bool containsMI(const LvlMIVec& mv, const LvlMI& needle) {
  for (size_t i = 0; i < mv.nMI(); ++i) {
    if (mv[i] == needle) return true;
  }
  return false;
}

// Compute expected "downwards closed" property by brute force:
// For every multi-index m in mv, all componentwise smaller-or-equal indices must be present.
bool expectedIsDownwardsClosed(const LvlMIVec& mv) {
  const size_t nMI = mv.nMI();
  if (nMI == 0) return true;  // vacuously true

  const size_t ndim = mv[0].nDim();
  for (size_t i = 0; i < nMI; ++i) {
    const LvlMI mi = mv[i];

    // iterate over all componentwise <= vectors (cartesian product)
    std::vector<LvlType> counter(ndim, 0);
    std::vector<LvlType> limits(ndim);
    for (size_t d = 0; d < ndim; ++d) limits[d] = mi[d];

    bool done = false;
    while (!done) {
      LvlMI candidate;
      for (size_t d = 0; d < ndim; ++d) candidate.push_back(counter[d]);
      if (!containsMI(mv, candidate)) return false;

      // increment counter
      for (size_t d = 0; d < ndim; ++d) {
        if (++counter[d] <= limits[d]) {
          break;
        } else {
          counter[d] = 0;
          if (d == ndim - 1) done = true;
        }
      }
      // Special case: if ndim == 0 we break immediately (but MI's nDim won't be 0 here).
    }
  }
  return true;
}

// Utility: compare two MIs for ordering when inserting into set<string> for uniqueness
std::string miToString(const LvlMI& m) {
  std::ostringstream os;
  os << "(";
  for (size_t i = 0; i < m.nDim(); ++i) {
    if (i) os << ",";
    os << m[i];
  }
  os << ")";
  return os.str();
}

// Compute closure size (all sub-multi-indices of each mi, unique)
std::set<std::string> computeDownwardsClosureSet(const LvlMIVec& mv) {
  std::set<std::string> s;
  if (mv.nMI() == 0) return s;
  const size_t ndim = mv[0].nDim();

  for (size_t i = 0; i < mv.nMI(); ++i) {
    const LvlMI mi = mv[i];
    // counters
    std::vector<LvlType> counter(ndim, 0);
    std::vector<LvlType> limits(ndim);
    for (size_t d = 0; d < ndim; ++d) limits[d] = mi[d];

    bool done = false;
    while (!done) {
      LvlMI candidate;
      for (size_t d = 0; d < ndim; ++d) candidate.push_back(counter[d]);
      s.insert(miToString(candidate));

      // increment
      for (size_t d = 0; d < ndim; ++d) {
        if (++counter[d] <= limits[d]) break;
        counter[d] = 0;
        if (d == ndim - 1) done = true;
      }
    }
  }
  return s;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(MIVec_isDownwardsClosed)

// Empty MIVec should be downwards-closed (vacuous truth)
BOOST_AUTO_TEST_CASE(empty_mivec_is_downwards_closed) {
  // Create empty MIVec using the (nDim, nMI) constructor
  const size_t ndim = 3;
  LvlMIVec empty(ndim, 0);

  BOOST_CHECK_EQUAL(empty.nMI(), 0u);
  BOOST_CHECK(empty.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(empty));
}

// Single zero MI (all components 0) should be downwards-closed
BOOST_AUTO_TEST_CASE(single_zero_mi_is_downwards_closed) {
  const LvlMI zero{0, 0, 0, 0};
  const LvlMIVec mv(std::vector<LvlMI>{zero});

  BOOST_CHECK_EQUAL(mv.nDim(), zero.nDim());
  BOOST_CHECK(mv.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(mv));
}

// Single non-zero MI should NOT be downwards-closed unless all lower combos included
BOOST_AUTO_TEST_CASE(single_nonzero_mi_not_downwards_closed) {
  const LvlMI mi{1, 0, 2};
  const LvlMIVec mv(std::vector<LvlMI>{mi});

  // This should be false because e.g. (0,0,0) etc. are missing.
  BOOST_CHECK(!mv.isDownwardsClosed());
  BOOST_CHECK(!expectedIsDownwardsClosed(mv));
}

// Fully closed small 2D set: all combinations up to (1,1) -> should be closed
BOOST_AUTO_TEST_CASE(complete_rectangle_is_downwards_closed) {
  // All of: (0,0), (1,0), (0,1), (1,1)
  const LvlMIVec mv({{0, 0}, {1, 0}, {0, 1}, {1, 1}});

  // Try elements in random order to ensure order doesn't matter
  LvlMIVec shuffled = mv;  // assuming copy ctor exists

  BOOST_CHECK(shuffled.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(shuffled));
}

// Missing one required lower element -> not downwards-closed
BOOST_AUTO_TEST_CASE(missing_lower_element_is_not_downwards_closed) {
  // Has (1,1) and (0,0) but missing (0,1)
  const LvlMIVec mv({{1, 1}, {0, 0}, {1, 0}});

  // (0,1) is missing so should be false
  BOOST_CHECK(!mv.isDownwardsClosed());
  BOOST_CHECK(!expectedIsDownwardsClosed(mv));
}

// Duplicate entries should not affect decision
BOOST_AUTO_TEST_CASE(duplicates_do_not_affect_downwards_closedness) {
  const LvlMIVec mv({
      {0, 0},
      {1, 0},
      {0, 1},
      {1, 1},
      {1, 1},  // duplicate
      {0, 1}   // duplicate
  });

  BOOST_CHECK(mv.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(mv));
}

// Higher dimension example: 3D set that is closed
BOOST_AUTO_TEST_CASE(three_dimensional_closed_example) {
  // include all combinations up to (1,1,1) => 8 elements
  const LvlMIVec mv(
      {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}});

  BOOST_CHECK(mv.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(mv));
}

// downwardsClosure() should produce a downwards-closed superset
BOOST_AUTO_TEST_CASE(downwards_closure_produces_closed_superset) {
  const LvlMIVec mv({{2, 1},  // requires many lower elements
                     {0, 1},
                     {1, 0}});

  BOOST_CHECK(!mv.isDownwardsClosed());  // original is not closed

  const LvlMIVec closed = mv.downwardsClosure();
  BOOST_CHECK(closed.isDownwardsClosed());

  // Original elements must be present in closure
  for (size_t i = 0; i < mv.nMI(); ++i) {
    BOOST_REQUIRE_MESSAGE(containsMI(closed, mv[i]),
                          "downwardsClosure() must contain original MI: " << miToString(mv[i]));
  }

  // The closure's unique set should equal brute-force computed set
  const std::set<std::string> expected = computeDownwardsClosureSet(mv);

  std::set<std::string> actualSet;
  for (size_t i = 0; i < closed.nMI(); ++i) {
    actualSet.insert(miToString(closed[i]));
  }

  BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), actualSet.begin(),
                                actualSet.end());
}

/*
Randomized small tests: generate random MIVec, compute expected result, compare.
Keep deterministic seed for reproducibility in tests.
*/
BOOST_AUTO_TEST_CASE(randomized_small_tests) {
  constexpr size_t nDim = 3;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t trial = 0; trial < 50; trial++) {
      const size_t nMI = randGen.getUniformIndexRN(11);  // 0..10

      std::vector<LvlMI> v;
      for (size_t i = 0; i < nMI; ++i) {
        LvlMI m;

        for (size_t d = 0; d < nDim; ++d) {
          const LvlType lvl = static_cast<LvlType>(randGen.getUniformIndexRN(3));
          m.push_back(lvl);
        }

        v.push_back(m);
      }

      const LvlMIVec mv(v);

      bool expected = expectedIsDownwardsClosed(mv);
      bool actual = mv.isDownwardsClosed();

      BOOST_CHECK_MESSAGE(actual == expected,
                          "Random test mismatch. expected=" << expected << " actual=" << actual
                                                            << " ; nMI=" << mv.nMI());
    }
  }
}

// Large-but-sparse: ensures function handles bigger indices correctly (not performance test)
BOOST_AUTO_TEST_CASE(large_values_non_closed) {
  // Single huge index (3,2,1) but missing many lower ones
  const LvlMIVec mv({{3, 2, 1}});

  BOOST_CHECK(!mv.isDownwardsClosed());

  // The downwards closure should be closed and contain the (3,2,1)
  const LvlMIVec closed = mv.downwardsClosure();

  BOOST_CHECK(closed.isDownwardsClosed());
  BOOST_CHECK(containsMI(closed, LvlMI{3, 2, 1}));
}

// Check that ordering / insertion order doesn't matter: create closed set in shuffled order
BOOST_AUTO_TEST_CASE(order_independence_for_closed_sets) {
  const LvlMIVec mv({{1, 1}, {0, 1}, {1, 0}, {0, 0}});
  const LvlMIVec mv_shuffled({{0, 1}, {1, 0}, {1, 1}, {0, 0}});

  BOOST_CHECK(mv.isDownwardsClosed());
  BOOST_CHECK(mv_shuffled.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(mv));
  BOOST_CHECK(expectedIsDownwardsClosed(mv_shuffled));
}

/*Multi-threaded randomized MIVec: generate at least DWC_MIN_MI_FOR_CONCURRENCY MIs randomly.
The used random seed is printed so a failing case can be reproduced.
*/
BOOST_AUTO_TEST_CASE(multi_threaded_random_mivec_with_seed) {
  constexpr size_t nDim = 3;
  constexpr size_t minMIForConcurrency = constants::mi_vec::DWC_MIN_MI_FOR_CONCURRENCY;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    // Generate exactly minMIForConcurrency random multi-indices
    std::vector<LvlMI> mis(minMIForConcurrency);

    for (size_t i = 0; i < minMIForConcurrency; ++i) {
      LvlMI m(nDim);

      for (size_t d = 0; d < nDim; ++d) {
        m[d] = static_cast<LvlType>(randGen.getUniformIndexRN(4));
      }

      mis[i] = m;
    }

    const LvlMIVec mv(mis);

    // BOOST_TEST_MESSAGE("Generated MIVec: nDim=" << mv.nDim() << " nMI=" << mv.nMI());

    // Brute-force expected result (may be expensive but kept feasible by small component range)
    const bool expected = expectedIsDownwardsClosed(mv);

    // Actual result (should execute multi-threaded path when nMI >= constant)
    const bool actual = mv.isDownwardsClosed();
    BOOST_CHECK_MESSAGE(actual == expected, "Mismatch in isDownwardsClosed() - expected="
                                                << expected << " actual=" << actual
                                                << " nMI=" << mv.nMI());

    // Additional consistency checks for downwardsClosure (only run if not too big)
    // (we still print seed above so it's reproducible if this becomes expensive)
    const LvlMIVec closed = mv.downwardsClosure();
    BOOST_CHECK_MESSAGE(closed.isDownwardsClosed(),
                        "downwardsClosure() result is not downwards-closed");

    // Original elements must be contained in the closure
    for (size_t i = 0; i < mv.nMI(); ++i) {
      BOOST_REQUIRE_MESSAGE(containsMI(closed, mv[i]),
                            "downwardsClosure() must contain original MI: " << miToString(mv[i]));
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()