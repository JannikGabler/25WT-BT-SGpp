// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <numeric>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
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

/// Writes the unit nodes (delta or regular) of (dimension @p dim, level @p lvl) to @p out.
void genRow(const SparseGridSetup& setup, const size_t dim, const LvlType lvl,
            const bool deltaNodes, base::DataVector& out) {
  const NodeGenFunc* const nodeGenFunc = setup.getNodeGenFuncForDim(dim);
  const LvlType boundaryLvlOffset = setup.getBoundaryLevelOffset();
  const GPCntType nNodes = compNodeCnt(setup, dim, lvl, deltaNodes);
  out.resize(nNodes);

  if (deltaNodes) {
    nodeGenFunc->genDeltaNodes(lvl, setup.getLvl2GPCntFuncForDim(dim), std::span<double>(out),
                               lvl == boundaryLvlOffset);
  } else {
    nodeGenFunc->genNodes(nNodes, out, lvl >= boundaryLvlOffset);
  }
}

}  // namespace

/**
 * Lays out the rows, then generates them in parallel (the largest rows dominate, so the rows are
 * scheduled dynamically). Every row is allocated by the thread that generates it: a single buffer
 * of all nodes would be zero-filled serially (2D, level 20, 16 threads: 81 ms instead of 22 ms).
 */
NodeTable genUnitNodeTable(const SparseGridSetup& setup, const LvlMIVec& levels,
                           const bool deltaNodes) {
  assert(levels.nDim() == setup.nDim());

  std::vector<LvlType> maxLvls = compMaxLvls(levels);
  std::vector<size_t> firstRowOfDim;
  const std::vector<std::pair<size_t, LvlType>> rows =
      layOutRows(findDimsWithSameNodes(setup, false), maxLvls, firstRowOfDim);

  size_t nNodes = 0;
  for (const auto& [dim, lvl] : rows) nNodes += compNodeCnt(setup, dim, lvl, deltaNodes);
  const bool parallel = nNodes >= constants::sparse_grid_storage::MIN_NODES_FOR_CONCURRENCY;

  NodeTable table(std::move(firstRowOfDim), std::move(maxLvls), rows.size());

#pragma omp parallel for schedule(dynamic) if (parallel)
  for (size_t r = 0; r < rows.size(); r++) {
    genRow(setup, rows[r].first, rows[r].second, deltaNodes, table.rowNodes(r));
  }

  return table;
}

/**
 * The domain table has rows of its own (dimensions with the same unit lists but different domain
 * intervals share unit rows only), each one the mapped copy of the unit row of its (dimension,
 * level); in parallel as the generation of the unit table.
 */
NodeTable mapOntoDomain(const SparseGridSetup& setup, const NodeTable& unitNodeTable) {
  const size_t nDim = unitNodeTable.nDim();
  assert(setup.nDim() == nDim);

  std::vector<LvlType> maxLvls(nDim);
  for (size_t dim = 0; dim < nDim; dim++) maxLvls[dim] = unitNodeTable.maxLvl(dim);
  std::vector<size_t> firstRowOfDim;
  const std::vector<std::pair<size_t, LvlType>> rows =
      layOutRows(findDimsWithSameNodes(setup, true), maxLvls, firstRowOfDim);

  size_t nNodes = 0;
  for (const auto& [dim, lvl] : rows) nNodes += unitNodeTable.nNodes(dim, lvl);
  const bool parallel = nNodes >= constants::sparse_grid_storage::MIN_NODES_FOR_CONCURRENCY;

  NodeTable table(std::move(firstRowOfDim), std::move(maxLvls), rows.size());

#pragma omp parallel for schedule(dynamic) if (parallel)
  for (size_t r = 0; r < rows.size(); r++) {
    const auto [dim, lvl] = rows[r];
    genDomainRow(setup, dim, unitNodeTable.nodeSpan(dim, lvl), table.rowNodes(r));
  }

  return table;
}

NodeTable genEmptyNodeTable(const SparseGridSetup& setup, std::vector<LvlType> maxLvls,
                            const bool sameDomain) {
  assert(maxLvls.size() == setup.nDim());
  std::vector<size_t> firstRowOfDim;
  const size_t nRows =
      layOutRows(findDimsWithSameNodes(setup, sameDomain), maxLvls, firstRowOfDim).size();
  return NodeTable(std::move(firstRowOfDim), std::move(maxLvls), nRows);
}

void genUnitRow(const SparseGridSetup& setup, const size_t dim, const LvlType lvl,
                base::DataVector& out) {
  genRow(setup, dim, lvl, false, out);
}

void genDomainRow(const SparseGridSetup& setup, const size_t dim,
                  const std::span<const double> unitNodes, base::DataVector& out) {
  const std::pair<double, double> interval = setup.getDomainForDim(dim);
  const double width = interval.second - interval.first;
  out.resize(unitNodes.size());
  for (size_t i = 0; i < unitNodes.size(); i++) {
    out[i] = mapToDomain(unitNodes[i], interval.first, width);
  }
}

std::vector<size_t> findDimsWithSameNodes(const SparseGridSetup& setup, const bool sameDomain) {
  const size_t nDim = setup.nDim();
  std::vector<size_t> result(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    result[dim] = dim;
    for (size_t other = 0; other < dim; other++) {
      if (setup.getNodeGenFuncForDim(other) == setup.getNodeGenFuncForDim(dim) &&
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
