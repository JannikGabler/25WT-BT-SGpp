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
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/collections/vector_bytes.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

QuadWeightTable::QuadWeightTable(const SparseGrid& sparseGrid)
    : genInstr_(sparseGrid.getGenInstr()),
      unitNodeTable_(sparseGrid.getUnitNodeTable()),
      rowsBySize_(unitNodeTable_.nRows()),
      rows_(unitNodeTable_.nRows()),
      published_(unitNodeTable_.nRows()),
      onceFlags_(unitNodeTable_.nRows()) {
  // Every row belongs to at least one (dimension, level) pair; dimensions sharing rows overwrite
  // each other's entries with equivalent ones
  for (size_t dim = 0; dim < unitNodeTable_.nDim(); dim++) {
    for (LvlType lvl = 0; lvl <= unitNodeTable_.maxLvl(dim); lvl++) {
      rowsBySize_[unitNodeTable_.row(dim, lvl)] = {dim, lvl};
    }
  }

  std::ranges::stable_sort(rowsBySize_, std::ranges::greater{}, [this](const auto& dimLvl) {
    return unitNodeTable_.nNodes(dimLvl.first, dimLvl.second);
  });
}

void QuadWeightTable::computeAllRows() const {
#pragma omp for schedule(dynamic, 1) nowait
  for (size_t i = 0; i < rowsBySize_.size(); i++) {
    weights(rowsBySize_[i].first, rowsBySize_[i].second);
  }
}

size_t QuadWeightTable::memoryBytes() const noexcept {
  size_t bytes = sizeof(*this) + tools::vectorBytes(rowsBySize_) + tools::vectorBytes(rows_) +
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
  // visible to them. Dimensions share a row only if they have the same node-generation function
  // (and node counts), so the weights do not depend on the dimension that computes them.
  std::call_once(onceFlags_[row], [this, dim, lvl, row]() {
    const GPCntType nNodes = unitNodeTable_.nNodes(dim, lvl);
    std::vector<double>& weights = rows_[row];

    weights.resize(nNodes);
    genInstr_.getNodeGenFuncForDim(dim)->getQuadRule(nNodes)->genWeightsInplace(weights);
    published_[row].store(weights.data(), std::memory_order_release);
  });

  return rows_[row].data();
}

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
