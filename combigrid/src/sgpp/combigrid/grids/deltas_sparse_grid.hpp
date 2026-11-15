// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid.hpp
 * @brief Deltas sparse grid: the grid points of a combination-technique sparse grid, partitioned
 * into delta tensor grids, for operators that evaluate every grid point once.
 */
#pragma once

#include <memory>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Sparse grid stored as the disjoint union of its delta tensor grids, for nested nodes.
 *
 * Describes the same combination @f$\sum_{\vec{\ell} \in I} c_{\vec{\ell}}\,
 * f_{\vec{\ell}}@f$ as the @c SparseGrid of the same generation instruction, but stores for every
 * level multi-index @f$\vec{\ell}@f$ of the downwards closure of @f$I@f$ only the grid points that
 * are new at @f$\vec{\ell}@f$: the tensor product of the 1D delta nodes (the nodes of level
 * @f$\ell_k@f$ that are not nodes of level @f$\ell_k - 1@f$; the boundary nodes belong to the
 * boundary level offset). With nested nodes every grid point of the sparse grid lies in exactly
 * one delta tensor grid, so the operators on a @c DeltasSparseGrid (@c quadrature, @c interpolate,
 * @c interpolateLinear) evaluate the source function once per grid point (resp. once per distinct
 * corner) instead of once per tensor grid it belongs to. Their results equal those of the
 * @c SparseGrid operators up to round-off.
 *
 * Each tensor grid (a @c TensorGridCTDataView, see @c SparseGridBase) has the level multi-index,
 * the CT coefficient of that multi-index (0 for multi-indices of the closure that are not
 * combined; unlike @c SparseGrid, such tensor grids are kept) and the delta tensor grid. The node
 * tables hold the delta nodes of every (dimension, level), on the unit interval and on the
 * domain; for nested nodes, the rows of a dimension together are the nodes of its finest level,
 * each stored once. The generation instruction is copied (@c clone); the copy of a
 * @c MIVecSGGenInstr references the caller's @c LvlMIVec, which must outlive the grid.
 */
class DeltasSparseGrid : public SparseGridBase {
 public:
  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the deltas sparse grid of a generation instruction.
   *
   * @param genInstruction Generation instruction (copied); its nodes must be nested in every
   *                       dimension.
   * @throws std::invalid_argument if the nodes are not nested in some dimension (see
   * @c SGGenInstr::areNodesOfAllDimsNested).
   */
  DeltasSparseGrid(const SGGenInstr& genInstruction);

  /// @}

  /// @name Getters
  /// @{

  /// @brief Returns the grid's copy of the generation instruction.
  const SGGenInstr& getGenInstr() const noexcept;

  /// @}

 protected:
  /// @name Attributes
  /// @{

  const std::unique_ptr<SGGenInstr> genInstr;  ///< Generation instruction that produced this SG.

  /// @}
};

}  // namespace combigrid
}  // namespace sgpp
