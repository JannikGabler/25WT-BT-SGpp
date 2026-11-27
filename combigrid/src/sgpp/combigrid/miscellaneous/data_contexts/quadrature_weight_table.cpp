// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
#include <mutex>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/tools/collections/vector_bytes.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/node_table_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

namespace {

/// Node count of a row from the growth function: equals the row's size once it is generated, so
/// rows generated after the construction get their rule too.
GPCntType nodeCnt(const SparseGridSetup& setup, const size_t dim, const LvlType lvl) {
  return tools::node_table_generation::compNodeCnt(setup, dim, lvl, false);
}

}  // namespace

QuadWeightTable::QuadWeightTable(const NodeTable& unitNodeTable, const SparseGridSetup& setup)
    : unitNodeTable_(unitNodeTable),
      rowsBySize_(unitNodeTable_.nRows()),
      rules_(unitNodeTable_.nRows()),
      rows_(unitNodeTable_.nRows()),
      published_(unitNodeTable_.nRows()),
      onceFlags_(unitNodeTable_.nRows()) {
  const LvlType boundaryLvlOffset = setup.getBoundaryLevelOffset();

  // Every row belongs to at least one (dimension, level) pair; dimensions sharing rows overwrite
  // each other's entries with equivalent ones (same node family and node counts)
  for (size_t dim = 0; dim < unitNodeTable_.nDim(); dim++) {
    for (LvlType lvl = 0; lvl <= unitNodeTable_.maxLvl(dim); lvl++) {
      const size_t row = unitNodeTable_.row(dim, lvl);
      rowsBySize_[row] = {dim, lvl};
      rules_[row] = setup.getNodeFamilyForDim(dim)->getQuadRule(nodeCnt(setup, dim, lvl),
                                                                lvl >= boundaryLvlOffset);
    }
  }

  std::ranges::stable_sort(rowsBySize_, std::ranges::greater{}, [&setup](const auto& dimLvl) {
    return nodeCnt(setup, dimLvl.first, dimLvl.second);
  });
}

QuadWeightTable::QuadWeightTable(const SparseGrid& sparseGrid)
    : QuadWeightTable(sparseGrid.getUnitNodeTable(), sparseGrid.getSetup()) {}

void QuadWeightTable::computeAllRows() const {
#pragma omp for schedule(dynamic, 1) nowait
  for (size_t i = 0; i < rowsBySize_.size(); i++) {
    weights(rowsBySize_[i].first, rowsBySize_[i].second);
  }
}

size_t QuadWeightTable::memoryBytes() const noexcept {
  size_t bytes = sizeof(*this) + tools::vectorBytes(rowsBySize_) + tools::vectorBytes(rules_) +
                 tools::vectorBytes(rows_) +
                 published_.capacity() * sizeof(std::atomic<const double*>) +
                 onceFlags_.capacity() * sizeof(std::once_flag);
  for (const auto& [dim, lvl] : rowsBySize_) {
    bytes += unitNodeTable_.nNodes(dim, lvl) * sizeof(double);
  }
  return bytes;
}

const double* QuadWeightTable::weights(const size_t dim, const LvlType lvl) const {
  const double* const published =
      published_[unitNodeTable_.row(dim, lvl)].load(std::memory_order_acquire);

  return (published != nullptr) ? published : computeRow(dim, lvl);
}

const double* QuadWeightTable::computeRow(const size_t dim, const LvlType lvl) const {
  const size_t row = unitNodeTable_.row(dim, lvl);

  // call_once lets the threads that need the row meanwhile wait for it and makes its weights
  // visible to them. The rule of the row was obtained by the constructor.
  std::call_once(onceFlags_[row], [this, dim, lvl, row]() {
    std::vector<double>& weights = rows_[row];

    weights.resize(unitNodeTable_.nNodes(dim, lvl));
    rules_[row]->genWeightsInplace(weights);
    published_[row].store(weights.data(), std::memory_order_release);
  });

  return rows_[row].data();
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
