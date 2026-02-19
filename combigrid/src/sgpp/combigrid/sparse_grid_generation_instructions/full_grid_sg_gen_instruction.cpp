#include <sgpp/combigrid/sparse_grid_generation_instructions/full_grid_sg_gen_instruction.hpp>
#include "sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp"

namespace sgpp {
namespace combigrid {

FullGridSGGenInstr::FullGridSGGenInstr(const MIType maxLvl, const size_t nDim)
    : SGGenInstr(nDim), maxLvl(maxLvl) {}

void FullGridSGGenInstr::setMaxLvl(MIType maxLvl) { this->maxLvl = maxLvl; }

MIVec FullGridSGGenInstr::genCompleteMIVec() {}

std::pair<MIVec, std::vector<CTCoeffType>> genCompleteMIVecWithCoeff() {}

}  // namespace combigrid
}  // namespace sgpp