#include <memory>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/complete_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

CompleteSGGenInstr::CompleteSGGenInstr(const LvlType maxLvl, const size_t nDim)
    : SGGenInstr(nDim), maxLvl(maxLvl) {}

void CompleteSGGenInstr::setMaxLvl(const LvlType maxLvl) { this->maxLvl = maxLvl; }

LvlMIVec CompleteSGGenInstr::genFullMIVec() const {
  return tools::genReducedMIVecForCompleteSG(maxLvl, nDim());
}

LvlMIVec CompleteSGGenInstr::genReducedMIVec() const {
  return tools::genReducedMIVecForCompleteSG(maxLvl, nDim());
}

std::pair<LvlMIVec, std::vector<CTCoeffType>> CompleteSGGenInstr::genFullMIVecWithCoeffs() const {
  const LvlMIVec miVec = genFullMIVec();
  const std::vector<CTCoeffType> coeff = tools::genFullCoeffForCompleteSG(maxLvl, nDim());
  return {miVec, coeff};
}

std::pair<LvlMIVec, std::vector<CTCoeffType>> CompleteSGGenInstr::genReducedMIVecWithCoeffs()
    const {
  const LvlMIVec miVec = genReducedMIVec();
  const std::vector<CTCoeffType> coeff = tools::genReducedCoeffForCompleteSG(maxLvl, nDim());
  return {miVec, coeff};
}

std::shared_ptr<SGGenInstr> CompleteSGGenInstr::clone() const {
  return std::make_shared<CompleteSGGenInstr>(*this);
}

}  // namespace combigrid
}  // namespace sgpp