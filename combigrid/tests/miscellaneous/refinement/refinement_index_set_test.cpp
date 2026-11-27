// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_index_set_test.cpp
 * @brief Boost.Test cases of @c misc::RefinementIndexSet: inserts and lookups, neighbour and
 * backward-box lookups against lookups of the materialized multi-indices, growth of the hash
 * table, 0- and 1-dimensional sets.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <vector>

using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlMIView;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::misc::RefinementIndexSet;

namespace {

sgpp::base::RandomNumberGenerator& randGen = sgpp::base::RandomNumberGenerator::getInstance();

constexpr size_t NOT_FOUND = RefinementIndexSet::NOT_FOUND;

/// Random multi-index with entries in [0, @p maxLvl].
LvlMI randomMI(const size_t nDim, const LvlType maxLvl) {
  LvlMI mi(nDim);
  for (size_t dim = 0; dim < nDim; dim++) {
    mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(maxLvl + 1));
  }
  return mi;
}

/// Copy of the multi-index viewed by @p view.
LvlMI toMI(const LvlMIView view) { return LvlMI(view.data(), view.data() + view.size()); }

/// Inserts up to @p nTries random multi-indices (duplicates skipped); returns them in order.
std::vector<LvlMI> fillRandomly(RefinementIndexSet& set, const size_t nTries,
                                const LvlType maxLvl) {
  std::vector<LvlMI> inserted;
  for (size_t i = 0; i < nTries; i++) {
    const LvlMI mi = randomMI(set.nDim(), maxLvl);
    if (set.find(mi) == NOT_FOUND) {
      BOOST_REQUIRE_EQUAL(set.insert(mi), inserted.size());
      inserted.push_back(mi);
    }
  }
  return inserted;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(Miscellaneous_RefinementIndexSet)

BOOST_AUTO_TEST_CASE(Insert_And_Find) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (const size_t nDim : {1, 2, 5, 20, 50}) {
      BOOST_TEST_CONTEXT("nDim " << nDim) {
        RefinementIndexSet set(nDim);
        const std::vector<LvlMI> inserted = fillRandomly(set, 3000, 4);

        BOOST_REQUIRE_EQUAL(set.size(), inserted.size());
        for (size_t idx = 0; idx < inserted.size(); idx++) {
          BOOST_CHECK_EQUAL(set.find(inserted[idx]), idx);
          BOOST_CHECK(toMI(set.levels(idx)) == inserted[idx]);
          BOOST_CHECK_EQUAL(set.hash(idx), set.hashOf(inserted[idx]));
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Absent_Multi_Indices_Are_Not_Found) {
  RefinementIndexSet set(3);
  set.insert(LvlMI{0, 0, 0});
  set.insert(LvlMI{1, 0, 2});

  BOOST_CHECK_EQUAL(set.find(LvlMI{1, 0, 1}), NOT_FOUND);
  BOOST_CHECK_EQUAL(set.find(LvlMI{0, 1, 0}), NOT_FOUND);
  BOOST_CHECK_EQUAL(set.findNeighbour(set.levels(0), set.hash(0), 1, -1), NOT_FOUND);
  BOOST_CHECK_EQUAL(set.findNeighbour(set.levels(0), set.hash(0), 0, 1), NOT_FOUND);
}

BOOST_AUTO_TEST_CASE(Neighbours_Equal_Lookups_Of_Materialized_Multi_Indices) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (const size_t nDim : {1, 3, 10}) {
      RefinementIndexSet set(nDim);
      const std::vector<LvlMI> inserted = fillRandomly(set, 800, 3);

      for (size_t idx = 0; idx < inserted.size(); idx++) {
        for (size_t dim = 0; dim < nDim; dim++) {
          for (const int step : {1, -1}) {
            LvlMI neighbour = inserted[idx];
            const bool valid = step > 0 || neighbour[dim] > 0;
            neighbour[dim] = static_cast<LvlType>(static_cast<int>(neighbour[dim]) + step);
            const size_t expected = valid ? set.find(neighbour) : NOT_FOUND;

            BOOST_TEST_CONTEXT("nDim " << nDim << ", index " << idx << ", dim " << dim << ", step "
                                       << step) {
              BOOST_CHECK_EQUAL(set.findNeighbour(inserted[idx], set.hash(idx), dim, step),
                                expected);
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Backward_Box_Members_Equal_Lookups_Of_Materialized_Multi_Indices) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (const size_t nDim : {1, 2, 6}) {
      RefinementIndexSet set(nDim);
      const std::vector<LvlMI> inserted = fillRandomly(set, 600, 2);

      for (size_t idx = 0; idx < inserted.size(); idx++) {
        std::vector<size_t> support;
        for (size_t dim = 0; dim < nDim; dim++) {
          if (inserted[idx][dim] > 0) support.push_back(dim);
        }
        for (uint64_t mask = 0; mask < (uint64_t{1} << support.size()); mask++) {
          LvlMI member = inserted[idx];
          for (size_t i = 0; i < support.size(); i++) {
            member[support[i]] -= static_cast<LvlType>((mask >> i) & 1u);
          }
          BOOST_TEST_CONTEXT("nDim " << nDim << ", index " << idx << ", mask " << mask) {
            BOOST_CHECK_EQUAL(set.findBackward(inserted[idx], set.hash(idx), support, mask),
                              set.find(member));
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Many_Inserts_Grow_The_Table) {
  RefinementIndexSet set(3);
  set.reserve(10);  // far too small: the table grows on its own
  size_t n = 0;
  for (LvlType a = 0; a < 30; a++) {
    for (LvlType b = 0; b < 30; b++) {
      for (LvlType c = 0; c < 20; c++) {
        BOOST_REQUIRE_EQUAL(set.insert(LvlMI{a, b, c}), n++);
      }
    }
  }
  BOOST_CHECK_EQUAL(set.size(), 18000u);
  BOOST_CHECK_EQUAL(set.find(LvlMI{29, 29, 19}), 17999u);
  BOOST_CHECK_EQUAL(set.find(LvlMI{12, 7, 3}), (12u * 30u + 7u) * 20u + 3u);
  BOOST_CHECK_EQUAL(set.find(LvlMI{30, 0, 0}), NOT_FOUND);
}

BOOST_AUTO_TEST_CASE(Zero_Dimensional) {
  RefinementIndexSet set(0);
  const LvlMI empty(0);

  BOOST_CHECK_EQUAL(set.find(empty), NOT_FOUND);
  BOOST_CHECK_EQUAL(set.insert(empty), 0u);
  BOOST_CHECK_EQUAL(set.find(empty), 0u);
  BOOST_CHECK_EQUAL(set.findBackward(empty, set.hash(0), {}, 0), 0u);
}

BOOST_AUTO_TEST_SUITE_END()
