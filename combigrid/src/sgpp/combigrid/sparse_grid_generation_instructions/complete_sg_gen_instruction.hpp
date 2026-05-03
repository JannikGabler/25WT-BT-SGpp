/**
 * @file complete_sg_gen_instruction.hpp
 * @brief Generation instruction for the standard "complete" combination
 * sparse grid up to a given maximum @f$\ell_1@f$ level.
 */
#ifndef COMBIGRID_FULL_SPARSE_GRID_GENERATION_INSTRUCTION_HPP
#define COMBIGRID_FULL_SPARSE_GRID_GENERATION_INSTRUCTION_HPP

#include <memory>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Standard combination-technique sparse-grid generator.
 *
 * Generates the full set of level multi-indices @f$\vec\ell@f$ with
 * @f$\|\vec\ell\|_1 \le L@f$ together with the standard combination
 * coefficients
 * @f$c_{\vec\ell} = (-1)^{L-\|\vec\ell\|_1}\,
 * \binom{d-1}{L-\|\vec\ell\|_1}@f$.
 */
class CompleteSGGenInstr : public SGGenInstr {
 public:
  /**
   * @brief Constructs the generator.
   * @param maxLvl Maximum @f$\ell_1@f$ norm of the generated level
   * multi-indices.
   * @param nDim   Spatial dimensionality.
   */
  CompleteSGGenInstr(LvlType maxLvl, size_t nDim);

  /// @brief Updates the maximum @f$\ell_1@f$ level.
  void setMaxLvl(LvlType maxLvl);

  /// @copydoc SGGenInstr::genMIVec
  LvlMIVec genMIVec() const override;

  /// @copydoc SGGenInstr::genMIVecWithCoeff
  std::pair<LvlMIVec, std::vector<CTCoeffType>> genMIVecWithCoeff() const override;

  /// @copydoc SGGenInstr::clone
  std::shared_ptr<SGGenInstr> clone() const override;

 private:
  LvlType maxLvl;  ///< Maximum @f$\ell_1@f$ level used by this generator.
};

}  // namespace combigrid
}  // namespace sgpp

#endif
