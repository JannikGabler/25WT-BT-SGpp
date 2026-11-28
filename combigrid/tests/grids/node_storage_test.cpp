// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file node_storage_test.cpp
 * @brief Boost.Test cases for the shared storage of the 1D node lists (@c NodeStorage,
 * @c NodeRowKey): rows equal the node families' lists bitwise (with and without boundary nodes,
 * regular and delta nodes, mapped onto intervals), grids with the same nodes share the rows,
 * rows are freed with their last grid, grids use the storage of their setup, concurrent requests
 * and failing node families.
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/node_row_key.hpp>
#include <sgpp/combigrid/grids/node_storage.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <thread>
#include <vector>

using sgpp::base::DataVector;
using namespace sgpp::combigrid;  // NOLINT(build/namespaces)

namespace {

/// Bitwise equality of two node lists.
bool bitwiseEqual(const std::span<const double> a, const std::span<const double> b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); i++) {
    if (std::bit_cast<uint64_t>(a[i]) != std::bit_cast<uint64_t>(b[i])) return false;
  }
  return true;
}

std::span<const double> spanOf(const DataVector& v) { return {v.data(), v.size()}; }

/// Inner part of a row (without the first and the last node).
std::span<const double> inner(const DataVector& row) {
  return spanOf(row).subspan(1, row.size() - 2);
}

/// Complete sparse grid on [0, 1]^3 (or shifted) with CC nodes, offset 1, using @p storage.
CompleteSGGenInstr makeGenInstr(const std::shared_ptr<NodeStorage>& storage, const bool unitCube,
                                const LvlType maxLvl = 5) {
  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeFamily(getClenshawCurtisNodes());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);
  if (!unitCube) genInstr.setDomain({-1.0, 2.0});
  genInstr.setNodeStorage(storage);
  return genInstr;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(grids_node_storage)

BOOST_AUTO_TEST_CASE(Regular_Rows_Equal_The_Node_Families_With_And_Without_Boundary) {
  NodeStorage storage;
  for (NodeFamily* family :
       {getEquidistantNodes(), getClenshawCurtisNodes(), getFirstTypeChebyshevNodes()}) {
    for (const GPCntType nInner : {0u, 1u, 2u, 3u, 7u, 15u}) {
      BOOST_TEST_CONTEXT("inner nodes " << nInner) {
        const auto row = storage.getRow(NodeRowKey::regular(family, nInner));
        BOOST_CHECK(bitwiseEqual(spanOf(*row), spanOf(family->genNodes(nInner + 2, true))));
        BOOST_CHECK(bitwiseEqual(inner(*row), spanOf(family->genNodes(nInner, false))));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Delta_Rows_Equal_The_Node_Families_With_And_Without_Boundary) {
  NodeStorage storage;
  for (NodeFamily* family : {getEquidistantNodes(), getClenshawCurtisNodes()}) {
    for (LvlType lvl = 0; lvl <= 5; lvl++) {
      BOOST_TEST_CONTEXT("level " << lvl) {
        const auto row = storage.getRow(NodeRowKey::delta(family, doublingLvl2GPCntFunction, lvl));
        BOOST_CHECK(bitwiseEqual(
            spanOf(*row), spanOf(family->genDeltaNodes(lvl, doublingLvl2GPCntFunction, true))));
        BOOST_CHECK(bitwiseEqual(
            inner(*row), spanOf(family->genDeltaNodes(lvl, doublingLvl2GPCntFunction, false))));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Rows_On_Intervals_Are_Mapped_And_The_Unit_Interval_Is_The_Unit_Row) {
  NodeStorage storage;
  const NodeRowKey key = NodeRowKey::regular(getClenshawCurtisNodes(), 7);
  const auto unit = storage.getRow(key);
  BOOST_CHECK_EQUAL(storage.getRow(key.onInterval(0.0, 1.0)), unit);  // The same row

  const auto shifted = storage.getRow(key.onInterval(-1.0, 2.0));
  BOOST_REQUIRE_EQUAL(shifted->size(), unit->size());
  for (size_t i = 0; i < unit->size(); i++) {
    const double expected = tools::mapToDomain((*unit)[i], -1.0, 3.0);
    BOOST_CHECK_EQUAL(std::bit_cast<uint64_t>((*shifted)[i]), std::bit_cast<uint64_t>(expected));
  }
  BOOST_CHECK_EQUAL(storage.getRow(key.onInterval(-1.0, 2.0)), shifted);
  BOOST_CHECK(!(key.onInterval(-0.0, 1.0) == key));  // Intervals are compared bitwise
}

BOOST_AUTO_TEST_CASE(Grids_Share_Rows_And_Free_Them_With_The_Last_Grid) {
  const auto storage = std::make_shared<NodeStorage>();
  {
    const SparseGrid first(makeGenInstr(storage, true));
    const SparseGrid second(makeGenInstr(storage, true));
    const DeltasSparseGrid deltas(makeGenInstr(storage, false));
    for (size_t dim = 0; dim < 3; dim++) {
      for (LvlType lvl = 0; lvl <= 5; lvl++) {
        BOOST_CHECK_EQUAL(first.getUnitNodeTable().nodes(dim, lvl),
                          second.getUnitNodeTable().nodes(dim, lvl));
        // On the unit cube the domain nodes are the unit nodes
        BOOST_CHECK_EQUAL(first.getDomainNodeTable().nodes(dim, lvl),
                          first.getUnitNodeTable().nodes(dim, lvl));
      }
    }
    // Level 0 has no boundary nodes (offset 1): it views its row without the first node
    const NodeTable& table = first.getUnitNodeTable();
    BOOST_CHECK_EQUAL(table.nodes(0, 0), table.rowOwner(table.row(0, 0))->data() + 1);
    BOOST_CHECK_EQUAL(table.nodes(0, 1), table.rowOwner(table.row(0, 1))->data());
    BOOST_CHECK_GT(storage->nRows(), 0u);
    BOOST_CHECK_GT(storage->nStoredNodes(), 0u);
  }
  BOOST_CHECK_EQUAL(storage->nRows(), 0u);
  BOOST_CHECK_EQUAL(storage->nStoredNodes(), 0u);
}

BOOST_AUTO_TEST_CASE(Copies_Of_Node_Tables_Keep_Their_Rows) {
  const auto storage = std::make_shared<NodeStorage>();
  NodeTable copy;
  DataVector expected;
  {
    const SparseGrid grid(makeGenInstr(storage, false));
    copy = grid.getDomainNodeTable();
    expected = DataVector(copy.nNodes(2, 3));
    for (size_t i = 0; i < expected.size(); i++) expected[i] = copy.nodes(2, 3)[i];
  }
  BOOST_CHECK_GT(storage->nRows(), 0u);
  BOOST_CHECK(bitwiseEqual(copy.nodeSpan(2, 3), spanOf(expected)));
  copy = NodeTable();
  BOOST_CHECK_EQUAL(storage->nRows(), 0u);
}

BOOST_AUTO_TEST_CASE(Grids_Use_The_Storage_Of_Their_Setup) {
  BOOST_CHECK_EQUAL(SparseGridSetup(2).getNodeStorage(), NodeStorage::global());
  BOOST_CHECK_THROW(SparseGridSetup(2).setNodeStorage(nullptr), std::invalid_argument);

  const auto a = std::make_shared<NodeStorage>();
  const auto b = std::make_shared<NodeStorage>();
  const CompleteSGGenInstr genInstrA = makeGenInstr(a, false);
  const CompleteSGGenInstr genInstrB = makeGenInstr(b, false);
  BOOST_CHECK(static_cast<const SparseGridSetup&>(genInstrA) == genInstrB);  // Storage ignored

  const SparseGrid gridA(genInstrA);
  const SparseGrid gridB(genInstrB);
  BOOST_CHECK_EQUAL(gridA.getSetup().getNodeStorage(), a);
  BOOST_CHECK(gridA.getDomainNodeTable() == gridB.getDomainNodeTable());  // The same nodes ...
  BOOST_CHECK_NE(gridA.getDomainNodeTable().nodes(0, 2),
                 gridB.getDomainNodeTable().nodes(0, 2));  // ... in different storages
  BOOST_CHECK(gridA == gridB);
  BOOST_CHECK_EQUAL(a->nRows(), b->nRows());
}

BOOST_AUTO_TEST_CASE(Concurrent_Requests_Give_The_Same_Rows) {
  const auto storage = std::make_shared<NodeStorage>();
  constexpr size_t N_THREADS = 8;
  std::vector<std::unique_ptr<SparseGrid>> grids(N_THREADS);
  std::vector<std::unique_ptr<DeltasSparseGrid>> deltaGrids(N_THREADS);
  std::vector<std::thread> threads;
  for (size_t t = 0; t < N_THREADS; t++) {
    threads.emplace_back([&, t]() {
      grids[t] = std::make_unique<SparseGrid>(makeGenInstr(storage, t % 2 == 0, 8));
      deltaGrids[t] = std::make_unique<DeltasSparseGrid>(makeGenInstr(storage, t % 2 == 0, 8));
    });
  }
  for (std::thread& thread : threads) thread.join();

  for (size_t t = 2; t < N_THREADS; t++) {
    BOOST_TEST_CONTEXT("thread " << t) {
      const size_t same = t % 2;  // Grid of the same domain
      for (LvlType lvl = 0; lvl <= 8; lvl++) {
        BOOST_CHECK_EQUAL(grids[t]->getDomainNodeTable().nodes(1, lvl),
                          grids[same]->getDomainNodeTable().nodes(1, lvl));
        BOOST_CHECK_EQUAL(deltaGrids[t]->getUnitNodeTable().nodes(2, lvl),
                          deltaGrids[same]->getUnitNodeTable().nodes(2, lvl));
      }
      BOOST_CHECK(grids[t]->getDomainNodeTable() == grids[same]->getDomainNodeTable());
    }
  }
}

BOOST_AUTO_TEST_CASE(Failing_Node_Families_Throw_On_Every_Request) {
  NodeStorage storage;
  // First-kind Chebyshev nodes are not nested with doubling growth: no delta nodes
  const NodeRowKey key =
      NodeRowKey::delta(getFirstTypeChebyshevNodes(), doublingLvl2GPCntFunction, 2);
  BOOST_CHECK_THROW(storage.getRow(key), std::exception);
  BOOST_CHECK_THROW(storage.getRow(key), std::exception);  // Not left pending
  BOOST_CHECK_EQUAL(storage.nRows(), 0u);
  BOOST_CHECK_EQUAL(storage.getRow(NodeRowKey::regular(getFirstTypeChebyshevNodes(), 3))->size(),
                    5u);
}

BOOST_AUTO_TEST_SUITE_END()
