// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/refinement_node_storage.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

using sgpp::combigrid::tools::node_table_generation::genDomainRow;
using sgpp::combigrid::tools::node_table_generation::genEmptyNodeTable;
using sgpp::combigrid::tools::node_table_generation::genUnitRow;

namespace sgpp {
namespace combigrid {

namespace misc {

RefinementNodeStorage::RefinementNodeStorage(const SparseGridSetup& setup, const LvlType capacity)
    : setup_(setup),
      capacity_(capacity),
      unit_(genEmptyNodeTable(setup, std::vector<LvlType>(setup.nDim(), capacity), false)),
      domain_(genEmptyNodeTable(setup, std::vector<LvlType>(setup.nDim(), capacity), true)),
      unitPresent_(unit_.nRows(), 0),
      domainPresent_(domain_.nRows(), 0) {}

void RefinementNodeStorage::adopt(NodeTable& unitTable, NodeTable& domainTable) {
  assert(unitTable.nDim() == unit_.nDim() && domainTable.nDim() == domain_.nDim());
  moveRows(unitTable, unit_);
  moveRows(domainTable, domain_);
  for (size_t dim = 0; dim < unitTable.nDim(); dim++) {
    for (LvlType lvl = 0; lvl <= unitTable.maxLvl(dim); lvl++) {
      unitPresent_[unit_.row(dim, lvl)] = 1;
      domainPresent_[domain_.row(dim, lvl)] = 1;
    }
  }
}

void RefinementNodeStorage::giveBack(NodeTable& unitTable, NodeTable& domainTable) {
  moveRows(unit_, unitTable);
  moveRows(domain_, domainTable);
  clearPresence();
}

void RefinementNodeStorage::ensureRows(const LvlMIView levels) {
  assert(levels.size() == unit_.nDim());

  for (size_t dim = 0; dim < levels.size(); dim++) {
    const LvlType lvl = levels[dim];
    assert(lvl <= capacity_);
    const size_t unitRow = unit_.row(dim, lvl);
    if (!unitPresent_[unitRow]) {
      genUnitRow(setup_, dim, lvl, unit_.rowNodes(unitRow));
      unitPresent_[unitRow] = 1;
    }
    const size_t domainRow = domain_.row(dim, lvl);
    if (!domainPresent_[domainRow]) {
      genDomainRow(setup_, dim, unit_.nodeSpan(dim, lvl), domain_.rowNodes(domainRow));
      domainPresent_[domainRow] = 1;
    }
  }
}

std::pair<NodeTable, NodeTable> RefinementNodeStorage::release(
    const std::vector<LvlType>& maxLvls) {
  NodeTable unit = genEmptyNodeTable(setup_, maxLvls, false);
  NodeTable domain = genEmptyNodeTable(setup_, maxLvls, true);
#ifndef NDEBUG
  for (size_t dim = 0; dim < maxLvls.size(); dim++) {
    for (LvlType lvl = 0; lvl <= maxLvls[dim]; lvl++) {
      assert(unitPresent_[unit_.row(dim, lvl)] && domainPresent_[domain_.row(dim, lvl)]);
    }
  }
#endif
  moveRows(unit_, unit);
  moveRows(domain_, domain);
  clearPresence();
  return {std::move(unit), std::move(domain)};
}

/**
 * Rows of @p to are visited once (dimensions may share them); the levels are those both tables
 * have, i.e. the levels of the smaller one.
 */
void RefinementNodeStorage::moveRows(NodeTable& from, NodeTable& to) {
  assert(from.nDim() == to.nDim());
  std::vector<uint8_t> moved(to.nRows(), 0);
  for (size_t dim = 0; dim < to.nDim(); dim++) {
    const LvlType maxLvl = std::min(from.maxLvl(dim), to.maxLvl(dim));
    for (LvlType lvl = 0; lvl <= maxLvl; lvl++) {
      const size_t toRow = to.row(dim, lvl);
      if (!moved[toRow]) {
        to.rowNodes(toRow).swap(from.rowNodes(from.row(dim, lvl)));
        moved[toRow] = 1;
      }
    }
  }
}

void RefinementNodeStorage::clearPresence() {
  unitPresent_.assign(unitPresent_.size(), 0);
  domainPresent_.assign(domainPresent_.size(), 0);
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
