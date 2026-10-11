// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tensor_grid_iterator_test.cpp
 * @brief Boost.Test cases for iterating the grid points of a @c TensorGrid:
 * the read-only iteration of the tensor grid itself, the mutable iteration
 * via @c TensorGrid::mutableGridPoints(), and the iterator interface.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_iterator.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;
using sgpp::base::DataVector;

namespace {

sgpp::base::RandomNumberGenerator& randGen = sgpp::base::RandomNumberGenerator::getInstance();

/// Tensor grid with the given per-dimension counts and random node positions.
TensorGrid randomTensorGrid(const GPMI& nGPPerDim) {
  DataVector nodes(nGPPerDim.sumOfElems<size_t>());
  for (double& node : nodes) {
    node = randGen.getUniformRN();
  }
  return TensorGrid(nGPPerDim, nodes);
}

/// Random tensor grid with 1 to @p maxDim dimensions and 1 to @p maxCnt nodes per dimension.
TensorGrid randomTensorGrid(const size_t maxDim, const size_t maxCnt) {
  GPMI nGPPerDim(1 + randGen.getUniformIndexRN(maxDim));
  for (GPCntType& nGP : nGPPerDim) {
    nGP = static_cast<GPCntType>(1 + randGen.getUniformIndexRN(maxCnt));
  }
  return randomTensorGrid(nGPPerDim);
}

std::string describe(const TensorGrid& tg) {
  std::string result = "Grid-point counts:";
  for (const GPCntType nGP : tg.getGPCntPerDim()) {
    result += " " + std::to_string(nGP);
  }
  return result;
}

/// Checks one yielded element against the linear-index access of the tensor grid. The
/// coordinates are compared exactly: the iterators copy the node positions.
void checkPoint(const TensorGrid& tg, const size_t idx, const TensorGridPoint& point) {
  BOOST_REQUIRE_LT(idx, tg.nGP());
  BOOST_REQUIRE_EQUAL(point.mi.size(), tg.nDim());
  BOOST_REQUIRE_EQUAL(point.gp.size(), tg.nDim());

  DataVector expectedGP(tg.nDim());
  GPMI expectedMI(tg.nDim());
  tg.getGridPointAndMI(idx, expectedGP, expectedMI);

  BOOST_CHECK(point.mi == expectedMI);
  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    BOOST_CHECK_EQUAL(point.gp[dim], expectedGP[dim]);
  }
}

/// Iterates @p tg read-only and checks every element and the number of elements.
void checkConstIteration(const TensorGrid& tg) {
  BOOST_TEST_CONTEXT(describe(tg)) {
    size_t idx = 0;
    for (const TensorGridPoint& point : tg) {
      checkPoint(tg, idx, point);
      idx++;
    }
    BOOST_CHECK_EQUAL(idx, tg.nGP());
  }
}

/// Iterates @p tg via mutableGridPoints(), checks every element and lets @p alter modify it.
template <typename Alter>
void checkMutableIteration(const TensorGrid& tg, Alter alter) {
  BOOST_TEST_CONTEXT(describe(tg)) {
    size_t idx = 0;
    for (TensorGridPoint& point : tg.mutableGridPoints()) {
      checkPoint(tg, idx, point);
      alter(idx, point);
      idx++;
    }
    BOOST_CHECK_EQUAL(idx, tg.nGP());
  }
}

void keepUnchanged(size_t, TensorGridPoint&) {}

/// Overwrites every entry of the element with values that are valid for no grid point.
void overwriteAllEntries(size_t, TensorGridPoint& point) {
  for (GPCntType& idx : point.mi) {
    idx = 4000000000u;
  }
  for (double& coordinate : point.gp) {
    coordinate = -12345.678;
  }
}

/// Tensor grids that exercise the carry logic of the iterators.
std::vector<TensorGrid> edgeCaseGrids() {
  const std::vector<GPMI> counts = {{1},          {5},          {1, 1},          {1, 1, 1},
                                    {4, 1},       {1, 4},       {1, 3, 1, 2, 1}, {2, 1, 1, 3},
                                    {1, 1, 1, 6}, {3, 3, 3, 3}, {2, 2, 2, 2, 2, 2, 2},
                                    {1, 1, 7, 1, 3, 1, 1, 5, 1, 3}};
  std::vector<TensorGrid> grids;
  for (const GPMI& nGPPerDim : counts) {
    grids.push_back(randomTensorGrid(nGPPerDim));
  }
  return grids;
}

/// Tensor grids without grid points.
std::vector<TensorGrid> emptyGrids() {
  std::vector<TensorGrid> grids;
  grids.push_back(TensorGrid());
  grids.push_back(TensorGrid(GPMI{0}));
  grids.push_back(TensorGrid(GPMI{0, 3}));
  grids.push_back(TensorGrid(GPMI{3, 0, 2}));
  grids.push_back(TensorGrid(GPMI{2, 4, 0}));
  return grids;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(TensorGrid_const_iteration)

BOOST_AUTO_TEST_CASE(Handwritten_2D_Grid) {
  const TensorGrid tg(GPMI{2, 3}, DataVector(std::vector<double>{0.1, 0.9, 0.2, 0.5, 0.7}));
  const std::vector<std::vector<GPCntType>> expectedMI = {{0, 0}, {1, 0}, {0, 1},
                                                          {1, 1}, {0, 2}, {1, 2}};
  const std::vector<std::vector<double>> expectedGP = {{0.1, 0.2}, {0.9, 0.2}, {0.1, 0.5},
                                                       {0.9, 0.5}, {0.1, 0.7}, {0.9, 0.7}};

  size_t idx = 0;
  for (const TensorGridPoint& point : tg) {
    BOOST_REQUIRE_LT(idx, expectedMI.size());
    BOOST_CHECK(point.mi == expectedMI[idx]);
    BOOST_CHECK(static_cast<const std::vector<double>&>(point.gp) == expectedGP[idx]);
    idx++;
  }
  BOOST_CHECK_EQUAL(idx, expectedMI.size());
}

BOOST_AUTO_TEST_CASE(Random_Grids) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t rep = 0; rep < 30; rep++) {
      checkConstIteration(randomTensorGrid(5, 6));
    }
  }
}

BOOST_AUTO_TEST_CASE(Edge_Case_Grids) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (const TensorGrid& tg : edgeCaseGrids()) {
      checkConstIteration(tg);
    }
  }
}

BOOST_AUTO_TEST_CASE(Empty_Grids) {
  for (const TensorGrid& tg : emptyGrids()) {
    BOOST_TEST_CONTEXT(describe(tg)) {
      BOOST_CHECK(tg.begin() == tg.end());
      BOOST_CHECK(tg.cbegin() == tg.cend());
      BOOST_CHECK(tg.begin() == std::default_sentinel);
      checkConstIteration(tg);
    }
  }
}

BOOST_AUTO_TEST_CASE(Tensor_Grids_Of_A_Sparse_Grid) {
  CompleteSGGenInstr genInstr(4, 3);
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);
  const SparseGrid sg(genInstr);
  BOOST_REQUIRE_GT(sg.nTG(), 0u);

  for (const TensorGridCTData& tgData : sg) {
    const TensorGrid& tg = tgData.tensorGrid;
    BOOST_TEST_CONTEXT(describe(tg)) {
      const sgpp::base::DataMatrix expected = tg.getGridPoints();

      size_t idx = 0;
      for (const TensorGridPoint& point : tg) {
        BOOST_REQUIRE_LT(idx, expected.getNrows());
        for (size_t dim = 0; dim < tg.nDim(); dim++) {
          BOOST_CHECK_EQUAL(point.gp[dim], expected(idx, dim));
        }
        idx++;
      }
      BOOST_CHECK_EQUAL(idx, tg.nGP());
    }
  }
}

// The element must stay in the same storage during the whole loop: no allocation per step.
BOOST_AUTO_TEST_CASE(Storage_Is_Reused) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(GPMI{3, 1, 4, 2});

    const TensorGridPoint* firstPoint = nullptr;
    const GPCntType* firstMIData = nullptr;
    const double* firstGPData = nullptr;
    for (const TensorGridPoint& point : tg) {
      if (firstPoint == nullptr) {
        firstPoint = &point;
        firstMIData = point.mi.data();
        firstGPData = point.gp.data();
      }
      BOOST_CHECK_EQUAL(&point, firstPoint);
      BOOST_CHECK_EQUAL(point.mi.data(), firstMIData);
      BOOST_CHECK_EQUAL(point.gp.data(), firstGPData);
    }
  }
}

BOOST_AUTO_TEST_CASE(Nested_Loops_Are_Independent) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(GPMI{3, 2, 2});

    size_t outerIdx = 0;
    for (const TensorGridPoint& outer : tg) {
      size_t innerIdx = 0;
      for (const TensorGridPoint& inner : tg) {
        checkPoint(tg, innerIdx, inner);
        checkPoint(tg, outerIdx, outer);
        innerIdx++;
      }
      BOOST_CHECK_EQUAL(innerIdx, tg.nGP());
      outerIdx++;
    }
    BOOST_CHECK_EQUAL(outerIdx, tg.nGP());
  }
}

BOOST_AUTO_TEST_CASE(Structured_Bindings) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid constGrid = randomTensorGrid(4, 5);
    TensorGrid nonConstGrid = randomTensorGrid(4, 5);

    size_t idx = 0;
    for (const auto& [mi, gp] : constGrid) {
      static_assert(std::is_same<decltype(mi), const GPMI>::value, "mi must be a const GPMI");
      static_assert(std::is_same<decltype(gp), const DataVector>::value,
                    "gp must be a const base::DataVector");
      checkPoint(constGrid, idx, TensorGridPoint{mi, gp});
      idx++;
    }
    BOOST_CHECK_EQUAL(idx, constGrid.nGP());

    // Iterating a tensor grid directly is read-only even if the grid is not const and the
    // element is bound with auto&.
    idx = 0;
    for (auto& [mi, gp] : nonConstGrid) {
      static_assert(std::is_same<decltype(mi), const GPMI>::value, "mi must be a const GPMI");
      static_assert(std::is_same<decltype(gp), const DataVector>::value,
                    "gp must be a const base::DataVector");
      checkPoint(nonConstGrid, idx, TensorGridPoint{mi, gp});
      idx++;
    }
    BOOST_CHECK_EQUAL(idx, nonConstGrid.nGP());
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TensorGrid_mutable_iteration)

BOOST_AUTO_TEST_CASE(Unaltered_Elements) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t rep = 0; rep < 30; rep++) {
      checkMutableIteration(randomTensorGrid(5, 6), keepUnchanged);
    }
    for (const TensorGrid& tg : edgeCaseGrids()) {
      checkMutableIteration(tg, keepUnchanged);
    }
  }
}

// Whatever the loop body writes into the element must be gone after the increment.
BOOST_AUTO_TEST_CASE(Altered_Entries_Are_Rewritten) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t rep = 0; rep < 30; rep++) {
      checkMutableIteration(randomTensorGrid(5, 6), overwriteAllEntries);
    }
    for (const TensorGrid& tg : edgeCaseGrids()) {
      checkMutableIteration(tg, overwriteAllEntries);
    }
  }
}

// The loop body may exchange the containers for others with the same number of entries, e.g.
// to take a grid point out of the loop without copying it.
BOOST_AUTO_TEST_CASE(Containers_May_Be_Swapped_For_Equally_Sized_Ones) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t rep = 0; rep < 10; rep++) {
      const TensorGrid tg = randomTensorGrid(5, 6);
      GPMI takenMI(tg.nDim(), 4000000000u);
      DataVector takenGP(tg.nDim(), -12345.678);

      size_t idx = 0;
      for (TensorGridPoint& point : tg.mutableGridPoints()) {
        checkPoint(tg, idx, point);
        point.mi.swap(takenMI);
        point.gp.swap(takenGP);
        // The swapped-out containers hold the grid point, the element holds garbage.
        checkPoint(tg, idx, TensorGridPoint{takenMI, takenGP});
        idx++;
      }
      BOOST_CHECK_EQUAL(idx, tg.nGP());
    }
  }
}

BOOST_AUTO_TEST_CASE(Empty_Grids) {
  for (const TensorGrid& tg : emptyGrids()) {
    BOOST_TEST_CONTEXT(describe(tg)) {
      const TensorGridMutableRange range = tg.mutableGridPoints();
      BOOST_CHECK(range.begin() == range.end());
      checkMutableIteration(tg, overwriteAllEntries);
    }
  }
}

// Altering the working copies must not change the tensor grid.
BOOST_AUTO_TEST_CASE(Tensor_Grid_Is_Not_Modified) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(4, 5);
    const GPMI nGPPerDimBefore = tg.getGPCntPerDim();
    const std::vector<double> nodesBefore = tg.getNodesPerDim();

    checkMutableIteration(tg, overwriteAllEntries);

    BOOST_CHECK(tg.getGPCntPerDim() == nGPPerDimBefore);
    BOOST_CHECK(static_cast<const std::vector<double>&>(tg.getNodesPerDim()) == nodesBefore);
  }
}

// Without interference of the loop body the element stays in the same storage.
BOOST_AUTO_TEST_CASE(Storage_Is_Reused) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(GPMI{3, 1, 4, 2});

    const TensorGridPoint* firstPoint = nullptr;
    const GPCntType* firstMIData = nullptr;
    const double* firstGPData = nullptr;
    size_t idx = 0;
    for (TensorGridPoint& point : tg.mutableGridPoints()) {
      if (firstPoint == nullptr) {
        firstPoint = &point;
        firstMIData = point.mi.data();
        firstGPData = point.gp.data();
      }
      BOOST_CHECK_EQUAL(&point, firstPoint);
      BOOST_CHECK_EQUAL(point.mi.data(), firstMIData);
      BOOST_CHECK_EQUAL(point.gp.data(), firstGPData);
      overwriteAllEntries(idx++, point);
    }
  }
}

// The use case of the quadrature operator: the coordinates are transformed in place by
// SourceFunc::evaluateNormalizedInPlace. The result must agree (same summation order) with the
// loop over a bounding box with TensorGrid::getGridPoint.
BOOST_AUTO_TEST_CASE(In_Place_Evaluation_Equals_Bounding_Box_Loop) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const SourceFunc sourceFunc([](const DataVector& x) {
      double result = 0.5;
      for (size_t dim = 0; dim < x.size(); dim++) {
        result += static_cast<double>(dim + 1) * x[dim] * x[dim];
      }
      return result;
    });

    for (size_t rep = 0; rep < 10; rep++) {
      const TensorGrid tg = randomTensorGrid(4, 6);
      const size_t nDim = tg.nDim();
      HyperCubeArea bounds(nDim);
      for (std::pair<double, double>& interval : bounds) {
        interval.first = randGen.getUniformRN(-3.0, 0.0);
        interval.second = randGen.getUniformRN(1.0, 4.0);
      }

      double expected = 0;
      DataVector gp(nDim);
      const misc::DiscRectBB<GPCntType> iterationBB(GPMI(nDim), tg.getGPCntPerDim(), false);
      for (const GPMI& gpMI : iterationBB) {
        tg.getGridPoint(gpMI, gp);
        expected += static_cast<double>(gpMI.sumOfElems<size_t>() + 1) *
                    sourceFunc.evaluateNormalizedInPlace(gp, bounds);
      }

      double result = 0;
      for (TensorGridPoint& point : tg.mutableGridPoints()) {
        result += static_cast<double>(point.mi.sumOfElems<size_t>() + 1) *
                  sourceFunc.evaluateNormalizedInPlace(point.gp, bounds);
      }

      // Not compared exactly: the compiler may fuse multiply-add differently in the two loops.
      BOOST_TEST_CONTEXT(describe(tg)) { BOOST_CHECK_CLOSE_FRACTION(result, expected, 1e-13); }
    }
  }
}

BOOST_AUTO_TEST_CASE(Structured_Bindings) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(4, 5);

    size_t idx = 0;
    for (auto& [mi, gp] : tg.mutableGridPoints()) {
      static_assert(std::is_same<decltype(mi), GPMI>::value, "mi must be a modifiable GPMI");
      static_assert(std::is_same<decltype(gp), DataVector>::value,
                    "gp must be a modifiable base::DataVector");
      checkPoint(tg, idx, TensorGridPoint{mi, gp});
      for (size_t dim = 0; dim < tg.nDim(); dim++) {
        mi[dim] += 17;
        gp[dim] = 2.0 * gp[dim] - 1.0;
      }
      idx++;
    }
    BOOST_CHECK_EQUAL(idx, tg.nGP());
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TensorGrid_iterator_interface)

static_assert(std::same_as<TensorGrid::const_iterator, TensorGridConstIterator>);
static_assert(std::same_as<TensorGrid::iterator, TensorGrid::const_iterator>,
              "Iterating a tensor grid must be read-only");
static_assert(std::same_as<TensorGrid::mutable_iterator, TensorGridMutableIterator>);

static_assert(std::ranges::input_range<TensorGrid>);
static_assert(std::ranges::input_range<const TensorGrid>);
static_assert(std::ranges::input_range<TensorGridMutableRange>);
static_assert(std::same_as<std::ranges::range_reference_t<TensorGrid>, const TensorGridPoint&>,
              "Iterating a non-const tensor grid must be read-only as well");
static_assert(std::same_as<std::ranges::range_reference_t<const TensorGrid>,
                           const TensorGridPoint&>);
static_assert(
    std::same_as<std::ranges::range_reference_t<TensorGridMutableRange>, TensorGridPoint&>);
static_assert(std::same_as<std::ranges::sentinel_t<const TensorGrid>, std::default_sentinel_t>);
static_assert(
    std::same_as<std::ranges::sentinel_t<TensorGridMutableRange>, std::default_sentinel_t>);
// The iterators of the mutable range refer to the tensor grid, not to the range object.
static_assert(std::ranges::borrowed_range<TensorGridMutableRange>);
static_assert(!std::ranges::borrowed_range<TensorGrid>);

BOOST_AUTO_TEST_CASE(Explicit_Iterator_Loop) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(4, 5);

    size_t idx = 0;
    for (TensorGrid::const_iterator it = tg.begin(); it != tg.end(); ++it) {
      checkPoint(tg, idx, *it);
      BOOST_CHECK_EQUAL(&it->mi, &(*it).mi);
      BOOST_CHECK(!(it == std::default_sentinel));
      BOOST_CHECK(std::default_sentinel != it);
      idx++;
    }
    BOOST_CHECK_EQUAL(idx, tg.nGP());

    idx = 0;
    const TensorGridMutableRange range = tg.mutableGridPoints();
    for (TensorGrid::mutable_iterator it = range.begin(); it != range.end(); it++) {
      checkPoint(tg, idx, *it);
      it->gp[0] = -1.0;
      (*it).mi[0] = 99;
      idx++;
    }
    BOOST_CHECK_EQUAL(idx, tg.nGP());
  }
}

BOOST_AUTO_TEST_CASE(Reaching_The_End) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(GPMI{2, 3});

    TensorGrid::const_iterator it = tg.begin();
    for (size_t i = 0; i < tg.nGP(); i++) {
      BOOST_CHECK(it != std::default_sentinel);
      ++it;
    }
    BOOST_CHECK(it == std::default_sentinel);
    BOOST_CHECK(std::default_sentinel == it);

    TensorGrid::mutable_iterator mutableIt = tg.mutableGridPoints().begin();
    for (size_t i = 0; i < tg.nGP(); i++) {
      BOOST_CHECK(mutableIt != std::default_sentinel);
      ++mutableIt;
    }
    BOOST_CHECK(mutableIt == std::default_sentinel);
  }
}

// A copy owns its own element and advances on its own.
BOOST_AUTO_TEST_CASE(Copies_Advance_Independently) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(GPMI{3, 2, 4});

    TensorGrid::const_iterator it = tg.begin();
    ++it;
    ++it;
    ++it;
    TensorGrid::const_iterator copy = it;
    BOOST_CHECK_NE(&*copy, &*it);
    ++copy;
    ++copy;
    checkPoint(tg, 3, *it);
    checkPoint(tg, 5, *copy);
    it = copy;
    ++copy;
    checkPoint(tg, 5, *it);
    checkPoint(tg, 6, *copy);

    // The iterators of the mutable range outlive the (temporary) range object.
    TensorGrid::mutable_iterator mutableIt = tg.mutableGridPoints().begin();
    ++mutableIt;
    TensorGrid::mutable_iterator mutableCopy = mutableIt;
    overwriteAllEntries(0, *mutableIt);
    checkPoint(tg, 1, *mutableCopy);
    ++mutableCopy;
    ++mutableIt;
    ++mutableIt;
    checkPoint(tg, 2, *mutableCopy);
    checkPoint(tg, 3, *mutableIt);

    TensorGrid::mutable_iterator moved = std::move(mutableIt);
    checkPoint(tg, 3, *moved);
    ++moved;
    checkPoint(tg, 4, *moved);
  }
}

BOOST_AUTO_TEST_CASE(Ranges_Algorithms_And_Views) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(GPMI{3, 4, 2});

    BOOST_CHECK_EQUAL(static_cast<size_t>(std::ranges::distance(tg)), tg.nGP());
    BOOST_CHECK_EQUAL(
        static_cast<size_t>(std::ranges::count_if(
            tg, [](const TensorGridPoint& point) { return point.mi[1] == 2; })),
        tg.nGP() / 4);
    BOOST_CHECK_EQUAL(
        std::ranges::fold_left(
            tg, size_t{0},
            [](const size_t sum, const TensorGridPoint& point) { return sum + point.mi[0]; }),
        tg.nGP() / 3 * (0 + 1 + 2));

    size_t idx = 0;
    std::ranges::for_each(
        tg, [&tg, &idx](const TensorGridPoint& point) { checkPoint(tg, idx++, point); });
    BOOST_CHECK_EQUAL(idx, tg.nGP());

    idx = 0;
    std::ranges::for_each(tg.mutableGridPoints(), [&tg, &idx](TensorGridPoint& point) {
      checkPoint(tg, idx, point);
      overwriteAllEntries(idx++, point);
    });
    BOOST_CHECK_EQUAL(idx, tg.nGP());

    // An algorithm can hand back an iterator of the temporary mutable range (borrowed range).
    TensorGrid::mutable_iterator found = std::ranges::find_if(
        tg.mutableGridPoints(), [](const TensorGridPoint& point) { return point.mi[2] == 1; });
    BOOST_REQUIRE(found != std::default_sentinel);
    checkPoint(tg, 12, *found);

    // Grid points whose node index in the last dimension is 1: the linear indices 12, ..., 23.
    idx = 12;
    for (const TensorGridPoint& point : tg | std::views::filter([](const TensorGridPoint& p) {
                                          return p.mi[2] == 1;
                                        })) {
      checkPoint(tg, idx++, point);
    }
    BOOST_CHECK_EQUAL(idx, 24u);

    idx = 0;
    for (const TensorGridPoint& point : tg | std::views::take(5)) {
      checkPoint(tg, idx++, point);
    }
    BOOST_CHECK_EQUAL(idx, 5u);
  }
}

// Algorithms that take a pair of iterators of the same type need std::views::common.
BOOST_AUTO_TEST_CASE(Classic_Algorithms_Via_Common_View) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const TensorGrid tg = randomTensorGrid(4, 5);

    auto common = tg | std::views::common;
    size_t idx = 0;
    std::for_each(common.begin(), common.end(),
                  [&tg, &idx](const TensorGridPoint& point) { checkPoint(tg, idx++, point); });
    BOOST_CHECK_EQUAL(idx, tg.nGP());

    const std::ptrdiff_t nInFirstNode =
        std::count_if(common.begin(), common.end(),
                      [](const TensorGridPoint& point) { return point.mi[0] == 0; });
    BOOST_CHECK_EQUAL(static_cast<size_t>(nInFirstNode), tg.nGP() / tg.getGPCntPerDim()[0]);
  }
}

BOOST_AUTO_TEST_SUITE_END()
