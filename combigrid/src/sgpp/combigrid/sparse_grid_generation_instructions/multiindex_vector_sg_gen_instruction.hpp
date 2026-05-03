/**
 * @file multiindex_vector_sg_gen_instruction.hpp
 * @brief Generation instruction that uses an externally supplied set of
 * level multi-indices.
 */
#ifndef COMBIGRID_MISETSGGENINSTR_HPP
#define COMBIGRID_MISETSGGENINSTR_HPP

#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Generation instruction backed by a user-supplied multi-index set.
 *
 * Useful when the level multi-indices come from an external algorithm
 * (e.g. an adaptive refinement loop). Combination coefficients are
 * computed on demand by @ref tools::computeCTCoeffs.
 *
 * @note The instruction stores a reference to @c miVec. The caller must
 * keep that vector alive for as long as this instruction (or any sparse
 * grid built from it) is in use.
 */
class MIVecSGGenInstr : public SGGenInstr {
 public:
  /**
   * @brief Constructs the instruction wrapping the given multi-index set.
   * @param miVec Level multi-indices (referenced, not copied).
   */
  MIVecSGGenInstr(const LvlMIVec& miVec);

  /// @copydoc SGGenInstr::genMIVec
  LvlMIVec genMIVec() const override;

  /// @copydoc SGGenInstr::genMIVecWithCoeff
  std::pair<LvlMIVec, std::vector<CTCoeffType>> genMIVecWithCoeff() const override;

  /// @copydoc SGGenInstr::clone
  std::shared_ptr<SGGenInstr> clone() const override;

 private:
  const LvlMIVec& miVecRef;  ///< Externally owned level multi-indices.
};

}  // namespace combigrid
}  // namespace sgpp

#endif
