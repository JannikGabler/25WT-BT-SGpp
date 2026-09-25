/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * TODO: Document
 */
class ExtensiveSparseGrid : public SparseGrid {
  using iterator = std::vector<TensorGridCTData>::iterator;
  using const_iterator = std::vector<TensorGridCTData>::const_iterator;
  using reverse_iterator = std::vector<TensorGridCTData>::reverse_iterator;
  using const_reverse_iterator = std::vector<TensorGridCTData>::const_reverse_iterator;

 public:
  /**********
  Constructor
  **********/
  //   /**
  //    * @brief Constructs an empty sparse grid in the given dimension.
  //    * @param nDim Spatial dimensionality.
  //    */
  //   ExtensiveSparseGrid(size_t nDim);

  //   /**
  //    * @brief Constructs a sparse grid with @p nTG default-constructed tensor-grid slots.
  //    * @param nDim Spatial dimensionality.
  //    * @param nTG  Number of tensor-grid slots to preallocate.
  //    */
  //   ExtensiveSparseGrid(size_t nDim, size_t nTG);

  /**
   * TODO: Document
   */
  ExtensiveSparseGrid(const SGGenInstr& genInstruction);

  /*****
  Getter
  *****/
  /// @brief Returns the number of tensor grids in the combination.
  size_t nTGWithCoeffZero() const noexcept;

  /**
   * TODO: Document (see SparseGrid::getTensorGrid)
   */
  const TensorGridCTData& getTensorGridWithCoeffZero(size_t idx) const noexcept;

  /**
   * TODO: Document (see SparseGrid::getTensorGrid)
   */
  const_iterator getTensorGridWithCoeffZero(const LvlMI& mi) const noexcept;

  /**
   * TODO: Document (see SparseGrid::getTensorGrids)
   */
  std::span<const TensorGridCTData> getTensorGridsWithCoeffZero() const noexcept;

  /**
   * TODO: Document (see SparseGrid::getMaxTGGPCnt)
   */
  size_t getMaxTGWithCoeffZeroGPCnt() const noexcept;

  /**
   * TODO: Document
   */
  size_t getMaxTGWithCoeffZeroSumOverGPCntsPerDim() const noexcept;

  /*****
  Setter
  *****/
  /**
   * TODO: Document
   */
  void setTensorGridsWithCoeffZero(std::vector<TensorGridCTData>&& tgs);

  /**
   * TODO: Document (see SparseGrid::addTensorGrid)
   */
  void addTensorGridWithCoeffZero(const TensorGridCTData& tg);

  /**
   * TODO: Document (see SparseGrid::addTensorGrid)
   */
  void addTensorGridWithCoeffZero(TensorGridCTData&& tg);

  /**
   * TODO: Document (see SparseGrid::setTensorGrid)
   */
  void setTensorGridWithCoeffZero(size_t idx, const TensorGridCTData& tg);

  /**
   * TODO: Document (see SparseGrid::setTensorGrid)
   */
  void setTensorGridWithCoeffZero(size_t idx, TensorGridCTData&& tg);

  /**
   * TODO: Document (see SparseGrid::setMaxTGGPCnt)
   */
  void setMaxTGWithCoeffZeroGPCnt(size_t maximum);

  /**
   * TODO: Document (see SparseGrid::setMaxTGSumOverGPCntsPerDim)
   */
  void setMaxTGWithCoeffZeroSumOverGPCntsPerDim(size_t maximum);

  /*****
  Helper
  *****/
  /**
   * TODO: Document (see SparseGrid::resize)
   */
  void resize(size_t nTGWithCoeffNonZero, size_t nTGWithCoeffZero);

  /**
   * TODO: Document (see SparseGrid::reserve)
   */
  void reserve(size_t nCoeffNonZero, size_t nCoeffZero);

  /*******
  Iterator
  *******/
  /// @name Iterators over the contained tensor grids.
  /// @{
  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_reverse_iterator crbegin() const noexcept;
  const_reverse_iterator crend() const noexcept;
  /// @}

  /*******
  Operator
  *******/

  /**
   * @brief Order-insensitive content equality.
   *
   * Two sparse grids are considered equal if they have the same dimension,
   * the same number of tensor grids, and every tensor grid in @c *this is
   * also present in @p other.
   *
   * @param other Sparse grid to compare against.
   * @return @c true if equal.
   * @note This operation is @c O(nTG^2) and should be used carefully.
   */
  bool operator==(const ExtensiveSparseGrid& other) const;

 protected:
  /*********
  Attributes
  *********/
  std::vector<TensorGridCTData> tensorGridWithCoeffZeroData;  ///< TODO: Document

  size_t maxTGWithCoeffZeroGPCnt = 0;                ///< TODO: Document
  size_t maxTGWithCoeffZeroSumOverGPCntsPerDim = 0;  ///< TODO: Document

  /*****
  Helper
  *****/
  /**
   * TODO: Document
   */
  void recomputeMetaValues();

  /**
   * TODO: Document
   */
  void updateMetaValuesAfterInsertion(const TensorGridCTData& tg);

  /**
   * TODO: Document
   */
  void updateMetaValuesBeforeSwap(const TensorGridCTData& oldTG, const TensorGridCTData& newTG);
};

}  // namespace combigrid
}  // namespace sgpp
