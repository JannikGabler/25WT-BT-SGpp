#ifndef COMBIGRID_MISETSGGENINSTR_HPP
#define COMBIGRID_MISETSGGENINSTR_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class MISetSGGenInstr : public SGGenInstr {
 public:
  MISetSGGenInstr(const MIVec& miSet);

  MIVec genCompleteMIVec() const override;

  std::pair<MIVec, std::vector<CTCoeffType>> genCompleteMIVecWithCoeff() const override;

 private:
  const MIVec& miSet;
};

}  // namespace combigrid
}  // namespace sgpp

#endif