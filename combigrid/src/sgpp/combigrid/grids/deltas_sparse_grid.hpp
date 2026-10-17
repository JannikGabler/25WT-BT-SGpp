/**
 * TODO: Document
 */
#pragma once

#include <memory>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>

namespace sgpp {
namespace combigrid {

/**
 * TODO: Document
 */
class DeltasSparseGrid : public SparseGridBase {
 public:
  /// @name Constructors
  /// @{

  /**
   * TODO: Document
   *
   * @throws std::invalid_argument if the nodes are not nested in some dimension (see
   * @c SGGenInstr::areNodesOfAllDimsNested).
   */
  DeltasSparseGrid(const SGGenInstr& genInstruction);

  /// @}

  /// @name Getters
  /// @{

  /// @brief Returns the (shared) generation instruction or @c nullptr if none was attached.
  const SGGenInstr& getGenInstr() const noexcept;

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

  const std::unique_ptr<SGGenInstr> genInstr;  ///< Generation instruction that produced this SG.

  /// @}
};

}  // namespace combigrid
}  // namespace sgpp
