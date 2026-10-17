// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file full_multiindex_vector_generation_test.cpp
 * @brief Boost.Test cases for @c SGGenInstr::genFullMIVec and @c genFullMIVecWithCoeffs of
 * @c CompleteSGGenInstr and @c MIVecSGGenInstr, which return all multi-indices of the downwards
 * closed set including those with combination coefficient 0 (as needed by @c DeltasSparseGrid),
 * their consistency with the reduced variants, and the partitioning helpers of
 * @c tools::extensive_sparse_grid_generation.
 *
 * Expected multi-indices and coefficients are computed by closed formulas or brute force.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <functional>
#include <map>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/extensive_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

/// Multi-index as a plain vector, usable as a key of ordered containers.
using MIKey = std::vector<LvlType>;

MIKey toKey(const LvlMI& mi) {
  MIKey key(mi.size());

  for (size_t dim = 0; dim < mi.size(); dim++) {
    key[dim] = mi[dim];
  }

  return key;
}

std::string toString(const MIKey& mi) {
  std::string result = "(";

  for (size_t dim = 0; dim < mi.size(); dim++) {
    result += (dim == 0 ? "" : ", ") + std::to_string(mi[dim]);
  }

  return result + ")";
}

/// Calls @p func for every multi-index l with 0 <= l <= @p upper (component-wise).
void forEachMIInBox(const LvlMI& upper, const std::function<void(const LvlMI&)>& func) {
  LvlMI mi(upper.size(), LvlType{0});

  const std::function<void(size_t)> recurse = [&](const size_t dim) {
    if (dim == upper.size()) {
      func(mi);
      return;
    }

    for (LvlType lvl = 0; lvl <= upper[dim]; lvl++) {
      mi[dim] = lvl;
      recurse(dim + 1);
    }
  };

  recurse(0);
}

/**
 * Expected multi-indices of a complete sparse grid of level @p maxLvl: every l with
 * |l|_1 <= maxLvl, with the closed-form coefficient (-1)^q * binom(d - 1, q), q = maxLvl - |l|_1.
 */
std::map<MIKey, CTCoeffType> expectedCompleteMIs(const LvlType maxLvl, const size_t nDim) {
  std::map<MIKey, CTCoeffType> expected;

  forEachMIInBox(LvlMI(nDim, maxLvl), [&](const LvlMI& mi) {
    const LvlType lvlSum = mi.sumOfElems();
    if (lvlSum > maxLvl) return;

    const CTCoeffType q = static_cast<CTCoeffType>(maxLvl - lvlSum);
    expected[toKey(mi)] =
        (q % 2 == 0 ? 1 : -1) * tools::binomial(static_cast<CTCoeffType>(nDim) - 1, q);
  });

  return expected;
}

/**
 * Expected multi-indices of an @c MIVecSGGenInstr with input @p inputMIs, by brute force: the
 * downwards closure I of the input with the inclusion-exclusion coefficients
 * c_l = sum over z in {0,1}^d with l + z in I of (-1)^|z|_1.
 */
std::map<MIKey, CTCoeffType> expectedMIVecMIs(const std::vector<LvlMI>& inputMIs,
                                              const size_t nDim) {
  const auto isInClosure = [&inputMIs, nDim](const LvlMI& mi) {
    return std::any_of(inputMIs.begin(), inputMIs.end(), [&mi, nDim](const LvlMI& inputMI) {
      for (size_t dim = 0; dim < nDim; dim++) {
        if (mi[dim] > inputMI[dim]) return false;
      }
      return true;
    });
  };

  LvlMI upper(nDim, LvlType{0});
  for (const LvlMI& inputMI : inputMIs) {
    for (size_t dim = 0; dim < nDim; dim++) {
      upper[dim] = std::max(upper[dim], inputMI[dim]);
    }
  }

  std::map<MIKey, CTCoeffType> expected;

  forEachMIInBox(upper, [&](const LvlMI& mi) {
    if (!isInClosure(mi)) return;

    CTCoeffType coeff = 0;
    forEachMIInBox(LvlMI(nDim, LvlType{1}), [&](const LvlMI& z) {
      if (isInClosure(mi + z)) {
        coeff += z.sumOfElems() % 2 == 0 ? 1 : -1;
      }
    });

    expected[toKey(mi)] = coeff;
  });

  return expected;
}

/// Only the entries of @p mis with a non-zero coefficient.
std::map<MIKey, CTCoeffType> withoutZeroCoeffs(const std::map<MIKey, CTCoeffType>& mis) {
  std::map<MIKey, CTCoeffType> result;

  for (const auto& [mi, coeff] : mis) {
    if (coeff != 0) result[mi] = coeff;
  }

  return result;
}

/**
 * Checks that @p miVec and @p coeffs contain every entry of @p expected exactly once (in any
 * order) with the expected coefficient, and nothing else.
 */
void checkMIsWithCoeffs(const LvlMIVec& miVec, const std::vector<CTCoeffType>& coeffs,
                        const std::map<MIKey, CTCoeffType>& expected) {
  BOOST_REQUIRE_EQUAL(miVec.nMI(), coeffs.size());
  BOOST_CHECK_EQUAL(miVec.nMI(), expected.size());

  std::map<MIKey, size_t> occurrences;

  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const MIKey mi = toKey(miVec[miIdx]);
    occurrences[mi]++;

    BOOST_TEST_CONTEXT("Multi-index " << toString(mi)) {
      const auto iter = expected.find(mi);

      if (iter == expected.end()) {
        BOOST_ERROR("Unexpected multi-index");
      } else {
        BOOST_CHECK_EQUAL(coeffs[miIdx], iter->second);
      }
    }
  }

  for (const auto& [mi, count] : occurrences) {
    BOOST_TEST_CONTEXT("Multi-index " << toString(mi)) { BOOST_CHECK_EQUAL(count, size_t{1}); }
  }
}

/// Checks that @p a and @p b contain the same multi-indices in the same order.
void checkSameMIVec(const LvlMIVec& a, const LvlMIVec& b) {
  BOOST_REQUIRE_EQUAL(a.nMI(), b.nMI());
  BOOST_REQUIRE_EQUAL(a.nDim(), b.nDim());

  for (size_t miIdx = 0; miIdx < a.nMI(); miIdx++) {
    BOOST_TEST_CONTEXT("Index " << miIdx) { BOOST_CHECK(toKey(a[miIdx]) == toKey(b[miIdx])); }
  }
}

/// Random multi-index with components in [0, @p maxComp].
LvlMI randomMI(const size_t nDim, const LvlType maxComp) {
  LvlMI mi(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(maxComp + 1));
  }

  return mi;
}

/// Random input multi-indices for an @c MIVecSGGenInstr.
std::vector<LvlMI> randomInputMIs(const size_t nDim) {
  const size_t nInputMIs = 1 + randGen.getUniformIndexRN(5);
  std::vector<LvlMI> inputMIs;

  for (size_t i = 0; i < nInputMIs; i++) {
    inputMIs.push_back(randomMI(nDim, 4));
  }

  return inputMIs;
}

}  // namespace

/*********************
CompleteSGGenInstr
*********************/

BOOST_AUTO_TEST_SUITE(CompleteSGGenInstr_full_multiindex_vector)

BOOST_AUTO_TEST_CASE(Full_MIVec_With_Coeffs) {
  for (size_t nDim = 2; nDim <= 5; nDim++) {
    for (LvlType maxLvl = 0; maxLvl <= 6; maxLvl++) {
      BOOST_TEST_CONTEXT("nDim " << nDim << ", max. level " << maxLvl) {
        const CompleteSGGenInstr genInstr(maxLvl, nDim);
        const auto miVecWithCoeffs = genInstr.genFullMIVecWithCoeffs();
        const LvlMIVec& miVec = miVecWithCoeffs.first;
        const std::vector<CTCoeffType>& coeffs = miVecWithCoeffs.second;

        BOOST_CHECK_EQUAL(miVec.nDim(), nDim);
        BOOST_CHECK_EQUAL(miVec.nMI(), tools::binomial(maxLvl + nDim, nDim));
        checkMIsWithCoeffs(miVec, coeffs, expectedCompleteMIs(maxLvl, nDim));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Full_MIVec_Matches_Full_MIVec_With_Coeffs) {
  for (size_t nDim = 2; nDim <= 5; nDim++) {
    for (LvlType maxLvl = 0; maxLvl <= 6; maxLvl++) {
      BOOST_TEST_CONTEXT("nDim " << nDim << ", max. level " << maxLvl) {
        const CompleteSGGenInstr genInstr(maxLvl, nDim);

        checkSameMIVec(genInstr.genFullMIVec(), genInstr.genFullMIVecWithCoeffs().first);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Reduced_Is_Full_Without_Zero_Coefficients) {
  for (size_t nDim = 2; nDim <= 5; nDim++) {
    for (LvlType maxLvl = 0; maxLvl <= 6; maxLvl++) {
      BOOST_TEST_CONTEXT("nDim " << nDim << ", max. level " << maxLvl) {
        const CompleteSGGenInstr genInstr(maxLvl, nDim);
        const auto miVecWithCoeffs = genInstr.genReducedMIVecWithCoeffs();
        const LvlMIVec& reducedMIVec = miVecWithCoeffs.first;
        const std::vector<CTCoeffType>& reducedCoeffs = miVecWithCoeffs.second;

        checkMIsWithCoeffs(reducedMIVec, reducedCoeffs,
                           withoutZeroCoeffs(expectedCompleteMIs(maxLvl, nDim)));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Large_Grid_Uses_Parallel_Generation) {
  // More multi-indices than constants::sg_gen_instr::CSG_MIN_MI_FOR_CONCURRENCY
  const CompleteSGGenInstr genInstr(8, 6);
  const auto miVecWithCoeffs = genInstr.genFullMIVecWithCoeffs();
  const LvlMIVec& miVec = miVecWithCoeffs.first;
  const std::vector<CTCoeffType>& coeffs = miVecWithCoeffs.second;

  BOOST_CHECK_EQUAL(miVec.nMI(), size_t{3003});  // binom(8 + 6, 6)
  checkMIsWithCoeffs(miVec, coeffs, expectedCompleteMIs(8, 6));
}

BOOST_AUTO_TEST_CASE(Zero_Dims) {
  const CompleteSGGenInstr genInstr(3, 0);

  const auto miVecWithCoeffs = genInstr.genFullMIVecWithCoeffs();
  const LvlMIVec& fullMIVec = miVecWithCoeffs.first;
  const std::vector<CTCoeffType>& fullCoeffs = miVecWithCoeffs.second;
  BOOST_CHECK_EQUAL(fullMIVec.nMI(), size_t{0});
  BOOST_CHECK(fullCoeffs.empty());
  BOOST_CHECK_EQUAL(genInstr.genFullMIVec().nMI(), size_t{0});
}

BOOST_AUTO_TEST_CASE(One_Dimension) {
  for (LvlType maxLvl = 0; maxLvl <= 6; maxLvl++) {
    BOOST_TEST_CONTEXT("Max. level " << maxLvl) {
      const CompleteSGGenInstr genInstr(maxLvl, 1);

      // The levels 0, ..., maxLvl; only maxLvl has a non-zero coefficient
      const auto miVecWithCoeffs = genInstr.genFullMIVecWithCoeffs();
      const LvlMIVec& miVec = miVecWithCoeffs.first;
      const std::vector<CTCoeffType>& coeffs = miVecWithCoeffs.second;
      checkMIsWithCoeffs(miVec, coeffs, expectedCompleteMIs(maxLvl, 1));
      checkSameMIVec(genInstr.genFullMIVec(), miVec);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

/******************
MIVecSGGenInstr
******************/

BOOST_AUTO_TEST_SUITE(MIVecSGGenInstr_full_multiindex_vector)

BOOST_AUTO_TEST_CASE(Handwritten_2D) {
  // Downwards closure of {(2, 0), (0, 3)}: 6 multi-indices, three of them with coefficient 0
  const LvlMIVec input{{2, 0}, {0, 3}};
  const MIVecSGGenInstr genInstr(input);

  const auto miVecWithCoeffs = genInstr.genFullMIVecWithCoeffs();
  const LvlMIVec& miVec = miVecWithCoeffs.first;
  const std::vector<CTCoeffType>& coeffs = miVecWithCoeffs.second;

  checkMIsWithCoeffs(
      miVec, coeffs,
      {{{0, 0}, -1}, {{1, 0}, 0}, {{2, 0}, 1}, {{0, 1}, 0}, {{0, 2}, 0}, {{0, 3}, 1}});
}

BOOST_AUTO_TEST_CASE(Full_MIVec_With_Coeffs) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 30; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const std::vector<LvlMI> inputMIs = randomInputMIs(nDim);

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim) {
        const LvlMIVec input(inputMIs);
        const MIVecSGGenInstr genInstr(input);

        const auto miVecWithCoeffs = genInstr.genFullMIVecWithCoeffs();
        const LvlMIVec& miVec = miVecWithCoeffs.first;
        const std::vector<CTCoeffType>& coeffs = miVecWithCoeffs.second;
        checkMIsWithCoeffs(miVec, coeffs, expectedMIVecMIs(inputMIs, nDim));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Full_MIVec_Matches_Full_MIVec_With_Coeffs) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 30; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim) {
        const LvlMIVec input(randomInputMIs(nDim));
        const MIVecSGGenInstr genInstr(input);

        checkSameMIVec(genInstr.genFullMIVec(), genInstr.genFullMIVecWithCoeffs().first);
        checkSameMIVec(genInstr.genReducedMIVec(), genInstr.genReducedMIVecWithCoeffs().first);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Reduced_Is_Full_Without_Zero_Coefficients) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 30; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const std::vector<LvlMI> inputMIs = randomInputMIs(nDim);

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim) {
        const LvlMIVec input(inputMIs);
        const MIVecSGGenInstr genInstr(input);

        const auto miVecWithCoeffs = genInstr.genReducedMIVecWithCoeffs();
        const LvlMIVec& miVec = miVecWithCoeffs.first;
        const std::vector<CTCoeffType>& coeffs = miVecWithCoeffs.second;
        checkMIsWithCoeffs(miVec, coeffs, withoutZeroCoeffs(expectedMIVecMIs(inputMIs, nDim)));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Zero_Coefficients_Come_First) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 30; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim) {
        const LvlMIVec input(randomInputMIs(nDim));
        const MIVecSGGenInstr genInstr(input);

        const std::vector<CTCoeffType> coeffs = genInstr.genFullMIVecWithCoeffs().second;

        BOOST_CHECK(std::is_partitioned(coeffs.begin(), coeffs.end(),
                                        [](const CTCoeffType coeff) { return coeff == 0; }));
        BOOST_CHECK(
            tools::extensive_sparse_grid_generation::areTGsCorrectlyPartitionedForExtensiveSG(
                coeffs));
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

/*************************
Partitioning helpers
*************************/

BOOST_AUTO_TEST_SUITE(extensive_sparse_grid_generation_partitioning)

BOOST_AUTO_TEST_CASE(Handwritten_Partition) {
  LvlMIVec miVec{{0, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}};
  std::vector<CTCoeffType> coeffs{0, 1, 0, -1, 0};

  const size_t nZeroCoeffs =
      tools::extensive_sparse_grid_generation::partitionForExtensiveSG(miVec, coeffs);

  // Stable: both groups keep their relative order
  BOOST_CHECK_EQUAL(nZeroCoeffs, size_t{3});
  checkSameMIVec(miVec, LvlMIVec{{0, 0}, {0, 1}, {1, 1}, {1, 0}, {2, 0}});
  BOOST_CHECK(coeffs == (std::vector<CTCoeffType>{0, 0, 0, 1, -1}));
}

BOOST_AUTO_TEST_CASE(Random_Partition_Is_Stable) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 30; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const size_t nMI = randGen.getUniformIndexRN(40);

      std::vector<std::pair<MIKey, CTCoeffType>> expected;
      std::vector<LvlMI> mis;
      std::vector<CTCoeffType> coeffs;

      for (size_t i = 0; i < nMI; i++) {
        const LvlMI mi = randomMI(nDim, 5);
        const CTCoeffType coeff = static_cast<CTCoeffType>(randGen.getUniformIndexRN(5)) - 2;

        mis.push_back(mi);
        coeffs.push_back(coeff);
        expected.emplace_back(toKey(mi), coeff);
      }

      std::stable_partition(expected.begin(), expected.end(),
                            [](const auto& entry) { return entry.second == 0; });
      const size_t expectedNZeroCoeffs =
          static_cast<size_t>(std::count(coeffs.begin(), coeffs.end(), CTCoeffType{0}));

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", nMI: " << nMI) {
        LvlMIVec miVec = nMI == 0 ? LvlMIVec(nDim, 0) : LvlMIVec(mis);

        const size_t nZeroCoeffs =
            tools::extensive_sparse_grid_generation::partitionForExtensiveSG(miVec, coeffs);

        BOOST_CHECK_EQUAL(nZeroCoeffs, expectedNZeroCoeffs);
        BOOST_REQUIRE_EQUAL(miVec.nMI(), nMI);
        BOOST_REQUIRE_EQUAL(coeffs.size(), nMI);

        for (size_t i = 0; i < nMI; i++) {
          BOOST_TEST_CONTEXT("Index " << i) {
            BOOST_CHECK(toKey(miVec[i]) == expected[i].first);
            BOOST_CHECK_EQUAL(coeffs[i], expected[i].second);
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Correctly_Partitioned_Predicate) {
  using tools::extensive_sparse_grid_generation::areTGsCorrectlyPartitionedForExtensiveSG;

  BOOST_CHECK(areTGsCorrectlyPartitionedForExtensiveSG({}));
  BOOST_CHECK(areTGsCorrectlyPartitionedForExtensiveSG({0, 0}));
  BOOST_CHECK(areTGsCorrectlyPartitionedForExtensiveSG({1, -1}));
  BOOST_CHECK(areTGsCorrectlyPartitionedForExtensiveSG({0, 0, 1, 2, -3}));
  BOOST_CHECK(!areTGsCorrectlyPartitionedForExtensiveSG({1, 0}));
  BOOST_CHECK(!areTGsCorrectlyPartitionedForExtensiveSG({0, 1, 0}));
  BOOST_CHECK(!areTGsCorrectlyPartitionedForExtensiveSG({0, -1, 2, 0, 1}));
}

BOOST_AUTO_TEST_SUITE_END()
