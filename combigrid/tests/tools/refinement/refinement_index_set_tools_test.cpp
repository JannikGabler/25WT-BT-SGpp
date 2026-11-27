// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file refinement_index_set_tools_test.cpp
 * @brief Boost.Test cases of the index-set operations of the dimension-adaptive refinement:
 * incremental combination coefficients against @c tools::computeCTCoeffs on random admissible
 * insertion sequences (d = 1 ... 50), the backward-box enumeration against brute force, the
 * colexicographic order against @c LvlMIVec::downwardsClosure, forward neighbours of complete
 * sets.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_index_set.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/refinement/refinement_index_set_tools.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/complete_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <utility>
#include <vector>

using sgpp::combigrid::CTCoeffType;
using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlMIView;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::misc::RefinementIndexSet;
namespace refinement = sgpp::combigrid::tools::refinement;

namespace {

sgpp::base::RandomNumberGenerator& randGen = sgpp::base::RandomNumberGenerator::getInstance();

constexpr size_t NOT_FOUND = RefinementIndexSet::NOT_FOUND;

LvlMI toMI(const LvlMIView view) { return LvlMI(view.data(), view.data() + view.size()); }

/// The multi-indices of @p set in insertion order.
LvlMIVec toMIVec(const RefinementIndexSet& set) {
  LvlMIVec miVec(set.nDim(), 0);
  for (size_t idx = 0; idx < set.size(); idx++) miVec.push_back(set.levels(idx));
  return miVec;
}

/// Forward neighbours of the set that are not in it but whose backward neighbours all are.
std::vector<LvlMI> admissibleMargin(const RefinementIndexSet& set, const LvlType maxLvl) {
  std::vector<LvlMI> margin;
  for (size_t idx = 0; idx < set.size(); idx++) {
    for (size_t dim = 0; dim < set.nDim(); dim++) {
      LvlMI candidate = toMI(set.levels(idx));
      if (candidate[dim] >= maxLvl) continue;
      candidate[dim]++;
      if (set.find(candidate) != NOT_FOUND) continue;
      bool admissible = true;
      for (size_t k = 0; k < set.nDim() && admissible; k++) {
        if (candidate[k] == 0) continue;
        LvlMI backward = candidate;
        backward[k]--;
        admissible = set.find(backward) != NOT_FOUND;
      }
      if (admissible) margin.push_back(candidate);  // duplicates only change the odds
    }
  }
  return margin;
}

/**
 * Grows a downwards-closed set from the zero multi-index by @p nInserts random admissible
 * inserts, updating the coefficients incrementally; checks them against computeCTCoeffs every
 * @p checkEvery inserts and at the end.
 */
void checkRandomAdmissibleSequence(const size_t nDim, const size_t nInserts,
                                   const size_t checkEvery, const LvlType maxLvl) {
  RefinementIndexSet set(nDim);
  std::vector<CTCoeffType> coefficients;
  std::vector<size_t> support;

  for (size_t step = 0; step <= nInserts; step++) {
    LvlMI next(nDim);  // the zero multi-index first
    if (step > 0) {
      const std::vector<LvlMI> margin = admissibleMargin(set, maxLvl);
      if (margin.empty()) break;
      next = margin[randGen.getUniformIndexRN(margin.size())];
    }
    const size_t idx = set.insert(next);
    coefficients.push_back(0);
    refinement::addToCombinationCoefficients(set, idx, coefficients, support);

    if (step % checkEvery == 0 || step == nInserts) {
      BOOST_TEST_CONTEXT("nDim " << nDim << ", step " << step) {
        const std::vector<CTCoeffType> expected =
            sgpp::combigrid::tools::computeCTCoeffs(toMIVec(set));
        BOOST_CHECK_EQUAL_COLLECTIONS(coefficients.begin(), coefficients.end(), expected.begin(),
                                      expected.end());
      }
    }
  }
}

/// A random downwards-closed set grown by random admissible inserts (no coefficients).
RefinementIndexSet randomClosedSet(const size_t nDim, const size_t nInserts, const LvlType maxLvl) {
  RefinementIndexSet set(nDim);
  set.insert(LvlMI(nDim));
  for (size_t step = 0; step < nInserts; step++) {
    const std::vector<LvlMI> margin = admissibleMargin(set, maxLvl);
    if (margin.empty()) break;
    set.insert(margin[randGen.getUniformIndexRN(margin.size())]);
  }
  return set;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(Tools_Refinement_IndexSetTools)

BOOST_AUTO_TEST_CASE(Incremental_Coefficients_Equal_ComputeCTCoeffs) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    checkRandomAdmissibleSequence(1, 12, 1, 20);
    for (const size_t nDim : {2, 3, 5}) {
      checkRandomAdmissibleSequence(nDim, 120, 7, 10);
    }
    for (const size_t nDim : {10, 20, 50}) {
      checkRandomAdmissibleSequence(nDim, 80, 20, 6);
    }
  }
}

BOOST_AUTO_TEST_CASE(Complete_Set_Coefficients_Equal_ComputeCTCoeffs) {
  // Inserting in the closure's (colexicographic) order keeps every prefix downwards closed
  for (const auto& [nDim, maxLvl] : {std::pair<size_t, LvlType>{1, 6}, {2, 5}, {4, 3}, {8, 2}}) {
    const LvlMIVec closure =
        sgpp::combigrid::tools::genFullMIVecForCompleteSG(maxLvl, nDim).downwardsClosure();
    const RefinementIndexSet set = refinement::toIndexSet(closure);
    std::vector<CTCoeffType> coefficients(set.size(), 0);
    std::vector<size_t> support;
    for (size_t idx = 0; idx < set.size(); idx++) {
      refinement::addToCombinationCoefficients(set, idx, coefficients, support);
    }
    const std::vector<CTCoeffType> expected = sgpp::combigrid::tools::computeCTCoeffs(closure);
    BOOST_TEST_CONTEXT("nDim " << nDim << ", maxLvl " << maxLvl) {
      BOOST_CHECK_EQUAL_COLLECTIONS(coefficients.begin(), coefficients.end(), expected.begin(),
                                    expected.end());
    }
  }
}

BOOST_AUTO_TEST_CASE(Backward_Box_Is_Visited_Once_Per_Member_In_Mask_Order) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const RefinementIndexSet set = randomClosedSet(4, 150, 4);
    std::vector<size_t> support;

    for (size_t idx = 0; idx < set.size(); idx++) {
      std::vector<std::pair<size_t, int>> visited;
      refinement::forEachBackwardBoxMember(
          set, idx, support, [&](size_t member, int sign) { visited.emplace_back(member, sign); });

      const LvlMI mi = toMI(set.levels(idx));
      std::vector<size_t> dims;
      for (size_t dim = 0; dim < mi.size(); dim++) {
        if (mi[dim] > 0) dims.push_back(dim);
      }
      BOOST_REQUIRE_EQUAL(visited.size(), size_t{1} << dims.size());
      for (uint64_t mask = 0; mask < visited.size(); mask++) {
        LvlMI member = mi;
        int sign = 1;
        for (size_t i = 0; i < dims.size(); i++) {
          if ((mask >> i) & 1u) {
            member[dims[i]]--;
            sign = -sign;
          }
        }
        BOOST_TEST_CONTEXT("index " << idx << ", mask " << mask) {
          BOOST_CHECK_EQUAL(visited[mask].first, set.find(member));
          BOOST_CHECK_EQUAL(visited[mask].second, sign);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Colexicographic_Order_Equals_Downwards_Closure_Order) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (const size_t nDim : {1, 2, 3, 7}) {
      const RefinementIndexSet set = randomClosedSet(nDim, 100, 5);
      std::vector<size_t> all(set.size());
      for (size_t idx = 0; idx < all.size(); idx++) all[idx] = idx;

      const std::vector<size_t> order = refinement::sortColexicographically(set, all);
      const LvlMIVec closure = toMIVec(set).downwardsClosure();

      BOOST_REQUIRE_EQUAL(order.size(), closure.nMI());
      for (size_t pos = 0; pos < order.size(); pos++) {
        BOOST_TEST_CONTEXT("nDim " << nDim << ", position " << pos) {
          BOOST_CHECK(toMI(set.levels(order[pos])) == toMI(closure[pos]));
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Forward_Neighbours_Of_Complete_Sets) {
  for (const auto& [nDim, maxLvl] : {std::pair<size_t, LvlType>{1, 4}, {3, 3}, {6, 2}}) {
    const RefinementIndexSet set = refinement::toIndexSet(
        sgpp::combigrid::tools::genFullMIVecForCompleteSG(maxLvl, nDim).downwardsClosure());

    for (size_t idx = 0; idx < set.size(); idx++) {
      LvlType sum = 0;
      for (size_t dim = 0; dim < nDim; dim++) sum += set.levels(idx)[dim];
      BOOST_TEST_CONTEXT("nDim " << nDim << ", index " << idx) {
        // Only the indices below the maximum level sum have all forward neighbours
        BOOST_CHECK_EQUAL(refinement::hasAllForwardNeighbours(set, idx, 100), sum < maxLvl);
        // With the level cap 0 no forward neighbour is required
        BOOST_CHECK(refinement::hasAllForwardNeighbours(set, idx, 0));
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
