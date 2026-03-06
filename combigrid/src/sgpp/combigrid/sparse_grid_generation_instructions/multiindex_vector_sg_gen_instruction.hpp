#ifndef COMBIGRID_MISETSGGENINSTR_HPP
#define COMBIGRID_MISETSGGENINSTR_HPP

#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class MIVecSGGenInstr : public SGGenInstr {
 public:
  MIVecSGGenInstr(const LvlMIVec& miVec);

  LvlMIVec genMIVec() const override;

  std::pair<LvlMIVec, std::vector<CTCoeffType>> genMIVecWithCoeff() const override;

  std::shared_ptr<SGGenInstr> clone() const override;

 private:
  const LvlMIVec& miVecRef;
};

}  // namespace combigrid
}  // namespace sgpp

#endif