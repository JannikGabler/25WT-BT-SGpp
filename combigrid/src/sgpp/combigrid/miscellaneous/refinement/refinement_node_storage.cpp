// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_node_storage.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

using sgpp::combigrid::tools::node_table_generation::genEmptyNodeTable;
using sgpp::combigrid::tools::node_table_generation::setRows;

namespace sgpp {
namespace combigrid {

namespace misc {

RefinementNodeStorage::RefinementNodeStorage(const SparseGridSetup& setup, const LvlType capacity)
    : setup_(setup),
      capacity_(capacity),
      unit_(genEmptyNodeTable(setup, std::vector<LvlType>(setup.nDim(), capacity), false)),
      domain_(genEmptyNodeTable(setup, std::vector<LvlType>(setup.nDim(), capacity), true)) {}

void RefinementNodeStorage::adopt(const NodeTable& unitTable, const NodeTable& domainTable) {
  assert(unitTable.nDim() == unit_.nDim() && domainTable.nDim() == domain_.nDim());
  copyRows(unitTable, unit_);
  copyRows(domainTable, domain_);
}

void RefinementNodeStorage::ensureRows(const LvlMIView levels) {
  assert(levels.size() == unit_.nDim());
  setMissingRows(levels, false, unit_);
  setMissingRows(levels, true, domain_);
}

std::pair<NodeTable, NodeTable> RefinementNodeStorage::release(
    const std::vector<LvlType>& maxLvls) const {
  NodeTable unit = genEmptyNodeTable(setup_, maxLvls, false);
  NodeTable domain = genEmptyNodeTable(setup_, maxLvls, true);
  copyRows(unit_, unit);
  copyRows(domain_, domain);
#ifndef NDEBUG
  for (size_t row = 0; row < unit.nRows(); row++) assert(unit.rowOwner(row) != nullptr);
  for (size_t row = 0; row < domain.nRows(); row++) assert(domain.rowOwner(row) != nullptr);
#endif
  return {std::move(unit), std::move(domain)};
}

/**
 * Rows of @p to are visited once per (dimension, level); dimensions sharing a row set it to the
 * same list. The levels are those both tables have, i.e. the levels of the smaller one.
 */
void RefinementNodeStorage::copyRows(const NodeTable& from, NodeTable& to) {
  assert(from.nDim() == to.nDim());
  for (size_t dim = 0; dim < to.nDim(); dim++) {
    const LvlType maxLvl = std::min(from.maxLvl(dim), to.maxLvl(dim));
    for (LvlType lvl = 0; lvl <= maxLvl; lvl++) {
      const size_t fromRow = from.row(dim, lvl);
      if (from.rowOwner(fromRow) == nullptr) continue;
      to.setRow(to.row(dim, lvl), from.rowOwner(fromRow), from.rowSpan(fromRow));
    }
  }
}

void RefinementNodeStorage::setMissingRows(const LvlMIView levels, const bool onDomain,
                                           NodeTable& table) const {
  std::vector<size_t> tableRows;
  std::vector<std::pair<size_t, LvlType>> rowLevels;
  for (size_t dim = 0; dim < levels.size(); dim++) {
    assert(levels[dim] <= capacity_);
    const size_t row = table.row(dim, levels[dim]);
    if (table.rowOwner(row) != nullptr || std::ranges::find(tableRows, row) != tableRows.end()) {
      continue;
    }
    tableRows.push_back(row);
    rowLevels.emplace_back(dim, levels[dim]);
  }
  if (!tableRows.empty()) setRows(setup_, false, onDomain, tableRows, rowLevels, table);
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
