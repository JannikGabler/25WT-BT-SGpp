// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_weight_table.hpp
 * @brief The 1D quadrature weights of a sparse grid, one list per row of its unit node table,
 * computed on first use and shared by all threads of a quadrature call.
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <mutex>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief The 1D quadrature weights of the (dimension, level) pairs of a @c SparseGrid.
 *
 * The weights of a tensor grid in dimension @f$k@f$ depend only on the quadrature rule of
 * dimension @f$k@f$ and the number of nodes of level @f$\ell_k@f$, i.e. on the row of the unit
 * node table of the sparse grid (@c SparseGridBase::getUnitNodeTable): dimensions sharing their
 * rows have the same node-generation function and node counts. The table therefore holds one
 * weight list per row, @c getQuadRule(n)->genWeightsInplace for the @c n nodes of the row
 * (bitwise the weights of the rule), on the unit interval (the domain volume is applied by the
 * operator).
 *
 * A row is computed on its first use (@ref weights), by the thread that needs it first, which
 * also allocates it; threads that need it meanwhile wait for it. Rows that no tensor grid uses are
 * never computed, and a row that is expensive to compute (Clenshaw-Curtis weights by FFT for many
 * nodes) delays only the threads that need it. @ref weights may be called concurrently.
 */
class QuadWeightTable {
 public:
  /**
   * @brief Constructs the table of @p sparseGrid with all rows not yet computed.
   * @param sparseGrid Sparse grid; must outlive the table.
   */
  explicit QuadWeightTable(const SparseGrid& sparseGrid);

  QuadWeightTable(const QuadWeightTable&) = delete;
  QuadWeightTable& operator=(const QuadWeightTable&) = delete;

  /**
   * @brief Returns the first of the 1D quadrature weights of (dimension @p dim, level @p lvl),
   * one per node of the level (in the order of the nodes); computes them on the first call.
   * Thread-safe.
   */
  const double* weights(size_t dim, LvlType lvl) const;

 private:
  /// @brief Computes the weights of (dimension @p dim, level @p lvl) once (see @ref weights) and
  /// returns them.
  const double* computeRow(size_t dim, LvlType lvl) const;

  const SGGenInstr& genInstr_;      ///< Generation instruction (node-generation functions).
  const NodeTable& unitNodeTable_;  ///< Unit node table of the sparse grid (rows, node counts).

  /// @brief Per row: its weights (empty until computed).
  mutable std::vector<std::vector<double>> rows_;
  /// @brief Per row: the start of its weights once they are computed, else @c nullptr (lets
  /// @ref weights skip @ref onceFlags_ once a row is there).
  mutable std::vector<std::atomic<const double*>> published_;
  /// @brief Per row: guards its computation.
  mutable std::vector<std::once_flag> onceFlags_;
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
