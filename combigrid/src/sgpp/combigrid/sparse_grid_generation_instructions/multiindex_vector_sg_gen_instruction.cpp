#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <utility>
#include <vector>

namespace sgpp {

namespace combigrid {

MIVecSGGenInstr::MIVecSGGenInstr(const LvlMIVec& miVec)
    : SGGenInstr(miVec.nDim()), miVecRef(miVec) {}

LvlMIVec MIVecSGGenInstr::genMIVec() const { return genMIVecWithCoeff().first; }

std::pair<LvlMIVec, std::vector<CTCoeffType>> MIVecSGGenInstr::genMIVecWithCoeff() const {
  LvlMIVec miVec = miVecRef.downwardsClosure();
  std::vector<CTCoeffType> coeffs = tools::computeCTCoeffs(miVec);
  const size_t nMI = miVec.nMI();

  size_t writeIdx = 0;
  for (size_t readIdx = 0; readIdx < nMI; readIdx++) {
    if (coeffs[readIdx] != 0) {
      miVec.moveMI(writeIdx, readIdx);
      coeffs[writeIdx] = std::move(coeffs[readIdx]);
      writeIdx++;
    }
  }

  miVec.resize(writeIdx);
  miVec.shrink_to_fit();
  coeffs.resize(writeIdx);
  coeffs.shrink_to_fit();

  return {miVec, coeffs};
}

std::shared_ptr<SGGenInstr> MIVecSGGenInstr::clone() const {
  return std::make_shared<MIVecSGGenInstr>(*this);
}

}  // namespace combigrid

}  // namespace sgpp
