#include <memory>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

CompleteSGGenInstr::CompleteSGGenInstr(const LvlType maxLvl, const size_t nDim)
    : SGGenInstr(nDim), maxLvl(maxLvl) {}

void CompleteSGGenInstr::setMaxLvl(const LvlType maxLvl) { this->maxLvl = maxLvl; }

LvlMIVec CompleteSGGenInstr::genMIVec() const { return tools::genMIVecForFullSG(maxLvl, nDim()); }

std::pair<LvlMIVec, std::vector<CTCoeffType>> CompleteSGGenInstr::genMIVecWithCoeff() const {
  LvlMIVec miVec = genMIVec();
  std::vector<CTCoeffType> coeff = tools::genCoeffForFullSG(maxLvl, nDim());
  return {std::move(miVec), std::move(coeff)};
}

std::shared_ptr<SGGenInstr> CompleteSGGenInstr::clone() const {
  return std::make_shared<CompleteSGGenInstr>(*this);
}

}  // namespace combigrid
}  // namespace sgpp