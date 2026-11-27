// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstdint>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/miscellaneous/refinement/sparse_grid_storage_access.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

uint64_t SparseGridStorageAccess::storageVersion(const SparseGridBase& grid) noexcept {
  return grid.storageVersion_;
}

NodeTable& SparseGridStorageAccess::unitNodeTable(SparseGridBase& grid) noexcept {
  return grid.unitNodeTable_;
}

NodeTable& SparseGridStorageAccess::domainNodeTable(SparseGridBase& grid) noexcept {
  return grid.domainNodeTable_;
}

void SparseGridStorageAccess::clearCrossCallOperatorData(const SparseGridBase& grid) {
  grid.clearCrossCallOperatorDataCache();
}

void SparseGridStorageAccess::replaceStorage(SparseGridBase& grid, LvlMIVec levels,
                                             std::vector<CTCoeffType> coefficients,
                                             NodeTable unitNodeTable, NodeTable domainNodeTable) {
  grid.setStorage(std::move(levels), std::move(coefficients), std::move(domainNodeTable),
                  std::move(unitNodeTable));
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
