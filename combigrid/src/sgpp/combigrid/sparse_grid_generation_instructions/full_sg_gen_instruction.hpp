#ifndef COMBIGRID_FULL_SPARSE_GRID_GENERATION_INSTRUCTION_HPP
#define COMBIGRID_FULL_SPARSE_GRID_GENERATION_INSTRUCTION_HPP

#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class FullSGGenInstr : public SGGenInstr {
 public:
  FullSGGenInstr(LvlType maxLvl, size_t nDim);

  void setMaxLvl(LvlType maxLvl);

  LvlMIVec genMIVec() const override;

  std::pair<LvlMIVec, std::vector<CTCoeffType>> genMIVecWithCoeff() const override;

 private:
  LvlType maxLvl;
};

}  // namespace combigrid
}  // namespace sgpp

#endif