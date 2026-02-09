#ifndef COMBIGRID_MISETSGGENINSTR_HPP
#define COMBIGRID_MISETSGGENINSTR_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>

namespace sgpp {
namespace combigrid {

class MISetSGGenInstr : public SGGenInstr {
 public:
  MISetSGGenInstr(const MIVec<unsigned int>& miSet);

  MIVec<unsigned int> genParetoMaximum() override;

 private:
  const MIVec<unsigned int>& miSet;
};

}  // namespace combigrid
}  // namespace sgpp

#endif