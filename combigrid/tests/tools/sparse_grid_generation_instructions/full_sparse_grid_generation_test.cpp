// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_generation.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

BOOST_AUTO_TEST_SUITE(Tools_getBarPosOfMIIdx)

BOOST_AUTO_TEST_CASE(Simple3DTest) {
  const std::vector<size_t> nMIs{1, 1 + 3, 1 + 3 + 6};

  // Sum = 0 (SumIdx = 0)
  std::vector<size_t> barPos = tools::full_sg_gen::getBarPosOfMIIdx(0, nMIs, 0, 1, 3);
  std::vector<size_t> expected{0, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  // Sum = 1 (SumIdx = 1)
  barPos = tools::full_sg_gen::getBarPosOfMIIdx(1, nMIs, 1, 2, 3);
  expected = {0, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  barPos = tools::full_sg_gen::getBarPosOfMIIdx(2, nMIs, 1, 2, 3);
  expected = {0, 2};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  barPos = tools::full_sg_gen::getBarPosOfMIIdx(3, nMIs, 1, 2, 3);
  expected = {1, 2};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  // Sum = 2 (SumIdx = 2)
  barPos = tools::full_sg_gen::getBarPosOfMIIdx(4, nMIs, 2, 3, 3);
  expected = {0, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  barPos = tools::full_sg_gen::getBarPosOfMIIdx(5, nMIs, 2, 3, 3);
  expected = {0, 2};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  barPos = tools::full_sg_gen::getBarPosOfMIIdx(6, nMIs, 2, 3, 3);
  expected = {0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  barPos = tools::full_sg_gen::getBarPosOfMIIdx(7, nMIs, 2, 3, 3);
  expected = {1, 2};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  barPos = tools::full_sg_gen::getBarPosOfMIIdx(8, nMIs, 2, 3, 3);
  expected = {1, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());

  barPos = tools::full_sg_gen::getBarPosOfMIIdx(9, nMIs, 2, 3, 3);
  expected = {2, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(barPos.begin(), barPos.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(RandomTest) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const MIType maxLvl = (MIType)randGen.getUniformIndexRN(10);
    const size_t nDim = 2 + randGen.getUniformIndexRN(3);  // nDim has to be at least 2

    const MIType minSum = tools::full_sg_gen::getMinComponentSum(maxLvl, nDim);
    const std::vector<size_t>& nMIs =
        tools::full_sg_gen::nMICntPerComponentSum(minSum, maxLvl, nDim);

    size_t miIdx = 0;
    for (MIType sum = minSum; sum <= maxLvl; sum++) {
      const size_t nMIWithSum = tools::numberOfMIWithComponentSum(nDim, sum);
      const size_t maxBarPos = tools::full_sg_gen::getMaxBarPos(sum, nDim);

      std::vector<size_t> iteratedBarPos = tools::full_sg_gen::initBarPos(nDim);

      for (size_t i = 0; i < nMIWithSum; i++) {
        const std::vector<size_t> resolvedBarPos =
            tools::full_sg_gen::getBarPosOfMIIdx(miIdx, nMIs, sum - minSum, maxBarPos, nDim);

        BOOST_CHECK_EQUAL_COLLECTIONS(iteratedBarPos.begin(), iteratedBarPos.end(),
                                      resolvedBarPos.begin(), resolvedBarPos.end());

        tools::full_sg_gen::incrementBarPos(iteratedBarPos, maxBarPos);
        miIdx++;
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Tools_genCoeffForFullSG)

BOOST_AUTO_TEST_CASE(RandomTest) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const MIType maxLvl = (MIType)randGen.getUniformIndexRN(14);
    const size_t nDim = randGen.getUniformIndexRN(4);  // nDim has to be at least 2

    const MIVec miVec = tools::genMIVecForFullSG(maxLvl, nDim);
    const std::vector<CTCoeffType> coeff = tools::genCoeffForFullSG(maxLvl, nDim);
    const std::vector<CTCoeffType> expectedCoeff1 = tools::computeCTCoeffs(miVec);

    BOOST_CHECK_MESSAGE(miVec.nMI() == coeff.size(), "The number of mis differs between '"
                                                         << miVec.nMI() << "' from the MIVec and '"
                                                         << coeff.size()
                                                         << "' from the coeff vector.");

    for (size_t miIdx = 0; miIdx < coeff.size(); miIdx++) {
      const MIType sum = miVec[miIdx].sumOfElems();
      const MIType diffToMaxLvl = maxLvl - sum;
      CTCoeffType expectedCoeff2 =
          tools::binomial((CTCoeffType)(nDim - 1), (CTCoeffType)(diffToMaxLvl));
      if (diffToMaxLvl % 2 == 1) {
        expectedCoeff2 *= -1;
      }

      BOOST_CHECK_MESSAGE(coeff[miIdx] == expectedCoeff1[miIdx],
                          "The coefficient at index '"
                              << miIdx << "' differs. The result states a coeff of '"
                              << coeff[miIdx] << "', whereas the first expected coeff states '"
                              << expectedCoeff1[miIdx] << "'.");
      BOOST_CHECK_MESSAGE(coeff[miIdx] == expectedCoeff2,
                          "The coefficient at index '"
                              << miIdx << "' differs. The result states a coeff of '"
                              << coeff[miIdx] << "', whereas the second expected coeff states '"
                              << expectedCoeff2 << "'.");
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()