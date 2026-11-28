// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_storage_test.cpp
 * @brief Boost.Test cases for the storage of the tensor grids shared by @c SparseGrid and
 * @c DeltasSparseGrid (@c SparseGridBase): the struct of arrays (levels, coefficients), the node
 * tables (@c NodeTable: one node list per distinct (dimension, level), on the unit interval and on
 * the domain, shared by the dimensions with the same nodes), the views (@c TensorGridView,
 * @c TensorGridCTDataView), the iterator, the cached maxima and the equality.
 *
 * The expected node lists are generated from the node families (and mapped with
 * @c tools::mapToDomain), independently of the generation code of the grids, and compared
 * bitwise.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

bool bitwiseEqual(const double a, const double b) {
  return std::bit_cast<uint64_t>(a) == std::bit_cast<uint64_t>(b);
}

/// Expected nodes of (dimension @p dim, level @p lvl): the regular nodes (sparse grid) or the
/// delta nodes (deltas sparse grid) of the node family, on the unit interval or
/// mapped onto the domain with tools::mapToDomain (@p onDomain).
DataVector expectedNodes(const SGGenInstr& genInstr, const size_t dim, const LvlType lvl,
                         const bool deltaNodes, const bool onDomain) {
  const NodeFamily* const nodeFamily = genInstr.getNodeFamilyForDim(dim);
  const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
  const LvlType offset = genInstr.getBoundaryLevelOffset();
  DataVector nodes = deltaNodes ? nodeFamily->genDeltaNodes(lvl, lvl2GPCntFunc, lvl == offset)
                                : nodeFamily->genNodes(lvl2GPCntFunc(lvl) + (lvl >= offset ? 2 : 0),
                                                       lvl >= offset);
  if (onDomain) {
    const auto [lower, upper] = genInstr.getDomainForDim(dim);
    for (double& node : nodes) node = tools::mapToDomain(node, lower, upper - lower);
  }
  return nodes;
}

/// Checks @p table, one of the node tables of @p grid: every (dimension, level up to the maximum
/// level of the levels of the grid) has the expected nodes, bitwise.
template <typename Grid>
void checkNodeTable(const Grid& grid, const NodeTable& table, const bool deltaNodes,
                    const bool onDomain) {
  const SGGenInstr& genInstr = grid.getGenInstr();
  BOOST_REQUIRE_EQUAL(table.nDim(), grid.nDim());

  for (size_t dim = 0; dim < grid.nDim(); dim++) {
    LvlType maxLvl = 0;
    for (const auto& tg : grid.domainTensorGrids()) maxLvl = std::max(maxLvl, tg.mi[dim]);
    BOOST_CHECK_EQUAL(table.maxLvl(dim), maxLvl);

    for (LvlType lvl = 0; lvl <= table.maxLvl(dim); lvl++) {
      BOOST_TEST_CONTEXT("dim " << dim << " lvl " << lvl) {
        const DataVector expected = expectedNodes(genInstr, dim, lvl, deltaNodes, onDomain);
        BOOST_REQUIRE_EQUAL(table.nNodes(dim, lvl), expected.size());
        for (size_t i = 0; i < expected.size(); i++) {
          BOOST_CHECK(bitwiseEqual(table.nodes(dim, lvl)[i], expected[i]));
        }
      }
    }
  }
}

/// Checks both node tables of @p grid and that the tensor grids refer to their lists.
template <typename Grid>
void checkNodeTable(const Grid& grid, const bool deltaNodes) {
  for (const bool onDomain : {true, false}) {
    BOOST_TEST_CONTEXT("on domain " << onDomain) {
      checkNodeTable(grid, onDomain ? grid.getDomainNodeTable() : grid.getUnitNodeTable(),
                     deltaNodes, onDomain);
    }
  }

  // Each tensor grid refers to the lists of its levels (no copies): the domain variant to the
  // domain table, the unit variant to the unit table; both have the same levels, coefficient and
  // counts
  const NodeTable& table = grid.getDomainNodeTable();
  const NodeTable& unitTable = grid.getUnitNodeTable();
  for (size_t idx = 0; idx < grid.nTG(); idx++) {
    const TensorGridCTDataView tg = grid.getDomainTensorGrid(idx);
    const TensorGridCTDataView unitTG = grid.getUnitTensorGrid(idx);
    BOOST_CHECK(unitTG.mi == tg.mi);
    BOOST_CHECK_EQUAL(unitTG.coefficient, tg.coefficient);
    for (size_t dim = 0; dim < grid.nDim(); dim++) {
      BOOST_CHECK_EQUAL(tg.tensorGrid.nodesInDim(dim), table.nodes(dim, tg.mi[dim]));
      BOOST_CHECK_EQUAL(tg.tensorGrid.nGPInDim(dim), table.nNodes(dim, tg.mi[dim]));
      BOOST_CHECK_EQUAL(unitTG.tensorGrid.nodesInDim(dim), unitTable.nodes(dim, tg.mi[dim]));
      BOOST_CHECK_EQUAL(unitTG.tensorGrid.nGPInDim(dim), tg.tensorGrid.nGPInDim(dim));
    }
  }
}

/// Generation instruction with per-dimension nodes (equi / cc / cheb1 by the letters of
/// @p families: 'e', 'c', 'h'), domain and boundary level offset.
CompleteSGGenInstr makeGenInstr(const LvlType maxLvl, const std::string& families,
                                const HyperCubeArea& domain, const LvlType boundary) {
  CompleteSGGenInstr genInstr(maxLvl, families.size());
  for (size_t dim = 0; dim < families.size(); dim++) {
    genInstr.setNodeFamilyForDim(families[dim] == 'e'   ? getEquidistantNodes()
                                 : families[dim] == 'c' ? getClenshawCurtisNodes()
                                                        : getFirstTypeChebyshevNodes(),
                                 dim);
  }
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setDomain(domain);
  genInstr.setBoundaryLevelOffset(boundary);
  return genInstr;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(sparse_grid_storage)

BOOST_AUTO_TEST_CASE(Node_Table_Layout) {
  // Two rows of dimension 0 (3 and 0 nodes), dimension 1 shares them, dimension 2 has its own
  NodeTable table({0, 0, 2}, {1, 0, 1}, 4);
  const size_t sizes[] = {3, 0, 1, 2};
  for (size_t row = 0; row < 4; row++) table.rowNodes(row).resize(sizes[row]);
  BOOST_CHECK_EQUAL(table.nDim(), 3u);
  BOOST_CHECK_EQUAL(table.nRows(), 4u);
  BOOST_CHECK_EQUAL(table.nStoredNodes(), 6u);
  BOOST_CHECK(table.sharesRowsWith(0, 1));
  BOOST_CHECK(!table.sharesRowsWith(0, 2));
  BOOST_CHECK_EQUAL(table.nNodes(0, 0), 3u);
  BOOST_CHECK_EQUAL(table.nNodes(0, 1), 0u);
  BOOST_CHECK_EQUAL(table.nNodes(1, 0), 3u);
  BOOST_CHECK_EQUAL(table.nNodes(2, 0), 1u);
  BOOST_CHECK_EQUAL(table.nNodes(2, 1), 2u);

  for (size_t row = 0; row < table.nRows(); row++) {
    for (double& node : table.rowNodes(row)) node = static_cast<double>(row) + 0.5;
  }
  BOOST_CHECK_EQUAL(table.nodes(1, 0), table.nodes(0, 0));
  BOOST_CHECK_EQUAL(table.nodes(2, 1)[1], 3.5);
  BOOST_CHECK_EQUAL(table.nodeSpan(2, 0).size(), 1u);

  NodeTable copy = table;
  BOOST_CHECK(copy == table);
  copy.rowNodes(3)[0] = -0.0;
  BOOST_CHECK(!(copy == table));
}

BOOST_AUTO_TEST_CASE(Nodes_Of_Sparse_Grids) {
  const HyperCubeArea shifted{{-1.0, 2.0}, {0.5, 0.75}, {-1.0, 2.0}};
  for (const LvlType boundary : {0u, 1u, 2u}) {
    for (const std::string families : {"eee", "ccc", "hhh", "ceh", "cec"}) {
      for (const bool unit : {true, false}) {
        BOOST_TEST_CONTEXT(families << " b" << boundary << " unit " << unit) {
          const HyperCubeArea domain = unit ? HyperCubeArea(3, {0.0, 1.0}) : shifted;
          const CompleteSGGenInstr genInstr = makeGenInstr(4, families, domain, boundary);
          checkNodeTable(SparseGrid(genInstr), false);
          if (families.find('h') == std::string::npos) {
            checkNodeTable(DeltasSparseGrid(genInstr), true);
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Each_Distinct_List_Stored_Once) {
  // Dimensions 0 and 2 have the same nodes and domain interval: shared; dimension 1 another
  // family, dimension 3 the same family as 0 but another interval (shared on the unit interval)
  const HyperCubeArea domain{{0.0, 1.0}, {0.0, 1.0}, {0.0, 1.0}, {-1.0, 1.0}};
  const CompleteSGGenInstr genInstr = makeGenInstr(5, "cecc", domain, 1);
  for (const bool delta : {false, true}) {
    BOOST_TEST_CONTEXT("deltas " << delta) {
      const NodeTable table = delta ? DeltasSparseGrid(genInstr).getDomainNodeTable()
                                    : SparseGrid(genInstr).getDomainNodeTable();
      BOOST_CHECK(table.sharesRowsWith(0, 2));
      BOOST_CHECK(!table.sharesRowsWith(0, 1));
      BOOST_CHECK(!table.sharesRowsWith(0, 3));
      BOOST_CHECK(!table.sharesRowsWith(1, 3));
      BOOST_CHECK_EQUAL(table.nRows(), 3u * 6u);  // 3 distinct lists per level 0, ..., 5

      size_t nNodes = 0;
      for (const size_t dim : {0, 1, 3}) {
        for (LvlType lvl = 0; lvl <= 5; lvl++) nNodes += table.nNodes(dim, lvl);
      }
      BOOST_CHECK_EQUAL(table.nStoredNodes(), nNodes);

      const NodeTable unitTable = delta ? DeltasSparseGrid(genInstr).getUnitNodeTable()
                                        : SparseGrid(genInstr).getUnitNodeTable();
      BOOST_CHECK(unitTable.sharesRowsWith(0, 2));
      BOOST_CHECK(unitTable.sharesRowsWith(0, 3));
      BOOST_CHECK(!unitTable.sharesRowsWith(0, 1));
      BOOST_CHECK_EQUAL(unitTable.nRows(), 2u * 6u);
    }
  }
}

BOOST_AUTO_TEST_CASE(Unit_And_Domain_Tables_Group_Dimensions_Differently) {
  // One node family: all dimensions share their unit rows (up to level 4, of dimension 1); on the
  // domain, dimensions 0 and 2 share theirs (up to level 3, of dimension 2), dimension 1 has its
  // own
  const LvlMIVec mis{{1, 0, 0}, {0, 4, 0}, {0, 0, 3}};
  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeFamily(getClenshawCurtisNodes());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setDomain({{0.0, 1.0}, {-1.0, 1.0}, {0.0, 1.0}});
  genInstr.setBoundaryLevelOffset(1);
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);
  for (const NodeTable* unitTable : {&sg.getUnitNodeTable(), &dsg.getUnitNodeTable()}) {
    BOOST_CHECK(unitTable->sharesRowsWith(0, 1) && unitTable->sharesRowsWith(0, 2));
    BOOST_CHECK_EQUAL(unitTable->nRows(), 5u);
  }
  for (const NodeTable* table : {&sg.getDomainNodeTable(), &dsg.getDomainNodeTable()}) {
    BOOST_CHECK(table->sharesRowsWith(0, 2) && !table->sharesRowsWith(0, 1));
    BOOST_CHECK_EQUAL(table->nRows(), 4u + 5u);
    BOOST_CHECK_EQUAL(table->maxLvl(0), 1u);
    BOOST_CHECK_EQUAL(table->maxLvl(2), 3u);
  }
  checkNodeTable(sg, false);
  checkNodeTable(dsg, true);
}

BOOST_AUTO_TEST_CASE(Shared_Rows_Reach_The_Maximum_Level_Of_All_Sharing_Dimensions) {
  const LvlMIVec mis{{1, 4}, {3, 0}};
  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeFamily(getClenshawCurtisNodes());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  for (const bool delta : {false, true}) {
    BOOST_TEST_CONTEXT("deltas " << delta) {
      const NodeTable table = delta ? DeltasSparseGrid(genInstr).getDomainNodeTable()
                                    : SparseGrid(genInstr).getDomainNodeTable();
      BOOST_CHECK(table.sharesRowsWith(0, 1));
      BOOST_CHECK_EQUAL(table.maxLvl(0), 3u);
      BOOST_CHECK_EQUAL(table.maxLvl(1), 4u);
      BOOST_CHECK_EQUAL(table.nRows(), 5u);
      for (LvlType lvl = 0; lvl <= 3; lvl++) {
        BOOST_CHECK_EQUAL(table.nodes(0, lvl), table.nodes(1, lvl));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Struct_Of_Arrays_And_Views) {
  const HyperCubeArea domain{{-1.0, 2.0}, {0.5, 0.75}, {3.0, 4.0}};
  const CompleteSGGenInstr genInstr = makeGenInstr(4, "cec", domain, 1);
  const SparseGrid sg(genInstr);
  const auto [miVec, coeffs] = genInstr.genReducedMIVecWithCoeffs();

  // The levels and coefficients are those of the generation, in its order
  BOOST_REQUIRE_EQUAL(sg.nTG(), miVec.size());
  BOOST_REQUIRE_EQUAL(sg.getLevels().size(), miVec.size());
  BOOST_CHECK(std::ranges::equal(sg.getCoefficients(), coeffs));

  size_t maxGPCnt = 0, maxSum = 0;
  for (size_t idx = 0; idx < sg.nTG(); idx++) {
    const TensorGridCTDataView tg = sg.getDomainTensorGrid(idx);
    BOOST_CHECK(tg.mi == miVec[idx]);
    BOOST_CHECK(tg.mi == sg.getLevels()[idx]);
    BOOST_CHECK_EQUAL(tg.coefficient, coeffs[idx]);

    // View accessors vs. an owning copy
    const TensorGridCTData copy = tg.toTensorGridCTData();
    BOOST_CHECK(copy.mi == tg.mi);
    BOOST_CHECK_EQUAL(copy.coefficient, tg.coefficient);
    BOOST_CHECK(copy.tensorGrid.getGPCntPerDim() == tg.tensorGrid.getGPCntPerDim());
    BOOST_CHECK(copy.tensorGrid.getNodesPerDim() == tg.tensorGrid.getNodesPerDim());
    BOOST_CHECK_EQUAL(copy.tensorGrid.nGP(), tg.tensorGrid.nGP());
    BOOST_CHECK_EQUAL(tg.tensorGrid.sumOfGPCntsPerDim(), copy.tensorGrid.getNodesPerDim().size());
    for (size_t gp = 0; gp < tg.tensorGrid.nGP(); gp += 7) {
      BOOST_CHECK(tg.tensorGrid.getGridPoint(gp) == copy.tensorGrid.getGridPoint(gp));
    }
    BOOST_CHECK(tg == tg);
    maxGPCnt = std::max(maxGPCnt, tg.tensorGrid.nGP());
    maxSum = std::max(maxSum, tg.tensorGrid.sumOfGPCntsPerDim());
  }
  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), maxGPCnt);
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), maxSum);
}

BOOST_AUTO_TEST_CASE(Domain_And_Unit_Ranges) {
  const CompleteSGGenInstr genInstr = makeGenInstr(3, "ce", {{-1.0, 2.0}, {0.5, 0.75}}, 0);
  const DeltasSparseGrid dsg(genInstr);
  static_assert(std::random_access_iterator<SparseGridBase::const_iterator>);
  static_assert(std::ranges::random_access_range<SparseGridBase::TensorGridRange>);

  for (const bool onDomain : {true, false}) {
    BOOST_TEST_CONTEXT("on domain " << onDomain) {
      const SparseGridBase::TensorGridRange tgs =
          onDomain ? dsg.domainTensorGrids() : dsg.unitTensorGrids();
      const auto getByIdx = [&](const size_t i) {
        return onDomain ? dsg.getDomainTensorGrid(i) : dsg.getUnitTensorGrid(i);
      };
      const auto findByMI = [&](const LvlMI& mi) {
        return onDomain ? dsg.getDomainTensorGrid(mi) : dsg.getUnitTensorGrid(mi);
      };
      const NodeTable& table = onDomain ? dsg.getDomainNodeTable() : dsg.getUnitNodeTable();

      BOOST_CHECK_EQUAL(tgs.size(), dsg.nTG());
      BOOST_CHECK_EQUAL(static_cast<size_t>(std::distance(tgs.begin(), tgs.end())), dsg.nTG());
      size_t idx = 0;
      for (const auto& tg : tgs) {
        BOOST_CHECK(tg == getByIdx(idx));
        BOOST_CHECK(tgs[static_cast<std::ptrdiff_t>(idx)] == tg);
        BOOST_CHECK_EQUAL((tgs.begin() + static_cast<std::ptrdiff_t>(idx))->coefficient,
                          tg.coefficient);
        BOOST_CHECK_EQUAL(tg.tensorGrid.nodesInDim(0), table.nodes(0, tg.mi[0]));
        idx++;
      }
      BOOST_CHECK_EQUAL(idx, dsg.nTG());

      // Lookup by level multi-index: an iterator into the same range
      const auto found = findByMI(LvlMI{2, 1});
      BOOST_REQUIRE(found != tgs.end());
      BOOST_CHECK(found->mi == LvlMI({2, 1}));
      BOOST_CHECK_EQUAL(found->tensorGrid.nodesInDim(1), table.nodes(1, 1));
      BOOST_CHECK(findByMI(LvlMI{4, 0}) == tgs.end());

      // Algorithms of std::ranges on the proxies
      const auto maxIt = std::ranges::max_element(
          tgs, {}, [](const TensorGridCTDataView& tg) { return tg.tensorGrid.nGP(); });
      BOOST_CHECK_EQUAL(maxIt->tensorGrid.nGP(), dsg.getMaxTGGPCnt());
    }
  }

  // The unit variant of a tensor grid has the unit nodes of the domain variant
  const TensorGridCTDataView domainTG = dsg.getDomainTensorGrid(LvlMI{2, 1})[0];
  const TensorGridCTDataView unitTG = dsg.getUnitTensorGrid(LvlMI{2, 1})[0];
  for (size_t dim = 0; dim < 2; dim++) {
    const auto [lower, upper] = genInstr.getDomainForDim(dim);
    for (GPCntType i = 0; i < domainTG.tensorGrid.nGPInDim(dim); i++) {
      BOOST_CHECK(bitwiseEqual(
          domainTG.tensorGrid.nodesInDim(dim)[i],
          tools::mapToDomain(unitTG.tensorGrid.nodesInDim(dim)[i], lower, upper - lower)));
    }
  }
}

BOOST_AUTO_TEST_CASE(Equality) {
  const HyperCubeArea unit(2, {0.0, 1.0});
  const CompleteSGGenInstr genInstr = makeGenInstr(3, "ce", unit, 1);
  const SparseGrid sg(genInstr);
  BOOST_CHECK(sg == SparseGrid(genInstr));
  BOOST_CHECK(DeltasSparseGrid(genInstr) == DeltasSparseGrid(genInstr));

  // Another domain: other nodes
  const CompleteSGGenInstr shifted = makeGenInstr(3, "ce", {{0.0, 2.0}, {0.0, 1.0}}, 1);
  BOOST_CHECK(!(sg == SparseGrid(shifted)));

  // Another level: other tensor grids
  BOOST_CHECK(!(sg == SparseGrid(makeGenInstr(4, "ce", unit, 1))));

  // The same multi-indices in another order: equal (order-insensitive)
  const auto [miVec, coeffs] = genInstr.genFullMIVecWithCoeffs();
  LvlMIVec reversed(2, 0);
  for (size_t i = miVec.size(); i-- > 0;) reversed.push_back(miVec[i]);
  MIVecSGGenInstr reversedInstr(reversed);
  reversedInstr.setNodeFamilies(genInstr.getNodeFamilies());
  reversedInstr.setLvl2GPCntFuncs(genInstr.getLvl2GPCntFuncs());
  reversedInstr.setBoundaryLevelOffset(1);
  BOOST_CHECK(sg == SparseGrid(reversedInstr));
}

BOOST_AUTO_TEST_CASE(Delta_Node_Counts) {
  // The rows of a deltas sparse grid hold the delta nodes of each level, incl. the boundary
  // nodes at the boundary level offset
  for (const LvlType boundary : {0u, 1u, 3u}) {
    const CompleteSGGenInstr genInstr =
        makeGenInstr(4, "e", HyperCubeArea(1, {0.0, 1.0}), boundary);
    const DeltasSparseGrid dsg(genInstr);
    const NodeTable& table = dsg.getDomainNodeTable();
    size_t nNodes = 0;
    for (LvlType lvl = 0; lvl <= 4; lvl++) {
      BOOST_CHECK_EQUAL(table.nNodes(0, lvl),
                        tools::getDeltaNodeCnt(lvl, doublingLvl2GPCntFunction, lvl == boundary));
      nNodes += table.nNodes(0, lvl);
    }
    // Nested nodes: the delta nodes of all levels are the nodes of the finest level
    BOOST_CHECK_EQUAL(nNodes, doublingLvl2GPCntFunction(4) + (boundary <= 4 ? 2 : 0));
  }
}

BOOST_AUTO_TEST_SUITE_END()
