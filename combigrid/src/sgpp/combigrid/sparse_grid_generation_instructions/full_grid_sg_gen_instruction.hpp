#ifndef COMBIGRID_FULL_GRID_GENERATION_INSTRUCTION_HPP
#define COMBIGRID_FULL_GRID_GENERATION_INSTRUCTION_HPP

#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class FullGridSGGenInstr : public SGGenInstr {
 public:
  FullGridSGGenInstr(MIType maxLvl, size_t nDim);

  void setMaxLvl(MIType maxLvl);

  MIVec genCompleteMIVec() override;

  std::pair<MIVec, std::vector<CTCoeffType>> genCompleteMIVecWithCoeff() override;

 private:
  MIType maxLvl;
};

}  // namespace combigrid
}  // namespace sgpp

#endif