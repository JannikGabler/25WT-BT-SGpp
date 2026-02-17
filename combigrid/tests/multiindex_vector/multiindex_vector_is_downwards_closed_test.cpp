// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <vector>
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <random>
#include <set>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>

using namespace sgpp::combigrid;

namespace {

// Helper: create MI from initializer_list
// static MI miFromList(std::initializer_list<MIType> elems) {
//   MI m;
//   m.insert(m.end(), elems.begin(), elems.end());
//   return m;
// }

// Helper: create MIVec from a list of MI initializer lists
static MIVec mivecFromList(size_t nDim, std::initializer_list<std::initializer_list<MIType>> list) {
  std::vector<MI> vec;
  for (const auto& l : list) {
    MI m{l};
    // ensure correct dimension
    if (m.size() != nDim) {
      // If user supplied wrong dimension, pad with zeros (safe choice for tests)
      if (m.size() < nDim)
        m.resize(nDim, 0);
      else
        m.resize(nDim);
    }
    vec.push_back(m);
  }
  return MIVec(vec);
}

// Check existence of an MI in an MIVec via linear scan (uses MI::operator==)
static bool containsMI(const MIVec& mv, const MI& needle) {
  for (size_t i = 0; i < mv.nMI(); ++i) {
    if (mv[i] == needle) return true;
  }
  return false;
}

// Compute expected "downwards closed" property by brute force:
// For every multi-index m in mv, all componentwise smaller-or-equal indices must be present.
static bool expectedIsDownwardsClosed(const MIVec& mv) {
  const size_t nMI = mv.nMI();
  if (nMI == 0) return true;  // vacuously true

  const size_t ndim = mv[0].nDim();
  for (size_t i = 0; i < nMI; ++i) {
    const MI mi = mv[i];

    // iterate over all componentwise <= vectors (cartesian product)
    std::vector<MIType> counter(ndim, 0);
    std::vector<MIType> limits(ndim);
    for (size_t d = 0; d < ndim; ++d) limits[d] = mi[d];

    bool done = false;
    while (!done) {
      MI candidate;
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
static std::string miToString(const MI& m) {
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
static std::set<std::string> computeDownwardsClosureSet(const MIVec& mv) {
  std::set<std::string> s;
  if (mv.nMI() == 0) return s;
  const size_t ndim = mv[0].nDim();

  for (size_t i = 0; i < mv.nMI(); ++i) {
    const MI mi = mv[i];
    // counters
    std::vector<MIType> counter(ndim, 0);
    std::vector<MIType> limits(ndim);
    for (size_t d = 0; d < ndim; ++d) limits[d] = mi[d];

    bool done = false;
    while (!done) {
      MI candidate;
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

// 1) Empty MIVec should be downwards-closed (vacuous truth)
BOOST_AUTO_TEST_CASE(empty_mivec_is_downwards_closed) {
  // Create empty MIVec using the (nDim, nMI) constructor
  const size_t ndim = 3;
  MIVec empty(ndim, 0);
  BOOST_CHECK_EQUAL(empty.nMI(), 0u);
  BOOST_CHECK(empty.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(empty));
}

// 2) Single zero MI (all components 0) should be downwards-closed
BOOST_AUTO_TEST_CASE(single_zero_mi_is_downwards_closed) {
  const MI zero{0, 0, 0, 0};
  const MIVec mv(std::vector<MI>{zero});

  BOOST_CHECK_EQUAL(mv.nDim(), zero.nDim());
  BOOST_CHECK(mv.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(mv));
}

// 3) Single non-zero MI should NOT be downwards-closed unless all lower combos included
BOOST_AUTO_TEST_CASE(single_nonzero_mi_not_downwards_closed) {
  const MI mi{1, 0, 2};
  const MIVec mv(std::vector<MI>{mi});

  // This should be false because e.g. (0,0,0) etc. are missing.
  BOOST_CHECK(!mv.isDownwardsClosed());
  BOOST_CHECK(!expectedIsDownwardsClosed(mv));
}

// 4) Fully closed small 2D set: all combinations up to (1,1) -> should be closed
BOOST_AUTO_TEST_CASE(complete_rectangle_is_downwards_closed) {
  // All of: (0,0), (1,0), (0,1), (1,1)
  MIVec mv = mivecFromList(2, {{0, 0}, {1, 0}, {0, 1}, {1, 1}});

  // Try elements in random order to ensure order doesn't matter
  MIVec shuffled = mv;  // assuming copy ctor exists
  // (if you want to be strict about re-ordering, use setMI to reorder; but copy should be OK)

  BOOST_CHECK(shuffled.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(shuffled));
}

// 5) Missing one required lower element -> not downwards-closed
BOOST_AUTO_TEST_CASE(missing_lower_element_is_not_downwards_closed) {
  // Have (1,1) and (0,0) but missing (0,1)
  MIVec mv = mivecFromList(2, {{1, 1}, {0, 0}, {1, 0}});
  // (0,1) is missing so should be false
  BOOST_CHECK(!mv.isDownwardsClosed());
  BOOST_CHECK(!expectedIsDownwardsClosed(mv));
}

// 6) Duplicate entries should not affect decision
BOOST_AUTO_TEST_CASE(duplicates_do_not_affect_downwards_closedness) {
  MIVec mv = mivecFromList(2, {
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

// 7) Higher dimension example: 3D set that is closed
BOOST_AUTO_TEST_CASE(three_dimensional_closed_example) {
  // include all combinations up to (1,1,1) => 8 elements
  MIVec mv = mivecFromList(
      3, {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}});
  BOOST_CHECK(mv.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(mv));
}

// 8) downwardsClosure() should produce a downwards-closed superset
BOOST_AUTO_TEST_CASE(downwards_closure_produces_closed_superset) {
  MIVec mv = mivecFromList(2, {{2, 1},  // requires many lower elements
                               {0, 1},
                               {1, 0}});

  // original is likely not closed
  BOOST_CHECK(!mv.isDownwardsClosed());

  MIVec closed = mv.downwardsClosure();
  BOOST_CHECK(closed.isDownwardsClosed());

  // Original elements must be present in closure
  for (size_t i = 0; i < mv.nMI(); ++i) {
    BOOST_CHECK_MESSAGE(containsMI(closed, mv[i]),
                        "downwardsClosure() must contain original MI: " << miToString(mv[i]));
  }

  // The closure's unique set should equal brute-force computed set
  auto expectedSet = computeDownwardsClosureSet(mv);
  std::set<std::string> actualSet;
  for (size_t i = 0; i < closed.nMI(); ++i) {
    actualSet.insert(miToString(closed[i]));
  }
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedSet.begin(), expectedSet.end(), actualSet.begin(),
                                actualSet.end());
}

// 9) Randomized small tests: generate random MIVec, compute expected result, compare.
//    Keep deterministic seed for reproducibility in tests.
BOOST_AUTO_TEST_CASE(randomized_small_tests) {
  std::mt19937_64 rng(123456789ULL);
  const size_t ndim = 3;
  std::uniform_int_distribution<MIType> compDist(0, 2);  // small components
  for (int trial = 0; trial < 50; ++trial) {
    size_t nmi = 1 + (rng() % 10);
    std::vector<MI> v;
    for (size_t i = 0; i < nmi; ++i) {
      MI m;
      for (size_t d = 0; d < ndim; ++d) m.push_back(compDist(rng));
      v.push_back(m);
    }
    MIVec mv(v);
    bool expected = expectedIsDownwardsClosed(mv);
    bool actual = mv.isDownwardsClosed();
    BOOST_CHECK_MESSAGE(actual == expected,
                        "Random test mismatch. expected=" << expected << " actual=" << actual
                                                          << " ; nMI=" << mv.nMI());
  }
}

// 10) Large-but-sparse: ensures function handles bigger indices correctly (not performance test)
BOOST_AUTO_TEST_CASE(large_values_non_closed) {
  // Single huge index (3,2,1) but missing many lower ones
  MIVec mv = mivecFromList(3, {{3, 2, 1}});
  BOOST_CHECK(!mv.isDownwardsClosed());
  // The downwards closure should be closed and contain the (3,2,1)
  MIVec closed = mv.downwardsClosure();
  BOOST_CHECK(closed.isDownwardsClosed());
  BOOST_CHECK(containsMI(closed, MI{3, 2, 1}));
}

// 11) Check that ordering / insertion order doesn't matter: create closed set in shuffled order
BOOST_AUTO_TEST_CASE(order_independence_for_closed_sets) {
  MIVec mv = mivecFromList(2, {{1, 1}, {0, 1}, {1, 0}, {0, 0}});
  // shuffle by constructing new vector in different order:
  MIVec mv_shuffled = mivecFromList(2, {{0, 1}, {1, 0}, {1, 1}, {0, 0}});
  BOOST_CHECK(mv.isDownwardsClosed());
  BOOST_CHECK(mv_shuffled.isDownwardsClosed());
  BOOST_CHECK(expectedIsDownwardsClosed(mv));
  BOOST_CHECK(expectedIsDownwardsClosed(mv_shuffled));
}

BOOST_AUTO_TEST_SUITE_END()