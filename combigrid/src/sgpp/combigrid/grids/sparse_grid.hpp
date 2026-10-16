/**
 * @file sparse_grid.hpp
 * @brief Combination-technique sparse grid: a collection of tensor grids
 * with associated combination coefficients.
 */
#pragma once

#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>

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
class SparseGrid : public SparseGridBase {
 public:
  /// @name Constructors
  /// @{

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

  /// @}

  /// @name Getters
  /// @{

  /// @brief Returns the (shared) generation instruction or @c nullptr if none was attached.
  const std::shared_ptr<const SGGenInstr> getGenInstr() const noexcept;

  /// @}

  /// @name Setters
  /// @{

  //   /**
  //    * @brief Stores a copy of @p genInstr as the attached generation instruction.
  //    * @param genInstr Generation instruction. Its dimension must match @ref nDim().
  //    */
  //   void setGenInstr(const SGGenInstr& genInstr);

  //   /**
  //    * @brief Adopts an existing shared generation instruction.
  //    * @param genInstr Shared pointer to a generation instruction (moved-from).
  //    */
  //   void setGenInstr(std::shared_ptr<const SGGenInstr>&& genInstr);

  /// @}

 protected:
  /// @name Attributes
  /// @{

  std::shared_ptr<const SGGenInstr> genInstr =
      nullptr;  ///< Optional generation instruction that produced this SG.

  /// @}
};

}  // namespace combigrid
}  // namespace sgpp
