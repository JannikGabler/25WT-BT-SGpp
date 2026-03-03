#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_set_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include "sgpp/base/exception/not_implemented_exception.hpp"

namespace sgpp {

namespace combigrid {

MISetSGGenInstr::MISetSGGenInstr(const LvlMI& miSet) : SGGenInstr(miSet.nDim()), miSet(miSet) {}

LvlMIVec MISetSGGenInstr::genMIVec() const {
  throw sgpp::base::not_implemented_exception("Operation is not implemented yet!");
}

std::pair<LvlMIVec, std::vector<CTCoeffType>> MISetSGGenInstr::genMIVecWithCoeff() const {
  throw sgpp::base::not_implemented_exception("Operation is not implemented yet!");
}

std::shared_ptr<SGGenInstr> MISetSGGenInstr::clone() const {
  return std::make_shared<MISetSGGenInstr>(*this);
}

}  // namespace combigrid

}  // namespace sgpp
