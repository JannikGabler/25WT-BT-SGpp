// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <memory>
#include <numeric>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/unit_node_table.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
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
 * Sets the rows: the dimensions that share their lists with an earlier one get its rows (whose
 * levels then reach the maximum over these dimensions), the others new rows. Returns the
 * dimension and level of every row and sets @p firstRowOfDim.
 */
std::vector<std::pair<size_t, LvlType>> layOutRows(const std::vector<size_t>& sameNodesAs,
                                                   const std::vector<LvlType>& maxLvls,
                                                   std::vector<size_t>& firstRowOfDim) {
  const size_t nDim = maxLvls.size();
  std::vector<LvlType> rowMaxLvl(maxLvls);  // Per first dimension: maximum over its group
  for (size_t dim = 0; dim < nDim; dim++) {
    rowMaxLvl[sameNodesAs[dim]] = std::max(rowMaxLvl[sameNodesAs[dim]], maxLvls[dim]);
  }

  std::vector<std::pair<size_t, LvlType>> rows;
  firstRowOfDim.assign(nDim, 0);
  for (size_t dim = 0; dim < nDim; dim++) {
    if (sameNodesAs[dim] == dim) {
      firstRowOfDim[dim] = rows.size();
      for (LvlType lvl = 0; lvl <= rowMaxLvl[dim]; lvl++) rows.emplace_back(dim, lvl);
    } else {
      firstRowOfDim[dim] = firstRowOfDim[sameNodesAs[dim]];
    }
  }
  return rows;
}

}  // namespace

/**
 * Lays out the rows, then generates them in parallel (largest rows dominate, so the rows are
 * scheduled dynamically).
 */
NodeTable genNodeTable(const SGGenInstr& genInstr, const LvlMIVec& levels, const bool deltaNodes,
                       const bool mapOntoDomain) {
  assert(levels.nDim() == genInstr.nDim());

  std::vector<LvlType> maxLvls = compMaxLvls(levels);
  std::vector<size_t> firstRowOfDim;
  const std::vector<std::pair<size_t, LvlType>> rows =
      layOutRows(findDimsWithSameNodes(genInstr), maxLvls, firstRowOfDim);

  size_t nNodes = 0;
  for (const std::pair<size_t, LvlType>& row : rows) {
    nNodes += compNodeCnt(genInstr, row.first, row.second, deltaNodes);
  }

  NodeTable table(std::move(firstRowOfDim), std::move(maxLvls), rows.size());
  const bool parallel = nNodes >= constants::sparse_grid_storage::MIN_NODES_FOR_CONCURRENCY;

  // Every row is allocated by the thread that generates it (no serial zero fill of all nodes)
#pragma omp parallel for schedule(dynamic) if (parallel)
  for (size_t r = 0; r < rows.size(); r++) {
    genRow(genInstr, rows[r].first, rows[r].second, deltaNodes, mapOntoDomain, table, r);
  }

  return table;
}

std::vector<size_t> findDimsWithSameNodes(const SGGenInstr& genInstr) {
  const size_t nDim = genInstr.nDim();
  std::vector<size_t> result(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    result[dim] = dim;
    for (size_t other = 0; other < dim; other++) {
      if (genInstr.getNodeGenFuncForDim(other) == genInstr.getNodeGenFuncForDim(dim) &&
          genInstr.getLvl2GPCntFuncForDim(other) == genInstr.getLvl2GPCntFuncForDim(dim) &&
          genInstr.getDomainForDim(other) == genInstr.getDomainForDim(dim)) {
        result[dim] = other;
        break;
      }
    }
  }

  return result;
}

GPCntType compNodeCnt(const SGGenInstr& genInstr, const size_t dim, const LvlType lvl,
                      const bool deltaNodes) {
  const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
  const LvlType boundaryLvlOffset = genInstr.getBoundaryLevelOffset();

  if (deltaNodes) {
    return getDeltaNodeCnt(lvl, lvl2GPCntFunc, lvl == boundaryLvlOffset);
  }
  return lvl2GPCntFunc(lvl) + (lvl >= boundaryLvlOffset ? 2 : 0);
}

/**
 * The unit nodes as the tensor grids of the grids were always generated (genNodes resp.
 * genDeltaNodes of the node-generation function), then mapped onto the domain with
 * tools::mapToDomain, which all operators use: the stored nodes are bitwise the coordinates of
 * the evaluated grid points.
 */
void genRow(const SGGenInstr& genInstr, const size_t dim, const LvlType lvl, const bool deltaNodes,
            const bool mapOntoDomain, NodeTable& table, const size_t row) {
  const NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
  const LvlType boundaryLvlOffset = genInstr.getBoundaryLevelOffset();
  const GPCntType nNodes = compNodeCnt(genInstr, dim, lvl, deltaNodes);
  base::DataVector& out = table.rowNodes(row);
  out.resize(nNodes);

  if (deltaNodes) {
    nodeGenFunc->genDeltaNodes(lvl, genInstr.getLvl2GPCntFuncForDim(dim), std::span<double>(out),
                               lvl == boundaryLvlOffset);
  } else {
    nodeGenFunc->genNodes(nNodes, out, lvl >= boundaryLvlOffset);
  }

  if (mapOntoDomain) {
    const std::pair<double, double> interval = genInstr.getDomainForDim(dim);
    const double width = interval.second - interval.first;
    for (double& node : out) {
      node = mapToDomain(node, interval.first, width);
    }
  }
}

/**
 * The unit table has the layout of the grid's table: generated from the same levels (the grid's
 * level multi-indices), without the mapping.
 */
const NodeTable& getUnitNodeTable(const SparseGrid& sparseGrid) {
  return sparseGrid.getOperatorData()
      .get<misc::UnitNodeTable>([&]() {
        return std::make_unique<misc::UnitNodeTable>(misc::UnitNodeTable{
            genNodeTable(sparseGrid.getGenInstr(), sparseGrid.getLevels(), false, false)});
      })
      .table;
}

}  // namespace node_table_generation
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
