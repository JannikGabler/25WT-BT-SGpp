// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cassert>
#include <cstring>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_functions.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

std::vector<sgpp::base::DataVector> prod2DGPs(const sgpp::base::DataVector& nodes1,
                                              const sgpp::base::DataVector& nodes2) {
  std::vector<sgpp::base::DataVector> gps;

  for (const double node2 : nodes2) {
    for (const double node1 : nodes1) {
      gps.push_back(sgpp::base::DataVector{node1, node2});
    }
  }

  return gps;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(SparseGrid_Constructor)

BOOST_AUTO_TEST_CASE(Zero_Dims) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = (LvlType)randGen.getUniformIndexRN(10);

    const FullSGGenInstr genInstr(maxLvl, 0);

    const SparseGrid expected(0);

    const SparseGrid result(genInstr);

    BOOST_CHECK(result == expected);
  }
}

BOOST_AUTO_TEST_CASE(Random_Full_1D_SG) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = (LvlType)randGen.getUniformIndexRN(10000);

    FullSGGenInstr genInstr(maxLvl, 1);
    genInstr.setNodeGenFuncs({genEquidistantNodes});
    genInstr.setLvl2GPCntFuncs({linearLvl2GPCntFunction});

    // Expected
    SparseGrid expected(1, 1);

    const LvlMI mi({maxLvl});

    const CTCoeffType coeff = 1;

    const sgpp::base::DataVector nodes = genEquidistantNodes(linearLvl2GPCntFunction(maxLvl));
    std::vector<sgpp::base::DataVector> gps(nodes.size() + 2);
    gps[0] = sgpp::base::DataVector{0};
    for (size_t i = 0; i < nodes.size(); i++) {
      gps[i + 1] = sgpp::base::DataVector{nodes[i]};
    }
    gps[gps.size() - 1] = sgpp::base::DataVector{1};
    const TensorGrid tg({(GPCntType)(nodes.size() + 2)}, gps);

    expected.setTensorGrid(0, TensorGridCTData{mi, coeff, tg});

    // Result
    const SparseGrid result(genInstr);

    // Comparison
    BOOST_CHECK(result == expected);
  }
}

BOOST_AUTO_TEST_CASE(Complex_Full_2D_SG) {
  FullSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFuncs({genEquidistantNodes, genClenshawCurtisNodes});
  genInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryIndexOffset(1);

  // Expected
  SparseGrid expected(2);

  // Tensor grid (2, 0)
  LvlMI mi({2, 0});

  CTCoeffType coeff = 1;

  sgpp::base::DataVector nodesDim1{double(0),
                                   genEquidistantNodes(7)[0],
                                   genEquidistantNodes(7)[1],
                                   genEquidistantNodes(7)[2],
                                   genEquidistantNodes(7)[3],
                                   genEquidistantNodes(7)[4],
                                   genEquidistantNodes(7)[5],
                                   genEquidistantNodes(7)[6],
                                   double(1)};
  sgpp::base::DataVector nodesDim2{genClenshawCurtisNodes(1)[0]};

  TensorGrid tg({GPCntType(9), GPCntType(1)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (1, 1)
  mi = LvlMI({1, 1});

  coeff = 1;

  nodesDim1 =
      sgpp::base::DataVector{double(0), genEquidistantNodes(3)[0], genEquidistantNodes(3)[1],
                             genEquidistantNodes(3)[2], double(1)};
  nodesDim2 = sgpp::base::DataVector{double(0), genClenshawCurtisNodes(2)[0],
                                     genClenshawCurtisNodes(2)[1], double(1)};

  tg = TensorGrid({GPCntType(5), GPCntType(4)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (0, 2)
  mi = LvlMI({0, 2});

  coeff = 1;

  nodesDim1 = sgpp::base::DataVector{genEquidistantNodes(1)[0]};
  nodesDim2 =
      sgpp::base::DataVector{double(0), genClenshawCurtisNodes(3)[0], genClenshawCurtisNodes(3)[1],
                             genClenshawCurtisNodes(3)[2], double(1)};

  tg = TensorGrid({GPCntType(1), GPCntType(5)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (1, 0)
  mi = LvlMI({1, 0});

  coeff = -1;

  nodesDim1 =
      sgpp::base::DataVector{double(0), genEquidistantNodes(3)[0], genEquidistantNodes(3)[1],
                             genEquidistantNodes(3)[2], double(1)};
  nodesDim2 = sgpp::base::DataVector{genClenshawCurtisNodes(1)[0]};

  tg = TensorGrid({GPCntType(5), GPCntType(1)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (0, 1)
  mi = LvlMI({0, 1});

  coeff = -1;

  nodesDim1 = sgpp::base::DataVector{genEquidistantNodes(1)[0]};
  nodesDim2 = sgpp::base::DataVector{double(0), genClenshawCurtisNodes(2)[0],
                                     genClenshawCurtisNodes(2)[1], double(1)};

  tg = TensorGrid({GPCntType(1), GPCntType(4)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

// BOOST_AUTO_TEST_CASE(fullsg_generation_consistency_2d_level2) {
//   // Parameter
//   const size_t nDim = 2;
//   const LvlType maxLevel = 2;

//   FullSGGenInstr genInstr(maxLevel, nDim);

//   // Explizit Node- / Lvl->GP-Funktionen setzen (sicherstellen, dass wir wissen, was verwendet
//   wird) for (size_t d = 0; d < nDim; ++d) {
//     genInstr.setNodeGenFuncForDim(genEquidistantNodes, d);
//     genInstr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, d);
//   }

//   // Erzeuge SparseGrid über Instruktion
//   SparseGrid sg(genInstr);

//   // Hole erwartete Multiindizes + Koeffizienten direkt aus der Instruktion
//   const std::pair<LvlMIVec, std::vector<CTCoeffType>> genWithCoeff =
//   genInstr.genMIVecWithCoeff(); const LvlMIVec& expectedMIVec = genWithCoeff.first; const
//   std::vector<CTCoeffType>& expectedCoeffs = genWithCoeff.second;

//   // Grundprüfungen
//   BOOST_REQUIRE_EQUAL(sg.nDim(), nDim);
//   BOOST_REQUIRE_EQUAL(sg.nTG(), expectedMIVec.nMI());
//   BOOST_REQUIRE_EQUAL(expectedCoeffs.size(), expectedMIVec.nMI());

//   // Für jedes TensorGrid: Konsistenzprüfungen
//   for (size_t tgIdx = 0; tgIdx < sg.nTG(); ++tgIdx) {
//     const TensorGridCTData& tg = sg.getTensorGrid(tgIdx);

//     // Multiindex und Koeffizient sollten exakt mit genInstr.genMIVecWithCoeff() übereinstimmen
//     BOOST_CHECK(tg.mi == expectedMIVec[tgIdx]);
//     BOOST_CHECK_EQUAL(tg.coefficient, expectedCoeffs[tgIdx]);

//     // getTensorGrid(LvlMI) muss dieselben Daten liefern
//     const auto tg_by_mi = sg.getTensorGrid(tg.mi);
//     BOOST_REQUIRE(tg_by_mi != sg.end());
//     BOOST_CHECK((*tg_by_mi).mi == tg.mi);
//     BOOST_CHECK_EQUAL((*tg_by_mi).coefficient, tg.coefficient);

//     // TensorGrid -> Anzahl Punkte (nGP) muss gleich Produkt der per-dim Knotenzahlen sein
//     std::vector<GPCntType> nGPPerDim(nDim);
//     for (size_t d = 0; d < nDim; ++d) {
//       const LvlType lvl = tg.mi[d];
//       nGPPerDim[d] = genInstr.getLvl2GPCntFuncForDim(d)(lvl) + 2;
//       BOOST_CHECK_GE(nGPPerDim[d], static_cast<GPCntType>(1));  // mindestens 1 Knoten
//     }
//     const unsigned long long expectedNP = prod_u64(nGPPerDim);
//     BOOST_CHECK_MESSAGE(tg.tensorGrid.nGP() == expectedNP, "TG idx '" << tgIdx << "'.");

//     // Erzeuge die Knoten pro Dimension via NodeGenFunc und vergleiche alle Grid-Punkte
//     std::vector<sgpp::base::DataVector> perDimNodes(nDim);
//     for (size_t d = 0; d < nDim; ++d) {
//       perDimNodes[d] = genInstr.getNodeGenFuncForDim(d)(nGPPerDim[d]);
//       BOOST_REQUIRE_EQUAL(perDimNodes[d].size(), static_cast<size_t>(nGPPerDim[d]));
//     }

//     // Über alle globalen Gitterpunkte iterieren und jede Koordinate prüfen
//     const size_t nGP = static_cast<size_t>(tg.tensorGrid.nGP());
//     for (size_t idx = 0; idx < nGP; ++idx) {
//       // Zerlege idx in per-dim Indizes (letzte Dim schnellst-var.)
//       std::vector<size_t> perIdx = decomposeLinearIndex(idx, nGPPerDim);
//       BOOST_REQUIRE_EQUAL(perIdx.size(), nDim);

//       for (size_t d = 0; d < nDim; ++d) {
//         const double expectedVal = perDimNodes[d][perIdx[d]];
//         // Zugriff über operator()(idx, dim)
//         const double gotVal = tg.tensorGrid(static_cast<size_t>(idx), d);
//         // numerischer Vergleich
//         BOOST_CHECK_CLOSE(gotVal, expectedVal, 1e-12);
//         // Werte müssen in [0,1] liegen (SparseGrid über [0,1]^d)
//         BOOST_CHECK_GE(gotVal, 0.0);
//         BOOST_CHECK_LE(gotVal, 1.0);
//       }
//     }
//   }
// }

// BOOST_AUTO_TEST_CASE(fullsg_generation_consistency_3d_level1) {
//   // kleinerer Test in 3D, maxLevel=1 (stresst andere D)
//   const size_t nDim = 3;
//   const LvlType maxLevel = 1;
//   FullSGGenInstr genInstr(nDim, maxLevel);

//   // Verwende Clenshaw-Curtis (alias genClenshawCurtisNodes) in Dim 0 und Equidistant in anderen
//   genInstr.setNodeGenFuncForDim(&genClenshawCurtisNodes, 0);
//   for (size_t d = 1; d < nDim; ++d) {
//     genInstr.setNodeGenFuncForDim(&genEquidistantNodes, d);
//   }
//   // Lvl->GPCnt: benutze doubling in allen Dimensionen
//   for (size_t d = 0; d < nDim; ++d) {
//     genInstr.setLvl2GPCntFuncForDim(&doublingLvl2GPCntFunction, d);
//   }

//   SparseGrid sg(genInstr);
//   auto [expectedMIVec, expectedCoeffs] = genInstr.genMIVecWithCoeff();

//   BOOST_REQUIRE_EQUAL(sg.nDim(), nDim);
//   BOOST_REQUIRE_EQUAL(sg.nTG(), expectedMIVec.nMI());
//   BOOST_REQUIRE_EQUAL(expectedCoeffs.size(), expectedMIVec.nMI());

//   for (size_t tgIdx = 0; tgIdx < sg.nTG(); ++tgIdx) {
//     const TensorGridCTData& tg = sg.getTensorGrid(tgIdx);
//     BOOST_CHECK(tg.mi == expectedMIVec[tgIdx]);
//     BOOST_CHECK_EQUAL(tg.coefficient, expectedCoeffs[tgIdx]);

//     // Berechne erwartete Knotenzahlen und Produkt
//     std::vector<GPCntType> nGPPerDim(nDim);
//     for (size_t d = 0; d < nDim; ++d) {
//       const LvlType lvl = tg.mi[d];
//       nGPPerDim[d] = genInstr.getLvl2GPCntFuncForDim(d)(lvl);
//       BOOST_CHECK_GE(nGPPerDim[d], static_cast<GPCntType>(1));
//     }
//     const unsigned long long expectedNP = prod_u64(nGPPerDim);
//     BOOST_CHECK_EQUAL(tg.tensorGrid.nGP(), expectedNP);

//     // Erzeuge Knoten per dim aus NodeGenFunc
//     std::vector<sgpp::base::DataVector> perDimNodes(nDim);
//     for (size_t d = 0; d < nDim; ++d) {
//       perDimNodes[d] = genInstr.getNodeGenFuncForDim(d)(nGPPerDim[d]);
//       BOOST_REQUIRE_EQUAL(perDimNodes[d].size(), static_cast<size_t>(nGPPerDim[d]));
//     }

//     // Überprüfe alle Punkte
//     const size_t nGP = static_cast<size_t>(tg.tensorGrid.nGP());
//     for (size_t idx = 0; idx < nGP; ++idx) {
//       std::vector<size_t> perIdx = decomposeLinearIndex(idx, nGPPerDim);
//       for (size_t d = 0; d < nDim; ++d) {
//         const double expectedVal = perDimNodes[d][perIdx[d]];
//         const double gotVal = tg.tensorGrid(idx, d);
//         BOOST_CHECK_CLOSE(gotVal, expectedVal, 1e-12);
//         BOOST_CHECK_GE(gotVal, 0.0);
//         BOOST_CHECK_LE(gotVal, 1.0);
//       }
//     }
//   }
// }

// BOOST_AUTO_TEST_CASE(Random1D) {
//   randGen.setSeed();
//   BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
//     const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(1000));

//     const FullSGGenInstr instr(maxLvl, 1);
//     const LvlMIVec result = instr.genMIVec();

//     BOOST_CHECK_MESSAGE(result.nDim() == 1, "Seed: " << randGen.getSeed());
//     BOOST_CHECK_MESSAGE(result.nMI() == 1, "Seed: " << randGen.getSeed());
//     BOOST_CHECK_MESSAGE(result(0, 0) == maxLvl, "Seed: " << randGen.getSeed());
//   }
// }

BOOST_AUTO_TEST_SUITE_END()