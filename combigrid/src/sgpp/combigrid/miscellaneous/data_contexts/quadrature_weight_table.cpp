// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <atomic>
#include <cstddef>
#include <mutex>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

QuadWeightTable::QuadWeightTable(const SparseGrid& sparseGrid)
    : genInstr_(sparseGrid.getGenInstr()),
      unitNodeTable_(sparseGrid.getUnitNodeTable()),
      rows_(unitNodeTable_.nRows()),
      published_(unitNodeTable_.nRows()),
      onceFlags_(unitNodeTable_.nRows()) {}

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
