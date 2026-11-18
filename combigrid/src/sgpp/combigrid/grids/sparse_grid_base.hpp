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
#include <ranges>
#include <sgpp/combigrid/grids/cross_call_operator_data_info.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_cache.hpp>
#include <sgpp/combigrid/miscellaneous/iterators/tensor_grid_ct_data_iterator.hpp>
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
 * - the 1D node lists once per (dimension, level) (see @c NodeTable), in domain coordinates
 *   (@ref getDomainNodeTable) and on the unit interval (@ref getUnitNodeTable): the nodes of tensor
 * grid
 *   @f$\vec{\ell}@f$ in dimension @f$k@f$ are the list of @f$(k, \ell_k)@f$, the number of grid
 *   points per dimension follows from it. The source function is evaluated at the domain
 *   coordinates; the operators compute their weights (quadrature weights, basis values, the
 *   interpolation cell) on @f$[0,1]^d@f$, from the unit nodes.
 *
 * Tensor grid @c i is accessed as a @c TensorGridCTDataView (level multi-index, coefficient,
 * @c TensorGridView), with its nodes either in domain coordinates or on the unit interval: every
 * accessor names the coordinates it uses, there is no default. By index
 * (@ref getDomainTensorGrid(size_t) const "getDomainTensorGrid(i)",
 * @ref getUnitTensorGrid(size_t) const "getUnitTensorGrid(i)"), by level multi-index, or by
 * iterating one of the two ranges (<tt>for (const auto& tgData : grid.domainTensorGrids())</tt>,
 * @ref unitTensorGrids). Both variants of a tensor grid have the same level multi-index,
 * coefficient and grid-point counts. The views refer to the storage of the grid.
 */
class SparseGridBase {
 public:
  using const_iterator = misc::TensorGridCTDataIterator;  ///< Iterator over the tensor grids.
  /// Range of the tensor grids (random access: @c begin, @c end, @c size, @c operator[]).
  using TensorGridRange = std::ranges::subrange<const_iterator>;

  /// @name Getters
  /// @{

  /// @brief Returns the spatial dimensionality.
  size_t nDim() const noexcept { return nDim_; }

  /// @brief Returns the number of tensor grids.
  size_t nTG() const noexcept { return coefficients_.size(); }

  /**
   * @brief Returns the @p idx -th tensor grid (with its level multi-index and combination
   * coefficient) with its nodes in domain coordinates, as a view into the grid.
   */
  TensorGridCTDataView getDomainTensorGrid(size_t idx) const noexcept {
    return domainTensorGrids()[static_cast<std::ptrdiff_t>(idx)];
  }

  /**
   * @brief Returns the @p idx -th tensor grid (with its level multi-index and combination
   * coefficient) with its nodes on the unit interval, as a view into the grid.
   */
  TensorGridCTDataView getUnitTensorGrid(size_t idx) const noexcept {
    return unitTensorGrids()[static_cast<std::ptrdiff_t>(idx)];
  }

  /**
   * @brief Returns an iterator into @ref domainTensorGrids to the tensor grid with level
   * multi-index @p mi.
   *
   * @param mi Level multi-index to search for.
   * @return Iterator to the matching tensor grid, or <tt>domainTensorGrids().end()</tt>.
   * @note Linear search (@c O(nTG)); avoid in hot loops.
   */
  const_iterator getDomainTensorGrid(const LvlMIView& mi) const noexcept;

  /**
   * @brief Returns an iterator into @ref unitTensorGrids to the tensor grid with level
   * multi-index @p mi.
   *
   * @param mi Level multi-index to search for.
   * @return Iterator to the matching tensor grid, or <tt>unitTensorGrids().end()</tt>.
   * @note Linear search (@c O(nTG)); avoid in hot loops.
   */
  const_iterator getUnitTensorGrid(const LvlMIView& mi) const noexcept;

  /// @brief Returns the tensor grids with their nodes in domain coordinates, in their order.
  TensorGridRange domainTensorGrids() const noexcept { return tensorGrids(domainNodeTable_); }

  /// @brief Returns the tensor grids with their nodes on the unit interval, in their order.
  TensorGridRange unitTensorGrids() const noexcept { return tensorGrids(unitNodeTable_); }

  /// @brief Returns the level multi-indices of the tensor grids, in the order of the tensor grids.
  const LvlMIVec& getLevels() const noexcept { return levels_; }

  /// @brief Returns the combination coefficients of the tensor grids, in their order.
  std::span<const CTCoeffType> getCoefficients() const noexcept { return coefficients_; }

  /// @brief Returns the 1D node lists per (dimension, level), in domain coordinates.
  const NodeTable& getDomainNodeTable() const noexcept { return domainNodeTable_; }

  /// @brief Returns the 1D node lists per (dimension, level), on the unit interval.
  const NodeTable& getUnitNodeTable() const noexcept { return unitNodeTable_; }

  /// @brief Returns the maximum number of grid points of a tensor grid.
  size_t getMaxTGGPCnt() const noexcept { return maxTGGPCnt_; }

  /// @brief Returns the maximum of @f$\sum_k n_k@f$ (the sum of the per-dimension grid-point
  /// counts) over the tensor grids.
  size_t getMaxTGSumOverGPCntsPerDim() const noexcept { return maxTGSumOverGPCntsPerDim_; }

  /**
   * @brief Returns the cross-call operator data cache of the grid (see
   * @ref getCrossCallOperatorDataCacheEntries); internal to the operators.
   */
  misc::CrossCallOperatorDataCache& getCrossCallOperatorDataCache() const noexcept {
    return crossCallOperatorDataCache_;
  }

  /// @}

  /// @name Cross-call operator data cache
  /// Query-independent data that operators build for this grid and keep for their later calls.
  /// An operator stores and reuses such data only if its options ask for it
  /// (@c useCrossCallOperatorDataCache; the default depends on the operator, see its
  /// documentation): the indices of @c interpolate and @c interpolateLinear on a
  /// @c DeltasSparseGrid (default: kept), the weight tables of @c quadrature (default: not kept).
  /// Results do not depend on whether the data is reused. The cache belongs to this grid object
  /// (grids are not copyable) and is ignored by @ref operator==. All methods are thread-safe,
  /// also concurrently with operator calls on the grid.
  /// @{

  /**
   * @brief Removes all data from the cross-call operator data cache: frees its memory and makes
   * the next operator call that uses the cache build its data anew.
   *
   * Operator calls running concurrently keep the data they use until they return; data whose
   * build started before the clear is not stored.
   */
  void clearCrossCallOperatorDataCache() const;

  /**
   * @brief Returns the descriptions of the entries of the cross-call operator data cache (which
   * operator stored what, under which reuse conditions, and its memory), in the order of their
   * creation.
   */
  std::vector<CrossCallOperatorDataInfo> getCrossCallOperatorDataCacheEntries() const;

  /// @brief Returns the total memory of the cross-call operator data cache in bytes (the sum over
  /// @ref getCrossCallOperatorDataCacheEntries).
  size_t getCrossCallOperatorDataCacheBytes() const;

  /// @}

  /// @name Operators
  /// @{

  /**
   * @brief Order-insensitive content equality.
   *
   * Two sparse grids are considered equal if they have the same dimension, the same number of
   * tensor grids, and every tensor grid of @c *this (level multi-index, coefficient, counts and
   * domain nodes) is also present in @p other.
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
   * @param levels          Level multi-indices of the tensor grids (moved in by the caller).
   * @param coefficients    Their combination coefficients.
   * @param domainNodeTable Node lists of all (dimension, level) of @p levels, on the domain.
   * @param unitNodeTable   The same node lists on the unit interval.
   */
  void setStorage(LvlMIVec levels, std::vector<CTCoeffType> coefficients, NodeTable domainNodeTable,
                  NodeTable unitNodeTable);

  /// @}

 private:
  /// @brief Returns the tensor grids with the nodes of @p table (one of the two node tables).
  TensorGridRange tensorGrids(const NodeTable& table) const noexcept {
    const const_iterator first(levels_.data(), coefficients_.data(), &table, 0);
    return {first, first + static_cast<std::ptrdiff_t>(nTG())};
  }

  /// @brief Returns the iterator to the tensor grid with level multi-index @p mi in @p range.
  static const_iterator findTensorGrid(TensorGridRange range, const LvlMIView& mi) noexcept;

  /// @brief Computes @ref getMaxTGGPCnt and @ref getMaxTGSumOverGPCntsPerDim.
  void computeMetaValues();

  const size_t nDim_;                      ///< Spatial dimensionality (fixed at construction).
  LvlMIVec levels_;                        ///< Level multi-indices of the tensor grids.
  std::vector<CTCoeffType> coefficients_;  ///< Combination coefficients of the tensor grids.
  NodeTable domainNodeTable_;              ///< Node lists per (dimension, level), on the domain.
  NodeTable unitNodeTable_;                ///< The same on the unit interval.

  size_t maxTGGPCnt_ = 0;                ///< See @ref getMaxTGGPCnt.
  size_t maxTGSumOverGPCntsPerDim_ = 0;  ///< See @ref getMaxTGSumOverGPCntsPerDim.

  /// Operator data kept across calls (see @ref getCrossCallOperatorDataCacheEntries). Declared
  /// last: destroyed first, its data may refer to the members above.
  mutable misc::CrossCallOperatorDataCache crossCallOperatorDataCache_;
};

}  // namespace combigrid
}  // namespace sgpp
