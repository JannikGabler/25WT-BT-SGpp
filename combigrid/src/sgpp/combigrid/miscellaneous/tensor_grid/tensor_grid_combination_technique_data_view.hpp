// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tensor_grid_combination_technique_data_view.hpp
 * @brief Read-only view of one summand of a sparse grid: level multi-index, combination
 * coefficient and tensor grid.
 */
#pragma once

#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Read-only view of the summand @f$c_{\vec{\ell}}\,f_{\vec{\ell}}@f$ of a @c SparseGrid
 * or @c DeltasSparseGrid, as obtained by indexing or iterating the grid.
 *
 * The grid stores the levels of all tensor grids contiguously, the coefficients in one array and
 * the node lists once per (dimension, level) (@c NodeTable); the view refers to them and is valid
 * as long as the grid exists. It has the members of the owning @c TensorGridCTData (to which
 * @ref toTensorGridCTData copies it).
 */
struct TensorGridCTDataView {
  LvlMIView mi;               ///< Level multi-index of the tensor grid.
  CTCoeffType coefficient;    ///< Combination-technique coefficient.
  TensorGridView tensorGrid;  ///< The tensor grid (nodes in domain coordinates).

  /// @brief Returns a pointer to this view: lets the iterator of the grid, whose @c operator->
  /// returns the view by value, support <tt>it->member</tt>.
  const TensorGridCTDataView* operator->() const noexcept { return this; }

  /// @brief Returns an owning copy (level multi-index, coefficient and tensor grid).
  TensorGridCTData toTensorGridCTData() const;

  /**
   * @brief Element-wise equality test (as @c TensorGridCTData::operator==).
   * @param other Other view.
   */
  bool operator==(const TensorGridCTDataView& other) const;
};

}  // namespace combigrid
}  // namespace sgpp
