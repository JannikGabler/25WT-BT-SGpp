// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_base_test.cpp
 * @brief Boost.Test cases for the variant-independent core of all sparse grids
 * (@c SparseGridBase): construction, element access, modification, cached meta
 * values, iteration and equality.
 *
 * The constructors and modifiers of @c SparseGridBase are protected, so the tests use the
 * minimal subclass @c TestSG that makes them public and adds nothing else.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

/// Concrete sparse grid that only makes the protected members of @c SparseGridBase public.
class TestSG : public SparseGridBase {
 public:
  explicit TestSG(const size_t nDim) : SparseGridBase(nDim) {}
  TestSG(const size_t nDim, const size_t nTG) : SparseGridBase(nDim, nTG) {}

  using SparseGridBase::addTensorGrid;
  using SparseGridBase::reserve;
  using SparseGridBase::resize;
  using SparseGridBase::setTensorGrid;
  using SparseGridBase::setTensorGrids;
};

/**
 * Builds a tensor-grid summand with @p gpCntPerDim grid points. The node values are
 * meaningless for SparseGridBase; @p shift only allows building grids that differ in
 * their nodes alone.
 */
TensorGridCTData makeTGData(const LvlMI& mi, const CTCoeffType coeff, const GPMI& gpCntPerDim,
                            const double shift = 0.0) {
  sgpp::base::DataVector nodes(gpCntPerDim.sumOfElems<size_t>());
  size_t nodeIdx = 0;

  for (const GPCntType gpCnt : gpCntPerDim) {
    for (GPCntType i = 0; i < gpCnt; i++) {
      nodes[nodeIdx++] = (i + 1.0) / (gpCnt + 1.0) + shift;
    }
  }

  return {mi, coeff, TensorGrid(gpCntPerDim, nodes)};
}

/// Expected value of @c getMaxTGGPCnt(), recomputed from scratch.
size_t bruteForceMaxTGGPCnt(const SparseGridBase& sg) {
  size_t maximum = 0;
  for (const TensorGridCTData& tgData : sg) maximum = std::max(maximum, tgData.tensorGrid.nGP());
  return maximum;
}

/// Expected value of @c getMaxTGSumOverGPCntsPerDim(), recomputed from scratch.
size_t bruteForceMaxTGSumOverGPCntsPerDim(const SparseGridBase& sg) {
  size_t maximum = 0;
  for (const TensorGridCTData& tgData : sg) {
    maximum = std::max(maximum, tgData.tensorGrid.getGPCntPerDim().sumOfElems<size_t>());
  }
  return maximum;
}

void checkMetaValues(const SparseGridBase& sg, const size_t expectedMaxTGGPCnt,
                     const size_t expectedMaxTGSumOverGPCntsPerDim) {
  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), expectedMaxTGGPCnt);
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), expectedMaxTGSumOverGPCntsPerDim);
}

// 2D tensor grids whose maximum #GP and maximum per-dim GP sum are attained by
// different grids, so that both meta values are tested independently.
//                                                                   nGP  sum
const TensorGridCTData tgA = makeTGData({0, 3}, 1, {1, 9});    //    9   10
const TensorGridCTData tgB = makeTGData({1, 1}, -1, {3, 3});   //    9    6
const TensorGridCTData tgC = makeTGData({2, 0}, 2, {4, 4});    //   16    8
const TensorGridCTData tgD = makeTGData({0, 1}, -2, {1, 2});   //    2    3
const TensorGridCTData tgE = makeTGData({0, 4}, 1, {1, 20});   //   20   21

}  // namespace

BOOST_AUTO_TEST_SUITE(SparseGridBase_Construction)

BOOST_AUTO_TEST_CASE(Empty) {
  for (const size_t nDim : std::vector<size_t>{0, 1, 3}) {
    BOOST_TEST_CONTEXT("nDim: " << nDim) {
      const TestSG sg(nDim);

      BOOST_CHECK_EQUAL(sg.nDim(), nDim);
      BOOST_CHECK_EQUAL(sg.nTG(), 0u);
      BOOST_CHECK(sg.getTensorGrids().empty());
      BOOST_CHECK(sg.begin() == sg.end());
      BOOST_CHECK(sg.rbegin() == sg.rend());
      checkMetaValues(sg, 0, 0);
    }
  }
}

BOOST_AUTO_TEST_CASE(Preallocated_Slots) {
  const TestSG sg(2, 4);

  BOOST_CHECK_EQUAL(sg.nDim(), 2u);
  BOOST_CHECK_EQUAL(sg.nTG(), 4u);
  BOOST_CHECK_EQUAL(sg.getTensorGrids().size(), 4u);

  for (const TensorGridCTData& tgData : sg) {
    BOOST_CHECK_EQUAL(tgData.coefficient, 0);
    BOOST_CHECK_EQUAL(tgData.tensorGrid.nGP(), 0u);
  }

  checkMetaValues(sg, 0, 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGridBase_Modification)

BOOST_AUTO_TEST_CASE(Add_Tensor_Grid_Copy) {
  TestSG sg(2);
  const std::vector<TensorGridCTData> tgs{tgA, tgB, tgC};

  for (size_t i = 0; i < tgs.size(); i++) {
    sg.addTensorGrid(tgs[i]);
    BOOST_CHECK_EQUAL(sg.nTG(), i + 1);
  }

  for (size_t i = 0; i < tgs.size(); i++) {
    BOOST_CHECK(sg.getTensorGrid(i) == tgs[i]);
  }
}

BOOST_AUTO_TEST_CASE(Add_Tensor_Grid_Move) {
  TestSG sg(2);
  std::vector<TensorGridCTData> tgs{tgA, tgB, tgC};

  for (size_t i = 0; i < tgs.size(); i++) {
    sg.addTensorGrid(std::move(tgs[i]));
    BOOST_CHECK_EQUAL(sg.nTG(), i + 1);
  }

  BOOST_CHECK(sg.getTensorGrid(0) == tgA);
  BOOST_CHECK(sg.getTensorGrid(1) == tgB);
  BOOST_CHECK(sg.getTensorGrid(2) == tgC);
}

BOOST_AUTO_TEST_CASE(Add_Tensor_Grid_Updates_Meta_Values) {
  TestSG sg(2);

  sg.addTensorGrid(tgD);
  checkMetaValues(sg, 2, 3);
  sg.addTensorGrid(tgB);
  checkMetaValues(sg, 9, 6);
  sg.addTensorGrid(TensorGridCTData(tgA));
  checkMetaValues(sg, 9, 10);
  sg.addTensorGrid(TensorGridCTData(tgC));
  checkMetaValues(sg, 16, 10);
  sg.addTensorGrid(tgD);
  checkMetaValues(sg, 16, 10);
}

BOOST_AUTO_TEST_CASE(Set_Tensor_Grids_Replaces_Content) {
  TestSG sg(2);
  sg.addTensorGrid(tgE);

  sg.setTensorGrids({tgA, tgB, tgD});

  BOOST_REQUIRE_EQUAL(sg.nTG(), 3u);
  BOOST_CHECK(sg.getTensorGrid(0) == tgA);
  BOOST_CHECK(sg.getTensorGrid(1) == tgB);
  BOOST_CHECK(sg.getTensorGrid(2) == tgD);
  BOOST_CHECK(sg.getTensorGrid(LvlMI{0, 4}) == sg.end());
}

BOOST_AUTO_TEST_CASE(Set_Tensor_Grids_Recomputes_Meta_Values) {
  TestSG sg(2);

  sg.setTensorGrids({tgE, tgC});
  checkMetaValues(sg, 20, 21);

  // The meta values must also decrease if the previous maximum is gone.
  sg.setTensorGrids({tgB, tgD});
  checkMetaValues(sg, 9, 6);

  sg.setTensorGrids({});
  BOOST_CHECK_EQUAL(sg.nTG(), 0u);
  checkMetaValues(sg, 0, 0);
}

BOOST_AUTO_TEST_CASE(Set_Tensor_Grid_Copy) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB, tgC});

  sg.setTensorGrid(1, tgD);

  BOOST_REQUIRE_EQUAL(sg.nTG(), 3u);
  BOOST_CHECK(sg.getTensorGrid(0) == tgA);
  BOOST_CHECK(sg.getTensorGrid(1) == tgD);
  BOOST_CHECK(sg.getTensorGrid(2) == tgC);
}

BOOST_AUTO_TEST_CASE(Set_Tensor_Grid_Move) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB, tgC});

  sg.setTensorGrid(1, TensorGridCTData(tgD));

  BOOST_REQUIRE_EQUAL(sg.nTG(), 3u);
  BOOST_CHECK(sg.getTensorGrid(0) == tgA);
  BOOST_CHECK(sg.getTensorGrid(1) == tgD);
  BOOST_CHECK(sg.getTensorGrid(2) == tgC);
}

BOOST_AUTO_TEST_CASE(Set_Tensor_Grid_Updates_Meta_Values) {
  BOOST_TEST_CONTEXT("Replace a non-maximal tensor grid by a larger one") {
    TestSG sg(2);
    sg.setTensorGrids({tgA, tgB, tgD});
    sg.setTensorGrid(2, tgC);
    checkMetaValues(sg, 16, 10);
  }

  BOOST_TEST_CONTEXT("Replace a non-maximal tensor grid by a smaller one") {
    TestSG sg(2);
    sg.setTensorGrids({tgA, tgC, tgB});
    sg.setTensorGrid(2, tgD);
    checkMetaValues(sg, 16, 10);
  }

  BOOST_TEST_CONTEXT("Replace the maximal tensor grid by a smaller one") {
    TestSG sg(2);
    sg.setTensorGrids({tgC, tgA, tgD});
    sg.setTensorGrid(0, tgD);
    checkMetaValues(sg, 9, 10);
  }

  BOOST_TEST_CONTEXT("Replace the maximal tensor grid by a larger one") {
    TestSG sg(2);
    sg.setTensorGrids({tgA, tgD});
    sg.setTensorGrid(0, TensorGridCTData(tgE));
    checkMetaValues(sg, 20, 21);
  }

  BOOST_TEST_CONTEXT("Fill preallocated slots") {
    TestSG sg(2, 2);
    sg.setTensorGrid(0, tgA);
    sg.setTensorGrid(1, TensorGridCTData(tgC));
    checkMetaValues(sg, 16, 10);
  }
}

/*
Applies a random sequence of add / set operations and compares the cached meta
values with values recomputed from scratch after every step.
*/
BOOST_AUTO_TEST_CASE(Random_Modifications_Keep_Meta_Values_Consistent) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(3);
    TestSG sg(nDim);

    const auto randomTGData = [nDim]() {
      LvlMI mi(nDim);
      GPMI gpCntPerDim(nDim);
      for (size_t dim = 0; dim < nDim; dim++) {
        mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(5));
        gpCntPerDim[dim] = static_cast<GPCntType>(1 + randGen.getUniformIndexRN(8));
      }
      return makeTGData(mi, 1, gpCntPerDim);
    };

    for (size_t step = 0; step < 200; step++) {
      BOOST_TEST_CONTEXT("Step: " << step) {
        if (sg.nTG() == 0 || randGen.getUniformIndexRN(3) == 0) {
          sg.addTensorGrid(randomTGData());
        } else {
          sg.setTensorGrid(randGen.getUniformIndexRN(sg.nTG()), randomTGData());
        }

        // Once the cached values diverge, all later steps would fail too: stop at the first one.
        BOOST_REQUIRE_EQUAL(sg.getMaxTGGPCnt(), bruteForceMaxTGGPCnt(sg));
        BOOST_REQUIRE_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(),
                            bruteForceMaxTGSumOverGPCntsPerDim(sg));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Resize) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB});

  sg.resize(4);
  BOOST_REQUIRE_EQUAL(sg.nTG(), 4u);
  BOOST_CHECK(sg.getTensorGrid(0) == tgA);
  BOOST_CHECK(sg.getTensorGrid(1) == tgB);
  BOOST_CHECK_EQUAL(sg.getTensorGrid(3).tensorGrid.nGP(), 0u);

  sg.resize(1);
  BOOST_REQUIRE_EQUAL(sg.nTG(), 1u);
  BOOST_CHECK(sg.getTensorGrid(0) == tgA);
}

BOOST_AUTO_TEST_CASE(Reserve) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB});

  sg.reserve(100);

  BOOST_REQUIRE_EQUAL(sg.nTG(), 2u);
  BOOST_CHECK(sg.getTensorGrid(0) == tgA);
  BOOST_CHECK(sg.getTensorGrid(1) == tgB);
  checkMetaValues(sg, 9, 10);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGridBase_Access)

BOOST_AUTO_TEST_CASE(Get_Tensor_Grid_By_MI) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB, tgC});
  const TestSG& constSG = sg;

  const auto iterB = constSG.getTensorGrid(LvlMI{1, 1});
  BOOST_REQUIRE(iterB != constSG.end());
  BOOST_CHECK_EQUAL(std::distance(constSG.begin(), iterB), 1);
  BOOST_CHECK(*iterB == tgB);

  const auto iterC = constSG.getTensorGrid(LvlMI{2, 0});
  BOOST_REQUIRE(iterC != constSG.end());
  BOOST_CHECK_EQUAL(std::distance(constSG.begin(), iterC), 2);

  BOOST_CHECK(constSG.getTensorGrid(LvlMI{0, 2}) == constSG.end());
  BOOST_CHECK(constSG.getTensorGrid(LvlMI{3, 0}) == constSG.end());
}

BOOST_AUTO_TEST_CASE(Get_Tensor_Grid_By_MI_Empty) {
  const TestSG sg(2);
  BOOST_CHECK(sg.getTensorGrid(LvlMI{0, 0}) == sg.end());
}

BOOST_AUTO_TEST_CASE(Get_Tensor_Grids_Is_A_View) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB, tgC});

  const std::span<const TensorGridCTData> tgs = sg.getTensorGrids();

  BOOST_REQUIRE_EQUAL(tgs.size(), sg.nTG());
  for (size_t i = 0; i < tgs.size(); i++) {
    BOOST_CHECK(&tgs[i] == &sg.getTensorGrid(i));
  }
}

BOOST_AUTO_TEST_CASE(Iteration_Order) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB, tgC, tgD});
  const TestSG& constSG = sg;

  BOOST_CHECK_EQUAL(std::distance(sg.begin(), sg.end()), 4);
  BOOST_CHECK_EQUAL(std::distance(constSG.cbegin(), constSG.cend()), 4);
  BOOST_CHECK_EQUAL(std::distance(sg.rbegin(), sg.rend()), 4);
  BOOST_CHECK_EQUAL(std::distance(constSG.crbegin(), constSG.crend()), 4);

  size_t idx = 0;
  for (auto iter = constSG.begin(); iter != constSG.end(); ++iter, ++idx) {
    BOOST_CHECK(&*iter == &sg.getTensorGrid(idx));
  }

  idx = 0;
  for (auto iter = constSG.cbegin(); iter != constSG.cend(); ++iter, ++idx) {
    BOOST_CHECK(&*iter == &sg.getTensorGrid(idx));
  }

  idx = sg.nTG();
  for (auto iter = constSG.rbegin(); iter != constSG.rend(); ++iter) {
    BOOST_CHECK(&*iter == &sg.getTensorGrid(--idx));
  }

  idx = sg.nTG();
  for (auto iter = constSG.crbegin(); iter != constSG.crend(); ++iter) {
    BOOST_CHECK(&*iter == &sg.getTensorGrid(--idx));
  }

  idx = sg.nTG();
  for (auto iter = sg.rbegin(); iter != sg.rend(); ++iter) {
    BOOST_CHECK(&*iter == &sg.getTensorGrid(--idx));
  }
}

BOOST_AUTO_TEST_CASE(Mutable_Iterator_Writes_Through) {
  TestSG sg(2);
  sg.setTensorGrids({tgA, tgB});

  sg.begin()->coefficient = 7;
  sg.rbegin()->coefficient = -7;

  BOOST_CHECK_EQUAL(sg.getTensorGrid(0).coefficient, 7);
  BOOST_CHECK_EQUAL(sg.getTensorGrid(1).coefficient, -7);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGridBase_Equality)

BOOST_AUTO_TEST_CASE(Empty_Grids) {
  BOOST_CHECK(TestSG(0) == TestSG(0));
  BOOST_CHECK(TestSG(3) == TestSG(3));
  BOOST_CHECK(!(TestSG(2) == TestSG(3)));
}

BOOST_AUTO_TEST_CASE(Same_Content) {
  TestSG sg1(2);
  TestSG sg2(2);
  sg1.setTensorGrids({tgA, tgB, tgC});
  sg2.setTensorGrids({tgA, tgB, tgC});

  BOOST_CHECK(sg1 == sg1);
  BOOST_CHECK(sg1 == sg2);
  BOOST_CHECK(sg2 == sg1);
}

BOOST_AUTO_TEST_CASE(Order_Insensitive) {
  TestSG sg1(2);
  TestSG sg2(2);
  sg1.setTensorGrids({tgA, tgB, tgC});
  sg2.setTensorGrids({tgC, tgA, tgB});

  BOOST_CHECK(sg1 == sg2);
  BOOST_CHECK(sg2 == sg1);
}

BOOST_AUTO_TEST_CASE(Different_Number_Of_Tensor_Grids) {
  TestSG sg1(2);
  TestSG sg2(2);
  sg1.setTensorGrids({tgA, tgB, tgC});
  sg2.setTensorGrids({tgA, tgB});

  BOOST_CHECK(!(sg1 == sg2));
  BOOST_CHECK(!(sg2 == sg1));
}

BOOST_AUTO_TEST_CASE(Different_Tensor_Grid_Data) {
  const std::vector<std::pair<std::string, TensorGridCTData>> modifiedTGBs{
      {"multi-index", makeTGData({1, 2}, -1, {3, 3})},
      {"coefficient", makeTGData({1, 1}, 1, {3, 3})},
      {"grid point counts", makeTGData({1, 1}, -1, {3, 4})},
      {"nodes", makeTGData({1, 1}, -1, {3, 3}, 0.01)},
  };

  TestSG reference(2);
  reference.setTensorGrids({tgA, tgB});

  for (const auto& [differingProperty, modifiedTGB] : modifiedTGBs) {
    BOOST_TEST_CONTEXT("Differing in: " << differingProperty) {
      TestSG modified(2);
      modified.setTensorGrids({tgA, modifiedTGB});

      BOOST_CHECK(!(reference == modified));
      BOOST_CHECK(!(modified == reference));
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
