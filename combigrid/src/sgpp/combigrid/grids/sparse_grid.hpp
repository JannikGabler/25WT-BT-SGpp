/**
 * @file sparse_grid.hpp
 * @brief Combination-technique sparse grid: a collection of tensor grids
 * with associated combination coefficients.
 */
#ifndef COMBIGRID_SPARSE_GRID_HPP
#define COMBIGRID_SPARSE_GRID_HPP

#include <memory>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Sparse grid expressed as a weighted sum of tensor grids
 * (combination technique).
 *
 * The combination technique approximates a function on a sparse grid as
 * @f[
 *   f(\vec{x}) \;\approx\; \sum_{\vec{\ell} \in I} c_{\vec{\ell}}\,
 *                          f_{\vec{\ell}}(\vec{x}),
 * @f]
 * where each @f$f_{\vec{\ell}}@f$ is the (anisotropic) tensor-product
 * approximation on the level @f$\vec{\ell}@f$ tensor grid and
 * @f$c_{\vec{\ell}}@f$ is the integer combination coefficient. A
 * @c SparseGrid stores those tensor grids together with the level
 * multi-index, the coefficient, and (optionally) the generation
 * instruction that produced them.
 */
class SparseGrid {
  using iterator = std::vector<TensorGridCTData>::iterator;
  using const_iterator = std::vector<TensorGridCTData>::const_iterator;
  using reverse_iterator = std::vector<TensorGridCTData>::reverse_iterator;
  using const_reverse_iterator = std::vector<TensorGridCTData>::const_reverse_iterator;

 public:
  /**********
  Constructor
  **********/
  /**
   * @brief Constructs an empty sparse grid in the given dimension.
   * @param nDim Spatial dimensionality.
   */
  SparseGrid(size_t nDim);

  /**
   * @brief Constructs a sparse grid with @p nTG default-constructed tensor-grid slots.
   * @param nDim Spatial dimensionality.
   * @param nTG  Number of tensor-grid slots to preallocate.
   */
  SparseGrid(size_t nDim, size_t nTG);

  /**
   * @brief Constructs a sparse grid by evaluating a generation instruction.
   *
   * The generation instruction provides the level multi-indices and
   * combination coefficients of the tensor grids that participate in the
   * combination, plus the per-dimension node generation rules.
   *
   * @param genInstruction Generation instruction that fully describes the
   * sparse grid.
   */
  SparseGrid(const SGGenInstr& genInstruction);

  /*****
  Getter
  *****/
  /// @brief Returns the spatial dimensionality.
  size_t nDim() const;

  /// @brief Returns the number of tensor grids in the combination.
  size_t nTG() const;

  /**
   * @brief Returns the @p idx -th tensor grid (with its level multi-index
   * and combination coefficient).
   */
  const TensorGridCTData& getTensorGrid(size_t idx) const;

  /**
   * @brief Returns an iterator to the tensor grid with level multi-index @p mi.
   *
   * @param mi Level multi-index to search for.
   * @return Iterator to the matching tensor grid, or @c end() if not found.
   * @note Linear search (@c O(nTG)); avoid in hot loops.
   */
  const_iterator getTensorGrid(const LvlMI& mi) const;

  /// @brief Returns the underlying vector of tensor grids.
  const std::vector<TensorGridCTData>& getTensorGrids() const;

  /// @brief Returns the (shared) generation instruction or @c nullptr if none was attached.
  const std::shared_ptr<const SGGenInstr> getGenInstr() const;

  /**
   * @brief Returns the cached maximum number of grid points across all
   * contained tensor grids.
   *
   * The value must be set explicitly via @ref setMaxTGGPCnt; it is not
   * recomputed automatically when tensor grids are added or modified.
   */
  size_t getMaxTGGPCnt() const;

  /**
   * @brief Returns the cached maximum of @f$\sum_k n_k@f$ across all
   * contained tensor grids, where @f$n_k@f$ is the per-dimension grid-point
   * count.
   *
   * The value must be set explicitly via @ref setMaxTGSumOverGPCntsPerDim;
   * it is not recomputed automatically.
   */
  size_t getMaxTGSumOverGPCntsPerDim() const;

  /*****
  Setter
  *****/
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

  /**
   * @brief Replaces the @p idx -th tensor grid (copy).
   * @param idx Slot index, must satisfy @c idx < nTG().
   * @param tg  New tensor grid data.
   */
  void setTensorGrid(size_t idx, const TensorGridCTData& tg);

  /**
   * @brief Replaces the @p idx -th tensor grid (move).
   * @param idx Slot index, must satisfy @c idx < nTG().
   * @param tg  New tensor grid data (moved-from).
   */
  void setTensorGrid(size_t idx, TensorGridCTData&& tg);

  /**
   * @brief Stores a copy of @p genInstr as the attached generation instruction.
   * @param genInstr Generation instruction. Its dimension must match @ref nDim().
   */
  void setGenInstr(const SGGenInstr& genInstr);

  /**
   * @brief Adopts an existing shared generation instruction.
   * @param genInstr Shared pointer to a generation instruction (moved-from).
   */
  void setGenInstr(std::shared_ptr<const SGGenInstr>&& genInstr);

  /**
   * @brief Sets the cached maximum number of grid points across contained
   * tensor grids.
   * @param maximum Cached maximum value.
   */
  void setMaxTGGPCnt(size_t maximum);

  /**
   * @brief Sets the cached maximum of @f$\sum_k n_k@f$ across contained
   * tensor grids.
   * @param maximum Cached maximum value.
   */
  void setMaxTGSumOverGPCntsPerDim(size_t maximum);

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
  bool operator==(const SparseGrid& other) const;

 private:
  const size_t nDim_;                          ///< Spatial dimensionality (fixed at construction).
  std::vector<TensorGridCTData> tensorGridData;  ///< Tensor grids and their combination coefficients.
  std::shared_ptr<const SGGenInstr> genInstr;  ///< Optional generation instruction that produced this SG.

  size_t maxTGGPCnt;                  ///< Cached maximum @c nGP() across contained tensor grids.
  size_t maxTGSumOverGPCntsPerDim;    ///< Cached maximum @f$\sum_k n_k@f$ across contained tensor grids.
};

}  // namespace combigrid
}  // namespace sgpp

#endif
