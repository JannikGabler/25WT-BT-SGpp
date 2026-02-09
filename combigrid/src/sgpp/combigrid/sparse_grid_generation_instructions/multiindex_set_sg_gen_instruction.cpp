#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_set_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include "sgpp/base/exception/not_implemented_exception.hpp"

sgpp::combigrid::MISetSGGenInstr::MISetSGGenInstr(const MIVec<unsigned int>& miSet)
    : SGGenInstr(miSet.nDim()), miSet(miSet) {}

sgpp::combigrid::MIVec<unsigned int> sgpp::combigrid::MISetSGGenInstr::genParetoMaximum() {
  // TODO: Compute pareto Maximum of this->miSet
  throw sgpp::base::not_implemented_exception("Operation is not implemented yet!");
}