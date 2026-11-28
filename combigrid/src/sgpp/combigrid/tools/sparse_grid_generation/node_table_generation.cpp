// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <numeric>
#include <sgpp/combigrid/grids/node_row_key.hpp>
#include <sgpp/combigrid/grids/node_storage.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {
namespace node_table_generation {

namespace {

/// Maximum level per dimension over @p levels (0 for an empty set).
std::vector<LvlType> compMaxLvls(const LvlMIVec& levels) {
  if (levels.size() == 0) {
    return std::vector<LvlType>(levels.nDim(), 0);
  }
  const LvlMI& maxLvls = *levels.componentWiseMax();
  return std::vector<LvlType>(maxLvls.begin(), maxLvls.end());
}

/**
 * Sets the rows: the dimensions that share their lists with an earlier one (@p sameNodesAs) get
 * its rows, the others new rows, up to the maximum level of the group of dimensions. Sets
 * @p firstRowOfDim and returns, per row, its level and the dimension of the group with the highest
 * maximum level (the group's lists are identical; this dimension has all levels of the rows).
 */
std::vector<std::pair<size_t, LvlType>> layOutRows(const std::vector<size_t>& sameNodesAs,
                                                   const std::vector<LvlType>& maxLvls,
                                                   std::vector<size_t>& firstRowOfDim) {
  const size_t nDim = maxLvls.size();
  std::vector<size_t> deepestDim(nDim);  // Per first dimension of a group
  std::iota(deepestDim.begin(), deepestDim.end(), size_t{0});
  for (size_t dim = 0; dim < nDim; dim++) {
    size_t& deepest = deepestDim[sameNodesAs[dim]];
    if (maxLvls[dim] > maxLvls[deepest]) deepest = dim;
  }

  std::vector<std::pair<size_t, LvlType>> rows;
  firstRowOfDim.assign(nDim, 0);
  for (size_t dim = 0; dim < nDim; dim++) {
    if (sameNodesAs[dim] == dim) {
      firstRowOfDim[dim] = rows.size();
      const size_t deepest = deepestDim[dim];
      for (LvlType lvl = 0; lvl <= maxLvls[deepest]; lvl++) rows.emplace_back(deepest, lvl);
    } else {
      firstRowOfDim[dim] = firstRowOfDim[sameNodesAs[dim]];
    }
  }
  return rows;
}

/// The node table of the levels 0, ..., @p maxLvls[dim] (unit or domain layout and nodes).
NodeTable genTable(const SparseGridSetup& setup, std::vector<LvlType> maxLvls,
                   const bool deltaNodes, const bool onDomain) {
  std::vector<size_t> firstRowOfDim;
  const std::vector<std::pair<size_t, LvlType>> rowLevels =
      layOutRows(findDimsWithSameNodes(setup, onDomain), maxLvls, firstRowOfDim);
  NodeTable table(std::move(firstRowOfDim), std::move(maxLvls), rowLevels.size());

  std::vector<size_t> tableRows(rowLevels.size());
  std::iota(tableRows.begin(), tableRows.end(), size_t{0});
  setRows(setup, deltaNodes, onDomain, tableRows, rowLevels, table);
  return table;
}

}  // namespace

/**
 * The rows come from the node storage of the setup (generated there in parallel if missing); the
 * domain rows are mapped from the unit rows, which the unit table holds by then.
 */
std::pair<NodeTable, NodeTable> genNodeTables(const SparseGridSetup& setup, const LvlMIVec& levels,
                                              const bool deltaNodes) {
  assert(levels.nDim() == setup.nDim());
  const std::vector<LvlType> maxLvls = compMaxLvls(levels);
  NodeTable unitTable = genTable(setup, maxLvls, deltaNodes, false);
  NodeTable domainTable = genTable(setup, maxLvls, deltaNodes, true);
  return {std::move(unitTable), std::move(domainTable)};
}

NodeTable genEmptyNodeTable(const SparseGridSetup& setup, std::vector<LvlType> maxLvls,
                            const bool sameDomain) {
  assert(maxLvls.size() == setup.nDim());
  std::vector<size_t> firstRowOfDim;
  const size_t nRows =
      layOutRows(findDimsWithSameNodes(setup, sameDomain), maxLvls, firstRowOfDim).size();
  return NodeTable(std::move(firstRowOfDim), std::move(maxLvls), nRows);
}

/**
 * One request to the storage for all rows; a row is the node list with the boundary nodes, the
 * table views it without them where the level has none.
 */
void setRows(const SparseGridSetup& setup, const bool deltaNodes, const bool onDomain,
             const std::span<const size_t> tableRows,
             const std::span<const std::pair<size_t, LvlType>> rowLevels, NodeTable& table) {
  assert(tableRows.size() == rowLevels.size());
  std::vector<NodeRowKey> keys(rowLevels.size());
  for (size_t r = 0; r < rowLevels.size(); r++) {
    keys[r] = rowKey(setup, rowLevels[r].first, rowLevels[r].second, deltaNodes, onDomain);
  }
  std::vector<NodeStorage::RowPtr> rows = setup.getNodeStorage()->getRows(keys);

  for (size_t r = 0; r < rows.size(); r++) {
    const std::span<const double> all(rows[r]->data(), rows[r]->size());
    const bool boundary = includesBoundary(setup, rowLevels[r].second, deltaNodes);
    table.setRow(tableRows[r], std::move(rows[r]), boundary ? all : all.subspan(1, all.size() - 2));
  }
}

NodeRowKey rowKey(const SparseGridSetup& setup, const size_t dim, const LvlType lvl,
                  const bool deltaNodes, const bool onDomain) {
  const NodeFamily* const nodeFamily = setup.getNodeFamilyForDim(dim);
  const Lvl2GPCntFunc lvl2GPCntFunc = setup.getLvl2GPCntFuncForDim(dim);
  const NodeRowKey key = deltaNodes ? NodeRowKey::delta(nodeFamily, lvl2GPCntFunc, lvl)
                                    : NodeRowKey::regular(nodeFamily, lvl2GPCntFunc(lvl));
  if (!onDomain) return key;
  const std::pair<double, double> interval = setup.getDomainForDim(dim);
  return key.onInterval(interval.first, interval.second);
}

bool includesBoundary(const SparseGridSetup& setup, const LvlType lvl, const bool deltaNodes) {
  const LvlType boundaryLvlOffset = setup.getBoundaryLevelOffset();
  return deltaNodes ? lvl == boundaryLvlOffset : lvl >= boundaryLvlOffset;
}

std::vector<size_t> findDimsWithSameNodes(const SparseGridSetup& setup, const bool sameDomain) {
  const size_t nDim = setup.nDim();
  std::vector<size_t> result(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    result[dim] = dim;
    for (size_t other = 0; other < dim; other++) {
      if (setup.getNodeFamilyForDim(other) == setup.getNodeFamilyForDim(dim) &&
          setup.getLvl2GPCntFuncForDim(other) == setup.getLvl2GPCntFuncForDim(dim) &&
          (!sameDomain || setup.getDomainForDim(other) == setup.getDomainForDim(dim))) {
        result[dim] = other;
        break;
      }
    }
  }

  return result;
}

GPCntType compNodeCnt(const SparseGridSetup& setup, const size_t dim, const LvlType lvl,
                      const bool deltaNodes) {
  const Lvl2GPCntFunc lvl2GPCntFunc = setup.getLvl2GPCntFuncForDim(dim);
  const LvlType boundaryLvlOffset = setup.getBoundaryLevelOffset();

  if (deltaNodes) {
    return getDeltaNodeCnt(lvl, lvl2GPCntFunc, lvl == boundaryLvlOffset);
  }
  return lvl2GPCntFunc(lvl) + (lvl >= boundaryLvlOffset ? 2 : 0);
}

}  // namespace node_table_generation
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
