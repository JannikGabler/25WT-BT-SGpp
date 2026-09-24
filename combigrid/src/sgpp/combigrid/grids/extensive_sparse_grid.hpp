/**
 * @file extensive_sparse_grid.hpp
 * @brief Sparse grid that additionally keeps the tensor grids with
 * combination coefficient 0.
 */
#pragma once

#include <sgpp/combigrid/grids/sparse_grid.hpp>

#include <cassert>
#include <cstddef>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * TODO: Document
 */
class ExtensiveSparseGrid : public SparseGrid {
 public:
  /**********
  Constructor
  **********/
  ExtensiveSparseGrid(const SGGenInstr& genInstr);

  /*****
  Getter
  *****/
  /// @brief Returns the number of all tensor grids (including those with a CT coefficient of 0).
  size_t nTGIncludingZeroCoeffs() const;

  /**
   * @brief Returns the @p idx -th tensor grid of the complete storage.
   * @param idx Index into all tensor grids, must satisfy @c idx < nTGAll().
   */
  const TensorGridCTData& getTensorGridIncludingZeroCoeffs(size_t idx) const;

  /**
   * @brief Searches all tensor grids (including c == 0) for level multi-index @p mi.
   * @return Iterator to the matching tensor grid, or @c endAll() if not found.
   * @note Linear search (@c O(nTGAll)); avoid in hot loops.
   */
  const_iterator findTensorGridIncludingZeroCoeffs(const LvlMI& mi) const;

  /// @brief Contiguous view on all tensor grids, e.g. for range-based for loops.
  std::span<TensorGridCTData> getTensorGridsIncludingZeroCoeffs();
  std::span<const TensorGridCTData> getTensorGridsIncludingZeroCoeffs() const;

  /// @brief Contiguous view on the tensor grids with c == 0.
  std::span<TensorGridCTData> getTensorGridsWithCoeffZero() noexcept;
  std::span<const TensorGridCTData> getTensorGridsWithCoeffZero() const noexcept;

  /*****
  Setter
  */

  /**
   * @brief Appends a tensor grid (copy).
   * @param tg Tensor grid combination-technique data to add.
   */
  void addTensorGrid(const TensorGridCTData& tg);

  /**
   * @brief Appends a tensor grid (move).
   * @param tg Tensor grid combination-technique data to move from.
   */
  void addTensorGrid(TensorGridCTData&& tg);

  // *****/
  //     /**
  //      * @brief Inserts a tensor grid with c == 0 into the inactive partition.
  //      * @note O(nTG()) because the active range is shifted by one element.
  //      * Existing active indices, iterators and spans are invalidated.
  //      */
  //     void addInactiveTensorGrid(const TensorGridCTData& tg);
  // void addInactiveTensorGrid(TensorGridCTData&& tg);

  /*******
  Iterator
  *******/
  /// @name Iterators over all tensor grids (including c == 0).
  /// @{
  iterator beginIncludingZeroCoeffs() noexcept;
  iterator endIncludingZeroCoeffs() noexcept;
  const_iterator beginIncludingZeroCoeffs() const noexcept;
  const_iterator endIncludingZeroCoeffs() const noexcept;
  const_iterator cbeginIncludingZeroCoeffs() const noexcept;
  const_iterator cendIncludingZeroCoeffs() const noexcept;

  reverse_iterator rbeginIncludingZeroCoeffs() noexcept;
  reverse_iterator rendIncludingZeroCoeffs() noexcept;
  const_reverse_iterator rbeginIncludingZeroCoeffs() const noexcept;
  const_reverse_iterator rendIncludingZeroCoeffs() const noexcept;
  const_reverse_iterator crbeginIncludingZeroCoeffs() const noexcept;
  const_reverse_iterator crendIncludingZeroCoeffs() const noexcept;
  /// @}

  /*******
  Operator
  *******/
  /**
   * @brief Order-insensitive equality of active @e and inactive tensor grids.
   *
   * Comparing with a plain SparseGrid (sg == esg) uses
   * SparseGrid::operator== and therefore only the active tensor grids.
   *
   * @note @c O(nTGAll^2).
   */
  bool operator==(const ExtensiveSparseGrid& other) const;
};

}  // namespace combigrid
}  // namespace sgpp