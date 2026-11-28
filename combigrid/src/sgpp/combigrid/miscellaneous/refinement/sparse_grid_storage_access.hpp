// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_storage_access.hpp
 * @brief Write access to the storage of a sparse grid for the dimension-adaptive refinement.
 */
#pragma once

#include <cstdint>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Write access to the storage of a @c SparseGridBase (a friend of it), used by the
 * dimension-adaptive refinement to grow a grid in place.
 */
struct SparseGridStorageAccess {
  /// @brief Returns the process-wide unique number of the grid's current storage.
  static uint64_t storageVersion(const SparseGridBase& grid) noexcept;

  /// @brief Drops the grid's cross-call operator data (it refers to the storage being changed).
  static void clearCrossCallOperatorData(const SparseGridBase& grid);

  /**
   * @brief Replaces the grid's storage (new storage version, recomputed meta values).
   * @param grid           The grid.
   * @param levels         Level multi-indices of the tensor grids.
   * @param coefficients   Their combination coefficients.
   * @param unitNodeTable  Node table on the unit interval.
   * @param domainNodeTable Node table on the domain.
   */
  static void replaceStorage(SparseGridBase& grid, LvlMIVec levels,
                             std::vector<CTCoeffType> coefficients, NodeTable unitNodeTable,
                             NodeTable domainNodeTable);
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
