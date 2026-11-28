// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tensor_grid_processing_order_test.cpp
 * @brief Boost.Test cases for the largest-first tensor-grid order of the operators: the size
 * classes (tools::sparse_grid::sizeClassOf), the stable counting sort with a floor class
 * (tools::sparse_grid::orderBySizeClassDescending) and the order a grid computes
 * (SparseGridBase::getTGProcessingOrder) for sparse and deltas sparse grids.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid/tensor_grid_processing_order.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace {

using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::SGGenInstr;
using sgpp::combigrid::SparseGridBase;
using sgpp::combigrid::tools::sparse_grid::N_SIZE_CLASSES;
using sgpp::combigrid::tools::sparse_grid::orderBySizeClassDescending;
using sgpp::combigrid::tools::sparse_grid::sizeClassOf;
using sgpp::combigrid::tools::sparse_grid::SizeClassType;

/// Checks that @p grid's processing order is a permutation sorted by decreasing size class of the
/// grid-point counts, where the classes below the one of 1/4096 of all points count as that class,
/// ascending within a (floored) class, and starts with a largest tensor grid's class.
void checkProcessingOrder(const SparseGridBase& grid) {
  const std::span<const size_t> order = grid.getTGProcessingOrder();
  BOOST_REQUIRE_EQUAL(order.size(), grid.nTG());

  size_t totalGPCnt = 0;
  for (size_t tgIdx = 0; tgIdx < grid.nTG(); tgIdx++) {
    totalGPCnt += grid.getDomainTensorGrid(tgIdx).tensorGrid.nGP();
  }
  const SizeClassType floorClass = sizeClassOf(totalGPCnt / 4096);
  const auto flooredClassOf = [&](const size_t tgIdx) {
    return std::max(sizeClassOf(grid.getDomainTensorGrid(tgIdx).tensorGrid.nGP()), floorClass);
  };

  std::vector<bool> seen(grid.nTG(), false);
  for (size_t pos = 0; pos < order.size(); pos++) {
    BOOST_REQUIRE_LT(order[pos], grid.nTG());
    BOOST_CHECK(!seen[order[pos]]);
    seen[order[pos]] = true;
    if (pos == 0) continue;
    const SizeClassType previous = flooredClassOf(order[pos - 1]);
    const SizeClassType current = flooredClassOf(order[pos]);
    BOOST_CHECK_GE(previous, current);
    if (previous == current) BOOST_CHECK_LT(order[pos - 1], order[pos]);
  }
  if (!order.empty()) {
    BOOST_CHECK_EQUAL(flooredClassOf(order[0]),
                      std::max(sizeClassOf(grid.getMaxTGGPCnt()), floorClass));
  }
}

/// Configures @p genInstr (Clenshaw-Curtis or first-kind Chebyshev nodes, boundary offset).
void configure(SGGenInstr& genInstr, const bool chebyshev, const LvlType boundary) {
  genInstr.setNodeFamily(chebyshev ? sgpp::combigrid::getFirstTypeChebyshevNodes()
                                   : sgpp::combigrid::getClenshawCurtisNodes());
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(boundary);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(tensor_grid_processing_order)

BOOST_AUTO_TEST_CASE(Size_Classes_Are_Exponent_And_Three_Mantissa_Bits) {
  BOOST_CHECK_EQUAL(sizeClassOf(0), 0);
  BOOST_CHECK_EQUAL(sizeClassOf(1), 8);
  BOOST_CHECK_EQUAL(sizeClassOf(2), 16);
  BOOST_CHECK_EQUAL(sizeClassOf(3), 20);  // 1.5 * 2^1: mantissa bits 100
  BOOST_CHECK_EQUAL(sizeClassOf(8), 32);
  BOOST_CHECK_EQUAL(sizeClassOf(15), 39);
  BOOST_CHECK_EQUAL(sizeClassOf(16), 40);
  // Sizes within a factor of 1.125 above a power of two share a class
  BOOST_CHECK_EQUAL(sizeClassOf(1024), sizeClassOf(1151));
  BOOST_CHECK_EQUAL(sizeClassOf(1152), sizeClassOf(1024) + 1);
  BOOST_CHECK_EQUAL(sizeClassOf(std::numeric_limits<size_t>::max()), N_SIZE_CLASSES - 1);
}

BOOST_AUTO_TEST_CASE(Size_Classes_Are_Non_Decreasing) {
  for (size_t nGP = 1; nGP < 70000; nGP++) {
    BOOST_REQUIRE_LE(sizeClassOf(nGP - 1), sizeClassOf(nGP));
  }
  for (unsigned shift = 1; shift < 64; shift++) {
    const size_t power = size_t{1} << shift;
    BOOST_REQUIRE_LT(sizeClassOf(power - 1), sizeClassOf(power));
    BOOST_REQUIRE_LE(sizeClassOf(power), sizeClassOf(power + power / 2));
  }
}

BOOST_AUTO_TEST_CASE(Counting_Sort_Is_Descending_And_Stable) {
  const std::vector<SizeClassType> classes{3, 9, 3, 0, 9, 5, N_SIZE_CLASSES - 1};
  const std::vector<size_t> expected{6, 1, 4, 5, 0, 2, 3};
  const std::vector<size_t> order = orderBySizeClassDescending(classes, 0);
  BOOST_CHECK_EQUAL_COLLECTIONS(order.begin(), order.end(), expected.begin(), expected.end());

  BOOST_CHECK(orderBySizeClassDescending(std::vector<SizeClassType>{}, 0).empty());
}

BOOST_AUTO_TEST_CASE(Classes_Below_The_Floor_Keep_Their_Index_Order) {
  const std::vector<SizeClassType> classes{3, 9, 3, 0, 9, 5, 4};
  const std::vector<size_t> expected{1, 4, 5, 0, 2, 3, 6};  // 3, 0 and 4 count as 4
  const std::vector<size_t> order = orderBySizeClassDescending(classes, 4);
  BOOST_CHECK_EQUAL_COLLECTIONS(order.begin(), order.end(), expected.begin(), expected.end());

  // Floor above every class: the identity
  const std::vector<size_t> identity = orderBySizeClassDescending(classes, 10);
  for (size_t idx = 0; idx < identity.size(); idx++) BOOST_CHECK_EQUAL(identity[idx], idx);
}

BOOST_AUTO_TEST_CASE(Grids_Order_Their_Tensor_Grids_Largest_First) {
  // Strongly different tensor grid sizes (1 to about 4000 points) and complete grids, boundary
  // offsets 0 / 1, sparse and deltas sparse grids (delta tensor grid sizes)
  const LvlMIVec skewed(std::vector<LvlMI>{LvlMI{11, 0, 0}, LvlMI{0, 0, 4}, LvlMI{2, 3, 1}});
  for (const LvlType boundary : {LvlType{0}, LvlType{1}}) {
    BOOST_TEST_CONTEXT("boundary offset " << boundary) {
      sgpp::combigrid::MIVecSGGenInstr mivec(skewed);
      configure(mivec, false, boundary);
      checkProcessingOrder(sgpp::combigrid::SparseGrid(mivec));
      checkProcessingOrder(sgpp::combigrid::DeltasSparseGrid(mivec));

      sgpp::combigrid::CompleteSGGenInstr complete(5, 4);
      configure(complete, true, boundary);
      checkProcessingOrder(sgpp::combigrid::SparseGrid(complete));

      sgpp::combigrid::CompleteSGGenInstr oneDim(6, 1);
      configure(oneDim, false, boundary);
      checkProcessingOrder(sgpp::combigrid::DeltasSparseGrid(oneDim));
    }
  }
}

BOOST_AUTO_TEST_CASE(Grid_Of_Small_Tensor_Grids_Keeps_Index_Order) {
  // Deltas sparse grid d15 L7, boundary offset 1: 170544 delta tensor grids of at most 2^14 points,
  // each below 1/4096 of all points
  sgpp::combigrid::CompleteSGGenInstr genInstr(7, 15);
  genInstr.setNodeFamily(sgpp::combigrid::getEquidistantNodes());
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);
  const sgpp::combigrid::DeltasSparseGrid grid(genInstr);

  size_t totalGPCnt = 0;
  for (size_t tgIdx = 0; tgIdx < grid.nTG(); tgIdx++) {
    totalGPCnt += grid.getDomainTensorGrid(tgIdx).tensorGrid.nGP();
  }
  BOOST_REQUIRE_LT(grid.getMaxTGGPCnt(), totalGPCnt / 4096);

  const std::span<const size_t> order = grid.getTGProcessingOrder();
  BOOST_REQUIRE_EQUAL(order.size(), grid.nTG());
  for (size_t pos = 0; pos < order.size(); pos++) {
    BOOST_REQUIRE_EQUAL(order[pos], pos);
  }
}

BOOST_AUTO_TEST_SUITE_END()
