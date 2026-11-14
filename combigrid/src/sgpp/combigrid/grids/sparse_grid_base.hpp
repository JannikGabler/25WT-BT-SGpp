// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_base.hpp
 * @brief Storage of the tensor grids shared by @c SparseGrid and @c DeltasSparseGrid.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/iterators/tensor_grid_ct_data_iterator.hpp>
#include <sgpp/combigrid/miscellaneous/operator_data_cache/operator_data_cache.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Storage of the tensor grids of a combination-technique sparse grid (base class of
 * @c SparseGrid and @c DeltasSparseGrid).
 *
 * The tensor grids are stored as a struct of arrays:
 * - the level multi-indices of all tensor grids, contiguously (@ref getLevels, an @c LvlMIVec
 *   taken over from the generation of the multi-index set);
 * - the combination coefficients in one array (@ref getCoefficients);
 * - the 1D node lists once per (dimension, level), already mapped onto the domain
 *   (@ref getNodeTable, see @c NodeTable): the nodes of tensor grid @f$\vec{\ell}@f$ in dimension
 *   @f$k@f$ are the list of @f$(k, \ell_k)@f$, the number of grid points per dimension follows
 *   from it.
 *
 * Tensor grid @c i is accessed as a @c TensorGridCTDataView (level multi-index, coefficient,
 * @c TensorGridView), by index (@ref getTensorGrid) or by iterating the grid
 * (<tt>for (const auto& tgData : grid)</tt>). The views refer to the storage of the grid.
 */
class SparseGridBase {
 public:
  using const_iterator = misc::TensorGridCTDataIterator;  ///< Iterator over the tensor grids.
  using iterator = const_iterator;                        ///< The tensor grids are read-only.

  /// @name Getters
  /// @{

  /// @brief Returns the spatial dimensionality.
  size_t nDim() const noexcept { return nDim_; }

  /// @brief Returns the number of tensor grids.
  size_t nTG() const noexcept { return coefficients_.size(); }

  /**
   * @brief Returns the @p idx -th tensor grid (with its level multi-index and combination
   * coefficient), as a view into the grid.
   */
  TensorGridCTDataView getTensorGrid(size_t idx) const noexcept { return begin()[idx]; }

  /**
   * @brief Returns an iterator to the tensor grid with level multi-index @p mi.
   *
   * @param mi Level multi-index to search for.
   * @return Iterator to the matching tensor grid, or @c end() if not found.
   * @note Linear search (@c O(nTG)); avoid in hot loops.
   */
  const_iterator getTensorGrid(const LvlMIView& mi) const noexcept;

  /// @brief Returns the level multi-indices of the tensor grids, in the order of the tensor grids.
  const LvlMIVec& getLevels() const noexcept { return levels_; }

  /// @brief Returns the combination coefficients of the tensor grids, in their order.
  std::span<const CTCoeffType> getCoefficients() const noexcept { return coefficients_; }

  /// @brief Returns the 1D node lists per (dimension, level), in domain coordinates.
  const NodeTable& getNodeTable() const noexcept { return nodeTable_; }

  /// @brief Returns the maximum number of grid points of a tensor grid.
  size_t getMaxTGGPCnt() const noexcept { return maxTGGPCnt_; }

  /// @brief Returns the maximum of @f$\sum_k n_k@f$ (the sum of the per-dimension grid-point
  /// counts) over the tensor grids.
  size_t getMaxTGSumOverGPCntsPerDim() const noexcept { return maxTGSumOverGPCntsPerDim_; }

  /**
   * @brief Returns the cache of query-independent data that operators build on their first call
   * on this grid (e.g. the index of the linear interpolation on a deltas sparse grid, the node
   * table on the unit interval); internal to the operators.
   */
  misc::OperatorDataCache& getOperatorData() const noexcept { return operatorData; }

  /// @}

  /// @name Iterators
  /// @{

  const_iterator begin() const noexcept {
    return const_iterator(levels_.data(), coefficients_.data(), &nodeTable_, 0);
  }
  const_iterator end() const noexcept { return begin() + static_cast<std::ptrdiff_t>(nTG()); }
  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }

  /// @}

  /// @name Operators
  /// @{

  /**
   * @brief Order-insensitive content equality.
   *
   * Two sparse grids are considered equal if they have the same dimension, the same number of
   * tensor grids, and every tensor grid of @c *this (level multi-index, coefficient, counts and
   * nodes) is also present in @p other.
   *
   * @param other Sparse grid to compare against.
   * @return @c true if equal.
   * @note This operation is @c O(nTG^2) and should be used carefully.
   */
  bool operator==(const SparseGridBase& other) const;

  /// @}

 protected:
  /// @name Constructors
  /// @{

  /**
   * @brief Constructs an empty sparse grid with the given number of dimensions.
   * @param nDim Number of spatial dimensions.
   */
  explicit SparseGridBase(size_t nDim);

  /// @}

  /// @name Setters
  /// @{

  /**
   * @brief Takes over the storage of the tensor grids and computes the cached maxima.
   *
   * @param levels       Level multi-indices of the tensor grids (moved in by the caller).
   * @param coefficients Their combination coefficients.
   * @param nodeTable    Node lists of all (dimension, level) of @p levels.
   */
  void setStorage(LvlMIVec levels, std::vector<CTCoeffType> coefficients, NodeTable nodeTable);

  /// @}

 private:
  /// @brief Computes @ref getMaxTGGPCnt and @ref getMaxTGSumOverGPCntsPerDim.
  void computeMetaValues();

  const size_t nDim_;                      ///< Spatial dimensionality (fixed at construction).
  LvlMIVec levels_;                        ///< Level multi-indices of the tensor grids.
  std::vector<CTCoeffType> coefficients_;  ///< Combination coefficients of the tensor grids.
  NodeTable nodeTable_;                    ///< Node lists per (dimension, level).

  size_t maxTGGPCnt_ = 0;                ///< See @ref getMaxTGGPCnt.
  size_t maxTGSumOverGPCntsPerDim_ = 0;  ///< See @ref getMaxTGSumOverGPCntsPerDim.

  /// Lazily built operator data (see @ref getOperatorData): only by operators that need it.
  mutable misc::OperatorDataCache operatorData;
};

}  // namespace combigrid
}  // namespace sgpp
