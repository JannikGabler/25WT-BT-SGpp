// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tensor_grid_view.hpp
 * @brief Read-only view of a tensor grid of a sparse grid: its levels and the shared node lists
 * of the sparse grid.
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief Read-only, non-owning view of a tensor grid of a @c SparseGrid or @c DeltasSparseGrid.
 *
 * A sparse grid stores no tensor grid objects: a tensor grid is given by its level multi-index
 * @f$\vec{\ell}@f$, and its nodes in dimension @f$k@f$ are the node list of
 * @f$(k, \ell_k)@f$ in the node table of the sparse grid (@c NodeTable), which all tensor grids
 * share. The view refers to both; it is valid as long as the sparse grid exists and is cheap to
 * copy (pass it by value).
 *
 * The nodes are those of the table: in domain coordinates (@c SparseGridBase::getDomainNodeTable,
 * @c SparseGridBase::getDomainTensorGrid) or on the unit interval
 * (@c SparseGridBase::getUnitNodeTable, @c SparseGridBase::getUnitTensorGrid). Grid points are
 * indexed as in
 * @c TensorGrid (linear index, first dimension fastest). @ref toTensorGrid copies the tensor grid
 * into a stand-alone @c TensorGrid (e.g. for its grid-point iterators).
 */
class TensorGridView {
 public:
  /**
   * @brief Constructs the view of the tensor grid of level @p lvls.
   * @param lvls  Level multi-index (the storage must outlive the view).
   * @param table Node table of the sparse grid (must outlive the view).
   */
  TensorGridView(const LvlMIView lvls, const NodeTable& table) noexcept
      : lvls_(lvls.data()),
        nDim_(lvls.size()),
        firstRowOfDim_(table.firstRowOfDim_.data()),
        rows_(table.rows_.data()) {
    assert(lvls.size() == table.nDim());
  }

  /// @name Getters
  /// @{

  /// @brief Returns the dimensionality of the grid.
  size_t nDim() const noexcept { return nDim_; }

  /// @brief Returns the level multi-index of the tensor grid.
  LvlMIView getLevels() const noexcept { return LvlMIView(lvls_, nDim_); }

  /// @brief Returns the number of grid points in dimension @p dim.
  GPCntType nGPInDim(const size_t dim) const noexcept {
    return static_cast<GPCntType>(rows_[row(dim)].size());
  }

  /// @brief Returns the first node of dimension @p dim (@ref nGPInDim "nGPInDim(dim)" nodes).
  const double* nodesInDim(const size_t dim) const noexcept { return rows_[row(dim)].data(); }

  /// @brief Returns the nodes of dimension @p dim.
  std::span<const double> nodeSpanInDim(const size_t dim) const noexcept {
    return {nodesInDim(dim), nGPInDim(dim)};
  }

  /// @brief Returns the total number of grid points (the product of the per-dimension counts; 0
  /// for d = 0, as @c TensorGrid).
  size_t nGP() const noexcept;

  /// @brief Returns the sum of the per-dimension grid-point counts.
  size_t sumOfGPCntsPerDim() const noexcept;

  /**
   * @brief Returns the per-dimension grid-point counts (as @c TensorGrid::getGPCntPerDim).
   * @note Allocates memory; in loops, use @ref nGPInDim.
   */
  GPMI getGPCntPerDim() const;

  /**
   * @brief Returns the node positions concatenated across dimensions (as
   * @c TensorGrid::getNodesPerDim).
   * @note Copies the nodes; in loops, use @ref nodesInDim.
   */
  base::DataVector getNodesPerDim() const;

  /**
   * @brief Writes the grid point at linear index @p idx (first dimension fastest) into @p out.
   * @param idx Linear grid-point index in @c [0, nGP()).
   * @param out Output vector; must have @c size() >= nDim().
   */
  void getGridPoint(size_t idx, base::DataVector& out) const;

  /**
   * @brief Out-of-place variant of @ref getGridPoint(size_t, base::DataVector&) const.
   * @note Allocates memory; avoid in hot loops.
   */
  base::DataVector getGridPoint(size_t idx) const;

  /**
   * @brief Returns a stand-alone copy of the tensor grid (counts and nodes).
   * @note Copies the nodes.
   */
  TensorGrid toTensorGrid() const;

  /// @}

  /// @name Operators
  /// @{

  /**
   * @brief Tests for equality: the same per-dimension counts and (approximately) the same nodes,
   * as @c TensorGrid::operator==.
   */
  bool operator==(const TensorGridView& other) const;

  /// @}

 private:
  /// @brief Returns the row of the node table of dimension @p dim.
  size_t row(const size_t dim) const noexcept {
    assert(dim < nDim_);
    return firstRowOfDim_[dim] + lvls_[dim];
  }

  // The rows of the node table are read through raw pointers (copied from the table at
  // construction): a node list is then two dependent loads away instead of four.
  const LvlType* lvls_;           ///< Level multi-index (nDim_ entries).
  size_t nDim_;                   ///< Dimensionality.
  const size_t* firstRowOfDim_;   ///< NodeTable: per dimension, the row of level 0.
  const base::DataVector* rows_;  ///< NodeTable: per row, its node list.
};

}  // namespace combigrid
}  // namespace sgpp
