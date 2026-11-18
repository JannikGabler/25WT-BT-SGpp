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
#include <utility>
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
 * A row is computed once, by the thread that requests it first, which also allocates it; threads
 * that need it meanwhile wait for it. Rows are requested on their first use (@ref weights) or all
 * at once by the threads of a parallel region (@ref computeAllRows), the largest first, so that
 * the most expensive rows (Clenshaw-Curtis weights by FFT for many nodes) start before the tensor
 * grids that need them. @ref weights and @ref computeAllRows may be called concurrently; the
 * weights do not depend on the thread or the order that computes them.
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

  /**
   * @brief Computes all rows not computed yet, the rows with the most nodes first, distributed
   * dynamically over the threads of the innermost enclosing OpenMP parallel region.
   *
   * An orphaned worksharing loop without barrier: every thread of the region must call it (outside
   * a parallel region, the calling thread computes all rows). A thread returns once every row is
   * computed or being computed by another thread; @ref weights then waits for the latter.
   *
   * Used by the quadrature on a @c SparseGrid before its tensor-grid loop.
   */
  void computeAllRows() const;

  /**
   * @brief Returns the memory of the table in bytes once all rows are computed (the object, its
   * per-row bookkeeping and one double per node of every row). Independent of the rows computed
   * so far, so it may be called concurrently with @ref weights.
   */
  size_t memoryBytes() const noexcept;

 private:
  /// @brief Computes the weights of (dimension @p dim, level @p lvl) once (see @ref weights) and
  /// returns them.
  const double* computeRow(size_t dim, LvlType lvl) const;

  const SGGenInstr& genInstr_;      ///< Generation instruction (node-generation functions).
  const NodeTable& unitNodeTable_;  ///< Unit node table of the sparse grid (rows, node counts).

  /// @brief Per row, sorted by decreasing node count: a (dimension, level) pair of the row.
  std::vector<std::pair<size_t, LvlType>> rowsBySize_;

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
