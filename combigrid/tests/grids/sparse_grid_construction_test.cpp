// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <algorithm>
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
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/tools/multiindex/multiindex_utilities.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();
static NodeGenFunc* equidistantNodeGenFunc = getEquidistantNodeGenFunc();
static NodeGenFunc* clenshawCurtisNodeGenFunc = getClenshawCurtisNodeGenFunc();

namespace {

sgpp::base::DataVector prod2DGPs(const sgpp::base::DataVector& nodes1,
                                 const sgpp::base::DataVector& nodes2) {
  sgpp::base::DataVector result(nodes1.size() + nodes2.size());

  std::copy(nodes1.begin(), nodes1.end(), result.begin());
  std::copy(nodes2.begin(), nodes2.end(), result.begin() + nodes1.size());

  return result;
}

sgpp::base::DataVector prod3DGPs(const sgpp::base::DataVector& nodes1,
                                 const sgpp::base::DataVector& nodes2,
                                 const sgpp::base::DataVector& nodes3) {
  sgpp::base::DataVector result(nodes1.size() + nodes2.size() + nodes3.size());

  std::copy(nodes1.begin(), nodes1.end(), result.begin());
  std::copy(nodes2.begin(), nodes2.end(), result.begin() + nodes1.size());
  std::copy(nodes3.begin(), nodes3.end(), result.begin() + nodes1.size() + nodes2.size());

  return result;
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
    genInstr.setNodeGenFuncs({equidistantNodeGenFunc});
    genInstr.setLvl2GPCntFuncs({linearLvl2GPCntFunction});

    // Expected
    SparseGrid expected(1, 1);

    const LvlMI mi({maxLvl});

    const CTCoeffType coeff = 1;

    const sgpp::base::DataVector nodes =
        equidistantNodeGenFunc->genNodes(linearLvl2GPCntFunction(maxLvl) + 2, true);
    // std::vector<sgpp::base::DataVector> gps(nodes.size() + 2); // TODO: Delete
    // gps[0] = sgpp::base::DataVector{0};
    // for (size_t i = 0; i < nodes.size(); i++) {
    //   gps[i + 1] = sgpp::base::DataVector{nodes[i]};
    // }
    // gps[gps.size() - 1] = sgpp::base::DataVector{1};
    const TensorGrid tg({(GPCntType)(nodes.size() + 2)}, nodes);

    expected.setTensorGrid(0, TensorGridCTData{mi, coeff, tg});

    // Result
    const SparseGrid result(genInstr);

    // Comparison
    BOOST_CHECK(result == expected);
  }
}

BOOST_AUTO_TEST_CASE(Complex_Full_2D_SG) {
  FullSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFuncs({equidistantNodeGenFunc, clenshawCurtisNodeGenFunc});
  genInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryIndexOffset(1);

  // Expected
  SparseGrid expected(2);

  // Tensor grid (2, 0)
  LvlMI mi({2, 0});

  CTCoeffType coeff = 1;

  sgpp::base::DataVector nodesDim1{static_cast<double>(0),
                                   equidistantNodeGenFunc->genNodes(7, false)[0],
                                   equidistantNodeGenFunc->genNodes(7, false)[1],
                                   equidistantNodeGenFunc->genNodes(7, false)[2],
                                   equidistantNodeGenFunc->genNodes(7, false)[3],
                                   equidistantNodeGenFunc->genNodes(7, false)[4],
                                   equidistantNodeGenFunc->genNodes(7, false)[5],
                                   equidistantNodeGenFunc->genNodes(7, false)[6],
                                   static_cast<double>(1)};
  sgpp::base::DataVector nodesDim2{clenshawCurtisNodeGenFunc->genNodes(1, false)[0]};

  TensorGrid tg({static_cast<GPCntType>(9), static_cast<GPCntType>(1)},
                prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (1, 1)
  mi = LvlMI({1, 1});

  coeff = 1;

  nodesDim1 =
      sgpp::base::DataVector{static_cast<double>(0), equidistantNodeGenFunc->genNodes(3, false)[0],
                             equidistantNodeGenFunc->genNodes(3, false)[1],
                             equidistantNodeGenFunc->genNodes(3, false)[2], static_cast<double>(1)};
  nodesDim2 = sgpp::base::DataVector{
      static_cast<double>(0), clenshawCurtisNodeGenFunc->genNodes(2, false)[0],
      clenshawCurtisNodeGenFunc->genNodes(2, false)[1], static_cast<double>(1)};

  tg = TensorGrid({GPCntType(5), GPCntType(4)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (0, 2)
  mi = LvlMI({0, 2});

  coeff = 1;

  nodesDim1 = sgpp::base::DataVector{equidistantNodeGenFunc->genNodes(1, false)[0]};
  nodesDim2 = sgpp::base::DataVector{
      static_cast<double>(0), clenshawCurtisNodeGenFunc->genNodes(3, false)[0],
      clenshawCurtisNodeGenFunc->genNodes(3, false)[1],
      clenshawCurtisNodeGenFunc->genNodes(3, false)[2], static_cast<double>(1)};

  tg = TensorGrid({GPCntType(1), GPCntType(5)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (1, 0)
  mi = LvlMI({1, 0});

  coeff = -1;

  nodesDim1 =
      sgpp::base::DataVector{static_cast<double>(0), equidistantNodeGenFunc->genNodes(3, false)[0],
                             equidistantNodeGenFunc->genNodes(3, false)[1],
                             equidistantNodeGenFunc->genNodes(3, false)[2], static_cast<double>(1)};
  nodesDim2 = sgpp::base::DataVector{clenshawCurtisNodeGenFunc->genNodes(1, false)[0]};

  tg = TensorGrid({GPCntType(5), GPCntType(1)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Tensor grid (0, 1)
  mi = LvlMI({0, 1});

  coeff = -1;

  nodesDim1 = sgpp::base::DataVector{equidistantNodeGenFunc->genNodes(1, false)[0]};
  nodesDim2 = sgpp::base::DataVector{
      static_cast<double>(0), clenshawCurtisNodeGenFunc->genNodes(2, false)[0],
      clenshawCurtisNodeGenFunc->genNodes(2, false)[1], static_cast<double>(1)};

  tg = TensorGrid({GPCntType(1), GPCntType(4)}, prod2DGPs(nodesDim1, nodesDim2));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Simple_Full_3D_Level0) {
  // Generation instruction
  const LvlType maxLvl = 0;
  FullSGGenInstr genInstr(maxLvl, 3);

  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, equidistantNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, linearLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryIndexOffset(1);

  // Expected
  SparseGrid expected(3);

  // Tensorgrid (0,0,0)
  const LvlMI mi({0, 0, 0});
  const CTCoeffType coeff = 1;

  const sgpp::base::DataVector nodesDim1{equidistantNodeGenFunc->genNodes(1, false)[0]};
  const sgpp::base::DataVector nodesDim2{equidistantNodeGenFunc->genNodes(1, false)[0]};
  const sgpp::base::DataVector nodesDim3{equidistantNodeGenFunc->genNodes(1, false)[0]};

  const TensorGrid tg({GPCntType(1), GPCntType(1), GPCntType(1)},
                      prod3DGPs(nodesDim1, nodesDim2, nodesDim3));
  expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Complex_Full_3D_SG) {
  // Generation instruction
  const LvlType maxLvl = 2;
  const size_t dim = 3;

  FullSGGenInstr genInstr(maxLvl, dim);

  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {doublingLvl2GPCntFunction, linearLvl2GPCntFunction, doublingLvl2GPCntFunction});
  genInstr.setBoundaryIndexOffset(1);

  // Expected
  SparseGrid expected(dim);

  // For all multiindices l = (l1,l2,l3) with l1+l2+l3 <= maxLvl
  for (LvlType l1 = 0; l1 <= maxLvl; ++l1) {
    for (LvlType l2 = 0; l2 <= maxLvl; ++l2) {
      for (LvlType l3 = 0; l3 <= maxLvl; ++l3) {
        if (l1 + l2 + l3 > maxLvl) continue;
        // Coefficient
        const LvlType sum = l1 + l2 + l3;

        const CTCoeffType sign = ((maxLvl - sum) % 2 == 0) ? 1 : -1;
        const CTCoeffType binom = tools::binomial(static_cast<CTCoeffType>(dim) - 1,
                                                  static_cast<CTCoeffType>(maxLvl - sum));
        const CTCoeffType coeff = sign * binom;

        // Nodes
        sgpp::base::DataVector nodesDim1;
        sgpp::base::DataVector nodesDim2;
        sgpp::base::DataVector nodesDim3;

        // Dimension 1: genEquidistantNodes, doublingLvl2GPCntFunction
        {
          const GPCntType gpCnt = doublingLvl2GPCntFunction(l1);
          if (l1 > 0) {
            nodesDim1.resize(gpCnt + 2);
            nodesDim1[0] = 0;
            sgpp::base::DataVector interior = equidistantNodeGenFunc->genNodes(gpCnt, false);
            for (size_t i = 0; i < gpCnt; ++i) nodesDim1[i + 1] = interior[i];
            nodesDim1[nodesDim1.size() - 1] = 1;
          } else {
            nodesDim1 = sgpp::base::DataVector{equidistantNodeGenFunc->genNodes(1, false)[0]};
          }
        }

        // Dimension 2: genClenshawCurtisNodes, linearLvl2GPCntFunction
        {
          const GPCntType gpCnt = linearLvl2GPCntFunction(l2);
          if (l2 > 0) {
            nodesDim2.resize(gpCnt + 2);
            nodesDim2[0] = 0;
            sgpp::base::DataVector interior = clenshawCurtisNodeGenFunc->genNodes(gpCnt, false);
            for (size_t i = 0; i < gpCnt; ++i) nodesDim2[i + 1] = interior[i];
            nodesDim2[nodesDim2.size() - 1] = 1;
          } else {
            nodesDim2 = sgpp::base::DataVector{clenshawCurtisNodeGenFunc->genNodes(1, false)[0]};
          }
        }

        // Dimension 3: genEquidistantNodes, doublingLvl2GPCntFunction
        {
          const GPCntType gpCnt = doublingLvl2GPCntFunction(l3);
          if (l3 > 0) {
            nodesDim3.resize(gpCnt + 2);
            nodesDim3[0] = 0;
            sgpp::base::DataVector interior = equidistantNodeGenFunc->genNodes(gpCnt, false);
            for (size_t i = 0; i < gpCnt; ++i) nodesDim3[i + 1] = interior[i];
            nodesDim3[nodesDim3.size() - 1] = 1;
          } else {
            nodesDim3 = sgpp::base::DataVector{equidistantNodeGenFunc->genNodes(1, false)[0]};
          }
        }

        GPMI gpcnts;
        gpcnts.push_back(static_cast<GPCntType>(nodesDim1.size()));
        gpcnts.push_back(static_cast<GPCntType>(nodesDim2.size()));
        gpcnts.push_back(static_cast<GPCntType>(nodesDim3.size()));

        TensorGrid tg(gpcnts, prod3DGPs(nodesDim1, nodesDim2, nodesDim3));
        LvlMI mi({l1, l2, l3});
        expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});
      }
    }
  }

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

/*
Construct a random small 3D full sparse-grid instruction (random max level)
and build the expected SparseGrid by iterating all valid multi-indices l
with l1 + l2 + l3 <= maxLvl. This mirrors the Smolyak combination logic
used in the explicit Complex_Full_3D_SG test.
*/
BOOST_AUTO_TEST_CASE(Random_Full_3D_SG) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    // Generation instruction
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(3));  // 0..2
    const size_t dim = 3;

    FullSGGenInstr genInstr(maxLvl, dim);
    genInstr.setNodeGenFuncs(
        {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
    genInstr.setLvl2GPCntFuncs(
        {doublingLvl2GPCntFunction, linearLvl2GPCntFunction, doublingLvl2GPCntFunction});
    genInstr.setBoundaryIndexOffset(1);

    // Expected
    SparseGrid expected(dim);

    for (LvlType l1 = 0; l1 <= maxLvl; ++l1) {
      for (LvlType l2 = 0; l2 <= maxLvl; ++l2) {
        for (LvlType l3 = 0; l3 <= maxLvl; ++l3) {
          if (l1 + l2 + l3 > maxLvl) continue;

          const LvlType sum = l1 + l2 + l3;
          const CTCoeffType sign = ((maxLvl - sum) % 2 == 0) ? 1 : -1;
          const CTCoeffType binom = tools::binomial(static_cast<CTCoeffType>(dim) - 1,
                                                    static_cast<CTCoeffType>(maxLvl - sum));
          CTCoeffType coeff = sign * binom;

          // Build per-dimension node vectors consistent with boundary handling
          sgpp::base::DataVector nodesDim1;
          sgpp::base::DataVector nodesDim2;
          sgpp::base::DataVector nodesDim3;

          // Dim 1: equidistant + doubling gp-count function
          {
            const GPCntType gpCnt = doublingLvl2GPCntFunction(l1);
            if (l1 > 0) {
              nodesDim1.resize(gpCnt + 2);
              nodesDim1[0] = 0;
              sgpp::base::DataVector interior = equidistantNodeGenFunc->genNodes(gpCnt, false);
              for (size_t i = 0; i < gpCnt; ++i) nodesDim1[i + 1] = interior[i];
              nodesDim1[nodesDim1.size() - 1] = 1;
            } else {
              nodesDim1 = sgpp::base::DataVector{equidistantNodeGenFunc->genNodes(1, false)[0]};
            }
          }

          // Dim 2: clenshaw-curtis + linear gp-count
          {
            const GPCntType gpCnt = linearLvl2GPCntFunction(l2);
            if (l2 > 0) {
              nodesDim2.resize(gpCnt + 2);
              nodesDim2[0] = 0;
              sgpp::base::DataVector interior = clenshawCurtisNodeGenFunc->genNodes(gpCnt, false);
              for (size_t i = 0; i < gpCnt; ++i) nodesDim2[i + 1] = interior[i];
              nodesDim2[nodesDim2.size() - 1] = double(1);
            } else {
              nodesDim2 = sgpp::base::DataVector{clenshawCurtisNodeGenFunc->genNodes(1, false)[0]};
            }
          }

          // Dim 3: equidistant + doubling gp-count
          {
            const GPCntType gpCnt = doublingLvl2GPCntFunction(l3);
            if (l3 > 0) {
              nodesDim3.resize(gpCnt + 2);
              nodesDim3[0] = 0;
              sgpp::base::DataVector interior = equidistantNodeGenFunc->genNodes(gpCnt, false);
              for (size_t i = 0; i < gpCnt; ++i) nodesDim3[i + 1] = interior[i];
              nodesDim3[nodesDim3.size() - 1] = 1;
            } else {
              nodesDim3 = sgpp::base::DataVector{equidistantNodeGenFunc->genNodes(1, false)[0]};
            }
          }

          GPMI gpcnts;
          gpcnts.push_back(static_cast<GPCntType>(nodesDim1.size()));
          gpcnts.push_back(static_cast<GPCntType>(nodesDim2.size()));
          gpcnts.push_back(static_cast<GPCntType>(nodesDim3.size()));

          TensorGrid tg(gpcnts, prod3DGPs(nodesDim1, nodesDim2, nodesDim3));
          LvlMI mi({l1, l2, l3});
          expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});
        }
      }
    }

    // Construct result via SparseGrid constructor
    const SparseGrid result(genInstr);

    // Compare
    BOOST_CHECK(result == expected);
  }
}

/*
  Mixed_NodeFunctions_3D
  ----------------------
  Deterministic 3D test that uses mixed node-generation strategies
  (Clenshaw-Curtis in some dims, equidistant in others) and verifies
  explicit expected construction for maxLvl = 2.
*/
BOOST_AUTO_TEST_CASE(Mixed_NodeFunctions_3D) {
  // Generation instruction
  const LvlType maxLvl = 2;
  const size_t dim = 3;

  FullSGGenInstr genInstr(maxLvl, dim);
  genInstr.setNodeGenFuncs(
      {clenshawCurtisNodeGenFunc, equidistantNodeGenFunc, clenshawCurtisNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryIndexOffset(1);

  // Expected
  SparseGrid expected(dim);

  for (LvlType l1 = 0; l1 <= maxLvl; ++l1) {
    for (LvlType l2 = 0; l2 <= maxLvl; ++l2) {
      for (LvlType l3 = 0; l3 <= maxLvl; ++l3) {
        if (l1 + l2 + l3 > maxLvl) continue;

        const LvlType sum = l1 + l2 + l3;
        const CTCoeffType sign = ((maxLvl - sum) % 2 == 0) ? 1 : -1;
        const CTCoeffType binom = tools::binomial(static_cast<CTCoeffType>(dim) - 1,
                                                  static_cast<CTCoeffType>(maxLvl - sum));
        CTCoeffType coeff = sign * binom;

        sgpp::base::DataVector nodesDim1;
        sgpp::base::DataVector nodesDim2;
        sgpp::base::DataVector nodesDim3;

        // Dim 1: clenshaw-curtis + linear
        {
          const GPCntType gpCnt = linearLvl2GPCntFunction(l1);
          if (l1 > 0) {
            nodesDim1.resize(gpCnt + 2);
            nodesDim1[0] = 0;
            sgpp::base::DataVector interior = clenshawCurtisNodeGenFunc->genNodes(gpCnt, false);
            for (size_t i = 0; i < gpCnt; ++i) nodesDim1[i + 1] = interior[i];
            nodesDim1[nodesDim1.size() - 1] = 1;
          } else {
            nodesDim1 = sgpp::base::DataVector{clenshawCurtisNodeGenFunc->genNodes(1, false)[0]};
          }
        }

        // Dim 2: equidistant + doubling
        {
          const GPCntType gpCnt = doublingLvl2GPCntFunction(l2);
          if (l2 > 0) {
            nodesDim2.resize(gpCnt + 2);
            nodesDim2[0] = 0;
            sgpp::base::DataVector interior = equidistantNodeGenFunc->genNodes(gpCnt, false);
            for (size_t i = 0; i < gpCnt; ++i) nodesDim2[i + 1] = interior[i];
            nodesDim2[nodesDim2.size() - 1] = 1;
          } else {
            nodesDim2 = sgpp::base::DataVector{equidistantNodeGenFunc->genNodes(1, false)[0]};
          }
        }

        // Dim 3: clenshaw-curtis + linear
        {
          const GPCntType gpCnt = linearLvl2GPCntFunction(l3);
          if (l3 > 0) {
            nodesDim3.resize(gpCnt + 2);
            nodesDim3[0] = 0;
            sgpp::base::DataVector interior = clenshawCurtisNodeGenFunc->genNodes(gpCnt, false);
            for (size_t i = 0; i < gpCnt; ++i) nodesDim3[i + 1] = interior[i];
            nodesDim3[nodesDim3.size() - 1] = 1;
          } else {
            nodesDim3 = sgpp::base::DataVector{clenshawCurtisNodeGenFunc->genNodes(1, false)[0]};
          }
        }

        GPMI gpcnts;
        gpcnts.push_back(static_cast<GPCntType>(nodesDim1.size()));
        gpcnts.push_back(static_cast<GPCntType>(nodesDim2.size()));
        gpcnts.push_back(static_cast<GPCntType>(nodesDim3.size()));

        TensorGrid tg(gpcnts, prod3DGPs(nodesDim1, nodesDim2, nodesDim3));
        LvlMI mi({l1, l2, l3});
        expected.addTensorGrid(TensorGridCTData{mi, coeff, tg});
      }
    }
  }

  const SparseGrid result(genInstr);
  BOOST_CHECK(result == expected);
}

/*
  Verify behavior when boundaryIndexOffset = 0 (no explicit 0/1 boundary points
  are added) vs boundaryIndexOffset = 1 (0 and 1 are added for levels > 0).
  This test constructs two gen-instructions and checks that the produced
  tensor node counts reflect the boundary policy.
*/
BOOST_AUTO_TEST_CASE(BoundaryOffsetBehavior) {
  const LvlType maxLvl = 1;
  const size_t dim = 3;

  // Generation instruction for boundaryIndexOffset = 0
  FullSGGenInstr genInstrNoBoundary(maxLvl, dim);
  genInstrNoBoundary.setNodeGenFuncs(
      {equidistantNodeGenFunc, equidistantNodeGenFunc, equidistantNodeGenFunc});
  genInstrNoBoundary.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, linearLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstrNoBoundary.setBoundaryIndexOffset(1);

  const SparseGrid resultNoBoundary(genInstrNoBoundary);

  // Generation instruction for boundaryIndexOffset = 1
  FullSGGenInstr genInstrWithBoundary(maxLvl, dim);
  genInstrWithBoundary.setNodeGenFuncs(
      {equidistantNodeGenFunc, equidistantNodeGenFunc, equidistantNodeGenFunc});
  genInstrWithBoundary.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, linearLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstrWithBoundary.setBoundaryIndexOffset(0);

  const SparseGrid resultWithBoundary(genInstrWithBoundary);

  // Inspect one representative tensor grid: multi-index (1,0,0) should differ in GPCnt for dim2 &
  // dim3
  LvlMI mi({1, 0, 0});
  const auto tgNoB = resultNoBoundary.getTensorGrid(mi);
  const auto tgWithB = resultWithBoundary.getTensorGrid(mi);

  BOOST_REQUIRE(tgNoB != resultNoBoundary.end());
  BOOST_REQUIRE(tgWithB != resultWithBoundary.end());

  const GPMI gpcntsNo = tgNoB->tensorGrid.getGPCntPerDim();
  const GPMI gpcntsWith = tgWithB->tensorGrid.getGPCntPerDim();

  BOOST_CHECK(gpcntsWith[0] == gpcntsNo[0]);
  BOOST_CHECK(gpcntsWith[1] == gpcntsNo[1] + 2);
  BOOST_CHECK(gpcntsWith[2] == gpcntsNo[2] + 2);
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

//     // Multiindex und Koeffizient sollten exakt mit genInstr.genMIVecWithCoeff übereinstimmen
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