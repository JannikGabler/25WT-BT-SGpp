// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_global_interpolation_index.hpp
 * @brief Defines @c DSGGlobalInterpIndex, the query-independent data of the global interpolation
 * operator on a deltas sparse grid (built once per grid, on first use).
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

class DeltasSparseGrid;

namespace misc {

/**
 * @brief Query-independent data of the global interpolation on a deltas sparse grid: everything
 * the 1D weights of a query point (the Lagrange basis values, see
 * @c tools::dsg_global_interpolation::compBasisValues) and the global weights of the grid points
 * (the row loop of the deltas sparse grid quadrature) need besides the point.
 *
 * <b>Rows.</b> One row per (dimension j, level l), l = 0, ..., maxLvl(j) (the maximum level of
 * dimension j over the multi-indices of the grid), row(j, l) = rowOffset(j) + l. A row holds the
 * nodes of the tensor grids of level l in dimension j (the regular nodes, as the
 * @c SparseGrid operator generates them, including the boundary nodes if l is at least the
 * boundary level offset), their 1D interpolation method, and the delta order: entry k is the
 * index (in the regular nodes) of the k-th node in delta order, i.e. the delta nodes of the
 * levels 0, 1, ..., l, each in the order of the deltas sparse grid's tensor grids (as
 * @c tools::dsg_quadrature::comp1DWeights lists the quadrature weights).
 *
 * <b>Weight layout.</b> @ref weightLayout is a @c FlatNestedVec with the shape of
 * @c weights1D[dim][lvl] of the deltas sparse grid quadrature (one entry per node of the row, in
 * delta order); a call copies it and overwrites the values. Row r starts at @ref weightStart(r)
 * of the flat values.
 */
class DSGGlobalInterpIndex {
 public:
  /**
   * @brief Builds the index of @p deltasSparseGrid. The grid must outlive the index.
   * @param deltasSparseGrid Deltas sparse grid (nested nodes, d > 0).
   */
  explicit DSGGlobalInterpIndex(const DeltasSparseGrid& deltasSparseGrid);

  /// @brief Returns the number of dimensions d.
  size_t nDim() const noexcept { return rowOffset_.size() - 1; }

  /// @brief Returns the trie over the levels of the tensor grids with a non-zero coefficient.
  const CTCoeffTrie& coeffTrie() const noexcept { return coeffTrie_; }

  /// @brief Returns the maximum level of dimension @p dim over the multi-indices.
  LvlType maxLvl(const size_t dim) const noexcept {
    return static_cast<LvlType>(rowOffset_[dim + 1] - rowOffset_[dim] - 1);
  }

  /// @brief Returns the row of (dimension @p dim, level @p lvl).
  size_t row(const size_t dim, const LvlType lvl) const noexcept { return rowOffset_[dim] + lvl; }

  /// @brief Returns the number of rows.
  size_t nRows() const noexcept { return rowOffset_.back(); }

  /// @brief Returns the dimension of row @p row.
  size_t rowDim(const size_t row) const noexcept { return rowDim_[row]; }

  /// @brief Returns the rows ordered by their number of nodes, largest first (the order in which
  /// the threads compute the basis values: the largest rows, which may cost @f$O(n^2)@f$, first).
  std::span<const size_t> rowsBySize() const noexcept { return rowsBySize_; }

  /// @brief Returns the regular nodes of row @p row (normalized, as the @c SparseGrid operator).
  const std::vector<double>& nodes(const size_t row) const noexcept { return rowNodes[row]; }

  /// @brief Returns the 1D interpolation method of row @p row.
  const InterpolationMethod& method(const size_t row) const noexcept { return *rowMethods[row]; }

  /// @brief Returns the delta order of row @p row (see the class description).
  std::span<const GPCntType> deltaOrder(const size_t row) const noexcept {
    return std::span<const GPCntType>(deltaOrder_).subspan(weightStart_[row], rowNodes[row].size());
  }

  /// @brief Returns the shape of the per-call 1D weights (see the class description).
  const FlatNestedVec<double, 3>& weightLayout() const noexcept { return weightLayout_; }

  /// @brief Returns the start of row @p row in the flat values of @ref weightLayout.
  size_t weightStart(const size_t row) const noexcept { return weightStart_[row]; }

  /// @brief Returns the maximum number of nodes of a row.
  size_t maxRowSize() const noexcept { return maxRowSize_; }

  /// @brief Returns the indices of the tensor grids of the deltas sparse grid ordered by their
  /// number of grid points, largest first (the order in which the threads process them).
  std::span<const size_t> tgsBySize() const noexcept { return tgsBySize_; }

  /// @brief Returns the number of grid points of the deltas sparse grid.
  size_t nGridPoints() const noexcept { return nGridPoints_; }

 private:
  /// @brief Sets the row offsets (the maximum levels of the dimensions).
  void setRowOffsets(const DeltasSparseGrid& deltasSparseGrid);

  /// @brief Appends the rows of dimension @p dim (nodes, method, delta order, weight layout).
  void addRowsOfDim(const DeltasSparseGrid& deltasSparseGrid, size_t dim);

  CTCoeffTrie coeffTrie_;                              ///< See @ref coeffTrie.
  std::vector<size_t> rowOffset_;                      ///< Per dimension (+ end): first row.
  std::vector<size_t> rowDim_;                         ///< Per row: its dimension.
  std::vector<size_t> rowsBySize_;                     ///< See @ref rowsBySize.
  std::vector<size_t> tgsBySize_;                      ///< See @ref tgsBySize.
  std::vector<std::vector<double>> rowNodes;           ///< Per row: regular nodes.
  std::vector<const InterpolationMethod*> rowMethods;  ///< Per row: 1D interpolation method.
  std::vector<GPCntType> deltaOrder_;                  ///< Delta orders of the rows, back to back.
  std::vector<size_t> weightStart_;                    ///< Per row: start in the flat weights.
  FlatNestedVec<double, 3> weightLayout_;              ///< See @ref weightLayout.
  size_t maxRowSize_ = 0;                              ///< See @ref maxRowSize.
  size_t nGridPoints_ = 0;                             ///< See @ref nGridPoints.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
