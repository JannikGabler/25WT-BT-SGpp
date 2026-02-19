#include <sgpp/combigrid/sparse_grid_generation_instructions/full_grid_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_multiindex_generation.hpp>

namespace sgpp {
namespace combigrid {

FullGridSGGenInstr::FullGridSGGenInstr(const MIType maxLvl, const size_t nDim)
    : SGGenInstr(nDim), maxLvl(maxLvl) {}

void FullGridSGGenInstr::setMaxLvl(MIType maxLvl) { this->maxLvl = maxLvl; }

MIVec FullGridSGGenInstr::genCompleteMIVec() const {
  return tools::genMIVecForFullSG(maxLvl, nDim());
}

std::pair<MIVec, std::vector<CTCoeffType>> FullGridSGGenInstr::genCompleteMIVecWithCoeff() const {}

}  // namespace combigrid
}  // namespace sgpp