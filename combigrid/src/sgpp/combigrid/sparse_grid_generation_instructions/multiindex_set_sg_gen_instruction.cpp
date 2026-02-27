#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_set_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include "sgpp/base/exception/not_implemented_exception.hpp"

namespace sgpp {

namespace combigrid {

MISetSGGenInstr::MISetSGGenInstr(const MIVec& miSet) : SGGenInstr(miSet.nDim()), miSet(miSet) {}

MIVec MISetSGGenInstr::genMIVec() const {
  throw sgpp::base::not_implemented_exception("Operation is not implemented yet!");
}

std::pair<MIVec, std::vector<CTCoeffType>> MISetSGGenInstr::genMIVecWithCoeff() const {
  throw sgpp::base::not_implemented_exception("Operation is not implemented yet!");
}

}  // namespace combigrid

}  // namespace sgpp
