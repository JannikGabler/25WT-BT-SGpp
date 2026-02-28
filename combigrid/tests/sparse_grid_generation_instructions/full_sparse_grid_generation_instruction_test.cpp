// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

size_t requiredNumberOfMIs(const size_t nDim, const LvlType maxLvl) {
  LvlType minSum = 0;
  if (nDim <= maxLvl) {
    minSum = maxLvl - static_cast<LvlType>(nDim + 1);
  }

  size_t totalNumber = 0;
  for (LvlType sum = minSum; sum <= maxLvl; sum++) {
    const size_t n = tools::binomial(sum + nDim - 1, nDim - 1);
    totalNumber += n;
  }

  return totalNumber;
}

bool checkForDuplicates(const LvlMIVec& miVec) {
  bool foundDuplicate = false;

#pragma omp parallel for shared(foundDuplicate) schedule(static)
  for (size_t i = 0; i < miVec.nMI(); i++) {
    for (size_t j = i + 1; j < miVec.nMI(); j++) {
      if (miVec[i] == miVec[j]) {
        const LvlMI mi1 = miVec[i];
        const LvlMI mi2 = miVec[j];

        foundDuplicate = true;
#pragma omp cancel for
      }
    }
#pragma omp cancellation point for
  }

  return foundDuplicate;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(FullSGGenInstr_genCompleteMIVec)

BOOST_AUTO_TEST_CASE(Random1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(1000));

    const FullSGGenInstr instr(maxLvl, 1);
    const LvlMIVec result = instr.genMIVec();

    BOOST_CHECK_MESSAGE(result.nDim() == 1, "Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(result.nMI() == 1, "Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(result(0, 0) == maxLvl, "Seed: " << randGen.getSeed());
  }
}

BOOST_AUTO_TEST_CASE(RandomSerial) {
  randGen.setSeed(1772301879);
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(10));
    const size_t nDim = randGen.getUniformIndexRN(6);

    const FullSGGenInstr instr(maxLvl, nDim);
    const LvlMIVec result = instr.genMIVec();

    const size_t expectedMICnt = requiredNumberOfMIs(nDim, maxLvl);

    BOOST_CHECK(result.nDim() == nDim);
    BOOST_CHECK(result.nMI() == expectedMICnt);
    BOOST_CHECK(!checkForDuplicates(result));

    for (size_t miIdx = 0; miIdx < result.nMI(); miIdx++) {
      const LvlType componentSum = result[miIdx].sumOfElems();
      BOOST_CHECK_MESSAGE(componentSum <= maxLvl, "MI at idx '"
                                                      << miIdx << "' has a sum of '" << componentSum
                                                      << "', which is larger than the maxLvl '"
                                                      << maxLvl << "'!");

      if (componentSum > maxLvl) {
        break;
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(ResolveTest) {}

BOOST_AUTO_TEST_CASE(RandomParallel) {
  randGen.setSeed(1772302453);
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = 12 + static_cast<LvlType>(randGen.getUniformIndexRN(3));
    const size_t nDim = 4 + randGen.getUniformIndexRN(3);

    const FullSGGenInstr instr(maxLvl, nDim);
    const LvlMIVec result = instr.genMIVec();

    const size_t expectedMICnt = requiredNumberOfMIs(nDim, maxLvl);

    // Enough mis for concurrency?
    const size_t minMICntForConcurrency =
        std::max(constants::sg_gen_instr::FSG_MIN_MI_FOR_CONCURRENCY,
                 2 * constants::sg_gen_instr::FSG_MIN_MI_PER_THREAD);
    BOOST_CHECK_MESSAGE(expectedMICnt >= minMICntForConcurrency,
                        "Not enough mis for concurrency! Test will generate '"
                            << expectedMICnt << "' mis but at least '" << minMICntForConcurrency
                            << "' are required for concurrency");

    // Is result right?
    BOOST_CHECK(result.nDim() == nDim);
    BOOST_CHECK(result.nMI() == expectedMICnt);
    BOOST_CHECK(!checkForDuplicates(result));

#pragma omp parallel for schedule(static)
    for (size_t miIdx = 0; miIdx < result.nMI(); miIdx++) {
      const LvlType componentSum = result[miIdx].sumOfElems();

      if (componentSum > maxLvl) {  // Pre check (BOOST struggles with concurrency)
        BOOST_REQUIRE_MESSAGE(componentSum <= maxLvl,
                              "MI at idx '" << miIdx << "' has a sum of '" << componentSum
                                            << "', which is larger than the maxLvl '" << maxLvl
                                            << "'!");
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()