#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_multiindex_generation.hpp>

namespace sgpp {
namespace combigrid {

FullSGGenInstr::FullSGGenInstr(const MIType maxLvl, const size_t nDim)
    : SGGenInstr(nDim), maxLvl(maxLvl) {}

void FullSGGenInstr::setMaxLvl(MIType maxLvl) { this->maxLvl = maxLvl; }

MIVec FullSGGenInstr::genCompleteMIVec() const { return tools::genMIVecForFullSG(maxLvl, nDim()); }

std::pair<MIVec, std::vector<CTCoeffType>> FullSGGenInstr::genCompleteMIVecWithCoeff() const {}

}  // namespace combigrid
}  // namespace sgpp