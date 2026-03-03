#include <memory>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

FullSGGenInstr::FullSGGenInstr(const LvlType maxLvl, const size_t nDim)
    : SGGenInstr(nDim), maxLvl(maxLvl) {}

void FullSGGenInstr::setMaxLvl(const LvlType maxLvl) { this->maxLvl = maxLvl; }

LvlMIVec FullSGGenInstr::genMIVec() const { return tools::genMIVecForFullSG(maxLvl, nDim()); }

std::pair<LvlMIVec, std::vector<CTCoeffType>> FullSGGenInstr::genMIVecWithCoeff() const {
  const LvlMIVec miVec = genMIVec();
  const std::vector<CTCoeffType> coeff = tools::genCoeffForFullSG(maxLvl, nDim());
  return {miVec, coeff};
}

std::shared_ptr<SGGenInstr> FullSGGenInstr::clone() const {
  return std::make_shared<FullSGGenInstr>(*this);
}

}  // namespace combigrid
}  // namespace sgpp