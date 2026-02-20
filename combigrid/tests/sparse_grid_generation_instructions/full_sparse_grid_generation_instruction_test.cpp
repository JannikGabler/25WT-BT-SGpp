// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <boost/test/tools/context.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/math_operations.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <string>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

size_t requiredNumberOfMIs(const size_t nDim, const MIType maxLvl) {
  MIType minSum = 0;
  if (nDim <= maxLvl) {
    minSum = maxLvl - (MIType)nDim + 1;
  }

  size_t totalNumber = 0;
  for (MIType sum = minSum; sum <= maxLvl; sum++) {
    const size_t n = tools::binomial(sum + nDim - 1, nDim - 1);
    totalNumber += n;
  }

  return totalNumber;
}

bool checkForDuplicates(const MIVec& miVec) {
  for (size_t i = 0; i < miVec.nMI(); i++) {
    for (size_t j = i + 1; j < miVec.nMI(); j++) {
      if (miVec[i] == miVec[j]) {
        const MI mi1 = miVec[i];
        const MI mi2 = miVec[j];

        return true;
      }
    }
  }

  return false;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(FullSGGenInstr_genCompleteMIVec)

BOOST_AUTO_TEST_CASE(Random1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const MIType maxLvl = (MIType)randGen.getUniformIndexRN(1000);

    const FullSGGenInstr instr(maxLvl, 1);
    const MIVec result = instr.genCompleteMIVec();

    BOOST_CHECK_MESSAGE(result.nDim() == 1, "Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(result.nMI() == 1, "Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(result(0, 0) == maxLvl, "Seed: " << randGen.getSeed());
  }
}

BOOST_AUTO_TEST_CASE(RandomSerial) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const MIType maxLvl = (MIType)randGen.getUniformIndexRN(10);
    const size_t nDim = randGen.getUniformIndexRN(6);

    const FullSGGenInstr instr(maxLvl, nDim);
    const MIVec result = instr.genCompleteMIVec();

    const size_t expectedMICnt = requiredNumberOfMIs(nDim, maxLvl);

    BOOST_CHECK_MESSAGE(result.nDim() == nDim, "Wrong nDim! Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(result.nMI() == expectedMICnt, "Wrong nMI! Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(!checkForDuplicates(result), "Duplicates! Seed: " << randGen.getSeed());

    for (size_t miIdx = 0; miIdx < result.nMI(); miIdx++) {
      const MIType componentSum = result[miIdx].sumOfElems();
      BOOST_CHECK_MESSAGE(componentSum <= maxLvl,
                          "MI at idx '" << miIdx << "' has a sum of '" << componentSum
                                        << "', which is larger than the maxLvl '" << maxLvl
                                        << "'! Seed: " << randGen.getSeed());

      if (componentSum > maxLvl) {
        break;
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(ResolveTest) {}

BOOST_AUTO_TEST_CASE(RandomParallel) {
  randGen.setSeed(1771596475);
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const MIType maxLvl = 12 + (MIType)randGen.getUniformIndexRN(5);
    const size_t nDim = 4 + randGen.getUniformIndexRN(3);

    const FullSGGenInstr instr(maxLvl, nDim);
    const MIVec result = instr.genCompleteMIVec();

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
    BOOST_CHECK_MESSAGE(result.nDim() == nDim, "Wrong nDim! Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(result.nMI() == expectedMICnt, "Wrong nMI! Seed: " << randGen.getSeed());
    BOOST_CHECK_MESSAGE(!checkForDuplicates(result), "Duplicates! Seed: " << randGen.getSeed());

    for (size_t miIdx = 0; miIdx < result.nMI(); miIdx++) {
      const MIType componentSum = result[miIdx].sumOfElems();
      BOOST_CHECK_MESSAGE(componentSum <= maxLvl,
                          "MI at idx '" << miIdx << "' has a sum of '" << componentSum
                                        << "', which is larger than the maxLvl '" << maxLvl
                                        << "'! Seed: " << randGen.getSeed());

      if (componentSum > maxLvl) {
        break;
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()