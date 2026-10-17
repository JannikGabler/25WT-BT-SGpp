// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_base_test.cpp
 * @brief Boost.Test cases for the container functionality shared by all sparse grid variants via
 * @c SparseGridBase: construction, element access, iteration, the protected modifiers, the cached
 * meta values and the order-insensitive equality.
 *
 * @c SparseGridBase cannot be instantiated directly, so the tests use @c TestSG, a minimal
 * subclass that makes the protected constructors, modifiers and helpers public.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
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

/// Concrete sparse grid whose protected interface is public, so that it can be tested directly.
class TestSG : public SparseGridBase {
 public:
  explicit TestSG(const size_t nDim) : SparseGridBase(nDim) {}
  TestSG(const size_t nDim, const size_t nTG) : SparseGridBase(nDim, nTG) {}

  using SparseGridBase::addTensorGrid;
  using SparseGridBase::recomputeMetaValues;
  using SparseGridBase::reserve;
  using SparseGridBase::resize;
  using SparseGridBase::setTensorGrid;
  using SparseGridBase::setTensorGrids;
  using SparseGridBase::tensorGridData;
};

/**
 * Tensor grid data with level multi-index @p mi, coefficient @p coeff and a tensor grid with
 * @p gpCntPerDim[k] nodes in dimension k. The nodes of dimension k are
 * (i + 1) / (n_k + 1) + @p shift for i = 0, ..., n_k - 1.
 */
TensorGridCTData makeTGData(const LvlMI& mi, const CTCoeffType coeff, const GPMI& gpCntPerDim,
                            const double shift = 0.0) {
  sgpp::base::DataVector nodesPerDim(gpCntPerDim.sumOfElems<size_t>());
  size_t nodeIdx = 0;

  for (const GPCntType nNodes : gpCntPerDim) {
    for (GPCntType i = 0; i < nNodes; i++) {
      nodesPerDim[nodeIdx++] = static_cast<double>(i + 1) / static_cast<double>(nNodes + 1) + shift;
    }
  }

  return {mi, coeff, TensorGrid(gpCntPerDim, nodesPerDim)};
}

/**
 * Three 2D tensor grids where the maxima of the meta values stem from different tensor grids:
 * the first has the most grid points (10 * 10 = 100), the second the largest sum over the
 * per-dimension grid point counts (1 + 50 = 51) and the third is maximal in neither (6, 5).
 */
std::vector<TensorGridCTData> sampleTGs() {
  return {makeTGData({3, 3}, 1, {10, 10}), makeTGData({0, 5}, -1, {1, 50}),
          makeTGData({1, 1}, 2, {2, 3})};
}

/// Sparse grid containing @p tgs in the given order, filled via @c addTensorGrid.
TestSG makeSG(const size_t nDim, const std::vector<TensorGridCTData>& tgs) {
  TestSG sg(nDim);

  for (const TensorGridCTData& tg : tgs) {
    sg.addTensorGrid(tg);
  }

  return sg;
}

/// Random tensor grid data with 1 to 6 grid points per dimension.
TensorGridCTData randomTGData(const size_t nDim) {
  LvlMI mi(nDim);
  GPMI gpCntPerDim(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(4));
    gpCntPerDim[dim] = static_cast<GPCntType>(1 + randGen.getUniformIndexRN(6));
  }

  const CTCoeffType coeff = static_cast<CTCoeffType>(randGen.getUniformIndexRN(7)) - 3;

  return makeTGData(mi, coeff, gpCntPerDim);
}

/// Maximum number of grid points over all tensor grids of @p sg, computed from scratch.
size_t bruteForceMaxTGGPCnt(const SparseGridBase& sg) {
  size_t maximum = 0;

  for (const TensorGridCTData& tg : sg) {
    maximum = std::max(maximum, tg.tensorGrid.nGP());
  }

  return maximum;
}

/// Maximum sum over the per-dimension grid point counts of all tensor grids of @p sg.
size_t bruteForceMaxTGSumOverGPCntsPerDim(const SparseGridBase& sg) {
  size_t maximum = 0;

  for (const TensorGridCTData& tg : sg) {
    maximum = std::max(maximum, tg.tensorGrid.getGPCntPerDim().sumOfElems<size_t>());
  }

  return maximum;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(SparseGridBase_construction)

BOOST_AUTO_TEST_CASE(Empty_Grid) {
  for (size_t nDim = 0; nDim <= 3; nDim++) {
    BOOST_TEST_CONTEXT("nDim: " << nDim) {
      const TestSG sg(nDim);

      BOOST_CHECK_EQUAL(sg.nDim(), nDim);
      BOOST_CHECK_EQUAL(sg.nTG(), size_t{0});
      BOOST_CHECK(sg.getTensorGrids().empty());
      BOOST_CHECK(sg.begin() == sg.end());
      BOOST_CHECK(sg.rbegin() == sg.rend());
      BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{0});
      BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{0});
    }
  }
}

BOOST_AUTO_TEST_CASE(Preallocated_Slots) {
  const TestSG sg(2, 4);

  BOOST_CHECK_EQUAL(sg.nDim(), size_t{2});
  BOOST_REQUIRE_EQUAL(sg.nTG(), size_t{4});

  for (const TensorGridCTData& tg : sg) {
    BOOST_CHECK(tg == TensorGridCTData{});
  }

  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{0});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{0});
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGridBase_access)

BOOST_AUTO_TEST_CASE(GetTensorGrid_By_Index) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TestSG sg = makeSG(2, tgs);

  BOOST_REQUIRE_EQUAL(sg.nTG(), tgs.size());

  for (size_t idx = 0; idx < tgs.size(); idx++) {
    BOOST_TEST_CONTEXT("Index: " << idx) {
      BOOST_CHECK(sg.getTensorGrid(idx) == tgs[idx]);
      BOOST_CHECK(&sg.getTensorGrid(idx) == &sg.getTensorGrids()[idx]);
    }
  }
}

BOOST_AUTO_TEST_CASE(GetTensorGrid_By_MI) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TestSG sg = makeSG(2, tgs);

  for (size_t idx = 0; idx < tgs.size(); idx++) {
    BOOST_TEST_CONTEXT("Index: " << idx) {
      const auto iter = sg.getTensorGrid(tgs[idx].mi);

      BOOST_REQUIRE(iter != sg.end());
      BOOST_CHECK(&*iter == &sg.getTensorGrid(idx));
    }
  }

  BOOST_CHECK(sg.getTensorGrid(LvlMI{2, 2}) == sg.end());
  BOOST_CHECK(sg.getTensorGrid(LvlMI{5, 0}) == sg.end());

  const TestSG emptySG(2);
  BOOST_CHECK(emptySG.getTensorGrid(LvlMI{0, 0}) == emptySG.end());
}

BOOST_AUTO_TEST_CASE(GetTensorGrids) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TestSG sg = makeSG(2, tgs);

  const std::span<const TensorGridCTData> span = sg.getTensorGrids();

  BOOST_REQUIRE_EQUAL(span.size(), tgs.size());
  BOOST_CHECK(std::equal(span.begin(), span.end(), tgs.begin(), tgs.end()));
  // The span is a view onto the stored tensor grids, not a copy
  BOOST_CHECK(span.data() == &sg.getTensorGrid(0));
}

BOOST_AUTO_TEST_CASE(Forward_Iteration) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg = makeSG(2, tgs);
  const TestSG& constSG = sg;

  BOOST_CHECK(std::equal(sg.begin(), sg.end(), tgs.begin(), tgs.end()));
  BOOST_CHECK(std::equal(constSG.begin(), constSG.end(), tgs.begin(), tgs.end()));
  BOOST_CHECK(std::equal(sg.cbegin(), sg.cend(), tgs.begin(), tgs.end()));

  // The iterators refer to the stored tensor grids
  BOOST_CHECK(&*sg.begin() == &sg.getTensorGrid(0));
  BOOST_CHECK(&*constSG.begin() == &sg.getTensorGrid(0));
  BOOST_CHECK(&*sg.cbegin() == &sg.getTensorGrid(0));

  size_t idx = 0;
  for (const TensorGridCTData& tg : constSG) {
    BOOST_CHECK(&tg == &sg.getTensorGrid(idx));
    idx++;
  }
  BOOST_CHECK_EQUAL(idx, tgs.size());
}

BOOST_AUTO_TEST_CASE(Reverse_Iteration) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg = makeSG(2, tgs);
  const TestSG& constSG = sg;

  BOOST_CHECK(std::equal(sg.rbegin(), sg.rend(), tgs.rbegin(), tgs.rend()));
  BOOST_CHECK(std::equal(constSG.rbegin(), constSG.rend(), tgs.rbegin(), tgs.rend()));
  BOOST_CHECK(std::equal(sg.crbegin(), sg.crend(), tgs.rbegin(), tgs.rend()));

  BOOST_CHECK(&*sg.rbegin() == &sg.getTensorGrid(tgs.size() - 1));
  BOOST_CHECK(&*constSG.rbegin() == &sg.getTensorGrid(tgs.size() - 1));
  BOOST_CHECK(&*sg.crbegin() == &sg.getTensorGrid(tgs.size() - 1));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGridBase_modification)

BOOST_AUTO_TEST_CASE(AddTensorGrid_Copy) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg(2);

  for (size_t idx = 0; idx < tgs.size(); idx++) {
    const TensorGridCTData& tg = tgs[idx];
    sg.addTensorGrid(tg);

    BOOST_CHECK_EQUAL(sg.nTG(), idx + 1);
  }

  BOOST_CHECK(std::equal(sg.begin(), sg.end(), tgs.begin(), tgs.end()));
}

BOOST_AUTO_TEST_CASE(AddTensorGrid_Move) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg(2);

  for (size_t idx = 0; idx < tgs.size(); idx++) {
    TensorGridCTData tg = tgs[idx];
    sg.addTensorGrid(std::move(tg));

    BOOST_CHECK_EQUAL(sg.nTG(), idx + 1);
  }

  BOOST_CHECK(std::equal(sg.begin(), sg.end(), tgs.begin(), tgs.end()));
}

BOOST_AUTO_TEST_CASE(SetTensorGrid_Copy) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TensorGridCTData replacement = makeTGData({4, 0}, -3, {7, 1});
  TestSG sg = makeSG(2, tgs);

  sg.setTensorGrid(1, replacement);

  BOOST_REQUIRE_EQUAL(sg.nTG(), tgs.size());
  BOOST_CHECK(sg.getTensorGrid(0) == tgs[0]);
  BOOST_CHECK(sg.getTensorGrid(1) == replacement);
  BOOST_CHECK(sg.getTensorGrid(2) == tgs[2]);
}

BOOST_AUTO_TEST_CASE(SetTensorGrid_Move) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TensorGridCTData replacement = makeTGData({4, 0}, -3, {7, 1});
  TestSG sg = makeSG(2, tgs);

  TensorGridCTData movedReplacement = replacement;
  sg.setTensorGrid(1, std::move(movedReplacement));

  BOOST_REQUIRE_EQUAL(sg.nTG(), tgs.size());
  BOOST_CHECK(sg.getTensorGrid(0) == tgs[0]);
  BOOST_CHECK(sg.getTensorGrid(1) == replacement);
  BOOST_CHECK(sg.getTensorGrid(2) == tgs[2]);
}

BOOST_AUTO_TEST_CASE(SetTensorGrid_Fills_Preallocated_Slots) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg(2, tgs.size());

  for (size_t idx = 0; idx < tgs.size(); idx++) {
    sg.setTensorGrid(idx, tgs[idx]);
  }

  BOOST_CHECK(std::equal(sg.begin(), sg.end(), tgs.begin(), tgs.end()));
}

BOOST_AUTO_TEST_CASE(SetTensorGrids) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg = makeSG(2, {makeTGData({4, 0}, -3, {7, 1})});

  std::vector<TensorGridCTData> newTGs = tgs;
  sg.setTensorGrids(std::move(newTGs));

  BOOST_CHECK(std::equal(sg.begin(), sg.end(), tgs.begin(), tgs.end()));

  sg.setTensorGrids({});

  BOOST_CHECK_EQUAL(sg.nTG(), size_t{0});
  BOOST_CHECK_EQUAL(sg.nDim(), size_t{2});
}

BOOST_AUTO_TEST_CASE(Resize) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg = makeSG(2, tgs);

  // Growing appends default-constructed slots
  sg.resize(5);

  BOOST_REQUIRE_EQUAL(sg.nTG(), size_t{5});
  BOOST_CHECK(std::equal(tgs.begin(), tgs.end(), sg.begin()));
  BOOST_CHECK(sg.getTensorGrid(3) == TensorGridCTData{});
  BOOST_CHECK(sg.getTensorGrid(4) == TensorGridCTData{});

  // Shrinking keeps the leading tensor grids
  sg.resize(2);

  BOOST_REQUIRE_EQUAL(sg.nTG(), size_t{2});
  BOOST_CHECK(sg.getTensorGrid(0) == tgs[0]);
  BOOST_CHECK(sg.getTensorGrid(1) == tgs[1]);
}

BOOST_AUTO_TEST_CASE(Reserve) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg = makeSG(2, {tgs[0]});

  const size_t nAdditionalTGs = 10;
  sg.reserve(1 + nAdditionalTGs);

  BOOST_REQUIRE_EQUAL(sg.nTG(), size_t{1});
  BOOST_CHECK(sg.getTensorGrid(0) == tgs[0]);

  // Adding up to the reserved number of tensor grids must not reallocate the storage
  const TensorGridCTData* const storage = &sg.getTensorGrid(0);
  for (size_t i = 0; i < nAdditionalTGs; i++) {
    sg.addTensorGrid(tgs[i % tgs.size()]);
  }

  BOOST_CHECK_EQUAL(sg.nTG(), 1 + nAdditionalTGs);
  BOOST_CHECK(&sg.getTensorGrid(0) == storage);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGridBase_meta_values)

BOOST_AUTO_TEST_CASE(Maxima_Of_Different_Tensor_Grids) {
  const TestSG sg = makeSG(2, sampleTGs());

  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{100});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{51});
}

BOOST_AUTO_TEST_CASE(AddTensorGrid_Updates_Maxima) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg(2);

  sg.addTensorGrid(tgs[2]);
  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{6});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{5});

  TensorGridCTData tg = tgs[0];
  sg.addTensorGrid(std::move(tg));
  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{100});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{20});

  sg.addTensorGrid(tgs[1]);
  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{100});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{51});
}

BOOST_AUTO_TEST_CASE(SetTensorGrids_Recomputes_Maxima) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg = makeSG(2, tgs);

  // The maxima also have to decrease
  sg.setTensorGrids({tgs[2]});
  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{6});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{5});

  sg.setTensorGrids({});
  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{0});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{0});
}

BOOST_AUTO_TEST_CASE(SetTensorGrid_Replaces_Non_Maximal_TG) {
  TestSG sg = makeSG(2, sampleTGs());

  sg.setTensorGrid(2, makeTGData({1, 1}, 2, {30, 30}));

  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{900});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{60});
}

BOOST_AUTO_TEST_CASE(SetTensorGrid_Replaces_Maximal_TG_By_Smaller) {
  const TensorGridCTData smallTG = makeTGData({0, 0}, 1, {1, 2});

  {  // Replace the tensor grid with the most grid points
    TestSG sg = makeSG(2, sampleTGs());
    sg.setTensorGrid(0, smallTG);

    BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{50});
    BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{51});
  }

  {  // Replace the tensor grid with the largest sum over the per-dimension grid point counts
    TestSG sg = makeSG(2, sampleTGs());
    TensorGridCTData tg = smallTG;
    sg.setTensorGrid(1, std::move(tg));

    BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{100});
    BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{20});
  }
}

BOOST_AUTO_TEST_CASE(SetTensorGrid_Replaces_Maximal_TG_By_Larger) {
  {  // Replace the tensor grid with the most grid points
    TestSG sg = makeSG(2, sampleTGs());
    sg.setTensorGrid(0, makeTGData({3, 3}, 1, {20, 20}));

    BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{400});
    BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{51});
  }

  {  // Replace the tensor grid with the largest sum over the per-dimension grid point counts
    TestSG sg = makeSG(2, sampleTGs());
    sg.setTensorGrid(1, makeTGData({0, 5}, -1, {1, 80}));

    BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{100});
    BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{81});
  }
}

BOOST_AUTO_TEST_CASE(SetTensorGrid_Fills_Preallocated_Slots) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  TestSG sg(2, tgs.size());

  for (size_t idx = 0; idx < tgs.size(); idx++) {
    sg.setTensorGrid(idx, tgs[idx]);
  }

  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{100});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{51});
}

BOOST_AUTO_TEST_CASE(RecomputeMetaValues) {
  TestSG sg = makeSG(2, sampleTGs());

  // Remove the tensor grid with the most grid points behind the back of the cached meta values
  sg.tensorGridData.erase(sg.tensorGridData.begin());
  sg.recomputeMetaValues();

  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{50});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{51});

  sg.tensorGridData.clear();
  sg.recomputeMetaValues();

  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), size_t{0});
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), size_t{0});
}

BOOST_AUTO_TEST_CASE(Random_Modification_Sequence) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(3);
    TestSG sg(nDim, randGen.getUniformIndexRN(3));

    for (size_t step = 0; step < 200; step++) {
      TensorGridCTData tg = randomTGData(nDim);
      const size_t operation = sg.nTG() == 0 ? 0 : randGen.getUniformIndexRN(5);
      const size_t idx = sg.nTG() == 0 ? 0 : randGen.getUniformIndexRN(sg.nTG());

      switch (operation) {
        case 0:
          sg.addTensorGrid(tg);
          break;
        case 1:
          sg.addTensorGrid(std::move(tg));
          break;
        case 2:
          sg.setTensorGrid(idx, tg);
          break;
        case 3:
          sg.setTensorGrid(idx, std::move(tg));
          break;
        default: {
          std::vector<TensorGridCTData> tgs(randGen.getUniformIndexRN(5));
          std::generate(tgs.begin(), tgs.end(), [nDim]() { return randomTGData(nDim); });
          sg.setTensorGrids(std::move(tgs));
          break;
        }
      }

      // Stop at the first mismatch, all later steps would only repeat it
      BOOST_TEST_CONTEXT("Step: " << step << ", operation: " << operation << ", index: " << idx) {
        BOOST_REQUIRE_EQUAL(sg.getMaxTGGPCnt(), bruteForceMaxTGGPCnt(sg));
        BOOST_REQUIRE_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(),
                            bruteForceMaxTGSumOverGPCntsPerDim(sg));
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGridBase_equality)

BOOST_AUTO_TEST_CASE(Empty_Grids) {
  BOOST_CHECK(TestSG(0) == TestSG(0));
  BOOST_CHECK(TestSG(2) == TestSG(2));
  BOOST_CHECK(!(TestSG(2) == TestSG(3)));
}

BOOST_AUTO_TEST_CASE(Reflexive) {
  const TestSG sg = makeSG(2, sampleTGs());

  BOOST_CHECK(sg == sg);
}

BOOST_AUTO_TEST_CASE(Order_Insensitive) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TestSG sg = makeSG(2, tgs);
  const TestSG permutedSG = makeSG(2, {tgs[2], tgs[0], tgs[1]});

  BOOST_CHECK(sg == permutedSG);
  BOOST_CHECK(permutedSG == sg);
}

BOOST_AUTO_TEST_CASE(Different_Number_Of_Tensor_Grids) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TestSG sg = makeSG(2, tgs);
  const TestSG smallerSG = makeSG(2, {tgs[0], tgs[1]});

  BOOST_CHECK(!(sg == smallerSG));
  BOOST_CHECK(!(smallerSG == sg));
}

BOOST_AUTO_TEST_CASE(Different_Tensor_Grid_Data) {
  const std::vector<TensorGridCTData> tgs = sampleTGs();
  const TensorGridCTData& tg = tgs[1];

  const std::vector<std::pair<std::string, TensorGridCTData>> modifications{
      {"coefficient", makeTGData(tg.mi, tg.coefficient + 1, tg.tensorGrid.getGPCntPerDim())},
      {"multi-index", makeTGData({0, 6}, tg.coefficient, tg.tensorGrid.getGPCntPerDim())},
      {"nodes", makeTGData(tg.mi, tg.coefficient, tg.tensorGrid.getGPCntPerDim(), 0.01)},
      // Same number of grid points (50), but distributed differently
      {"grid point counts", makeTGData(tg.mi, tg.coefficient, {2, 25})}};

  const TestSG sg = makeSG(2, tgs);

  for (const auto& [name, modifiedTG] : modifications) {
    BOOST_TEST_CONTEXT("Modified: " << name) {
      std::vector<TensorGridCTData> modifiedTGs = tgs;
      modifiedTGs[1] = modifiedTG;
      const TestSG modifiedSG = makeSG(2, modifiedTGs);

      BOOST_CHECK(!(sg == modifiedSG));
      BOOST_CHECK(!(modifiedSG == sg));
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
