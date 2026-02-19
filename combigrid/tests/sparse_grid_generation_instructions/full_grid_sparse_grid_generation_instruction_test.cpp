// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include "sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp"
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_grid_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/math_operations.hpp>
// #include <sgpp/combigrid/tools/random/random_number_generator.hpp>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

size_t requiredNumberOfMIs(const size_t nDim, const MIType maxLvl) {
  const MIType minSum = std::max(MIType(0), maxLvl - (MIType)nDim + 1);

  size_t totalNumber = 0;
  for (MIType sum = minSum; sum <= maxLvl; sum++) {
    const size_t n = tools::binomial(sum + nDim - 1, nDim - 1);
    totalNumber += n;
  }

  return totalNumber;
}

MIType sumOfMI(const MI& mi) {
  MIType sum = 0;

  for (const MIType v : mi) {
    sum += v;
  }

  return sum;
}

bool checkForDuplicates(const MIVec& miVec) {
  for (size_t i = 0; i < miVec.nMI(); i++) {
    for (size_t j = i + 1; j < miVec.nMI(); j++) {
      if (miVec[i] == miVec[j]) {
        return true;
      }
    }
  }

  return false;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(FullGridSGGenInstr_genCompleteMIVec)

BOOST_AUTO_TEST_CASE(Random1D) {
  const MIType maxLvl = (MIType)randGen.getUniformIndexRN(1000);

  const FullGridSGGenInstr instr(maxLvl, 1);
  const MIVec result = instr.genCompleteMIVec();

  BOOST_CHECK_MESSAGE(result.nDim() == 1, "Seed: " << randGen.getSeed());
  BOOST_CHECK_MESSAGE(result.nMI() == 1, "Seed: " << randGen.getSeed());
  BOOST_CHECK_MESSAGE(result(0, 0) == maxLvl, "Seed: " << randGen.getSeed());
}

BOOST_AUTO_TEST_CASE(RandomSerial) {
  const MIType maxLvl = (MIType)randGen.getUniformIndexRN(10);
  const size_t nDim = randGen.getUniformIndexRN(6);

  const FullGridSGGenInstr instr(maxLvl, 2);
  const MIVec result = instr.genCompleteMIVec();

  const size_t expectedMICnt = requiredNumberOfMIs(nDim, maxLvl);

  BOOST_CHECK_MESSAGE(result.nDim() == nDim, "Wrong nDim! Seed: " << randGen.getSeed());
  BOOST_CHECK_MESSAGE(result.nMI() == expectedMICnt, "Wrong nMI! Seed: " << randGen.getSeed());
  BOOST_CHECK_MESSAGE(checkForDuplicates(result), "Duplicates! Seed: " << randGen.getSeed());

  for (size_t miIdx = 0; miIdx < result.nMI(); miIdx++) {
    BOOST_CHECK_MESSAGE(sumOfMI(result[miIdx]) > maxLvl,
                        "MI at idx " << miIdx << " has a sum over maxLvl " << maxLvl
                                     << "! Seed: " << randGen.getSeed());
  }
}

BOOST_AUTO_TEST_SUITE_END()