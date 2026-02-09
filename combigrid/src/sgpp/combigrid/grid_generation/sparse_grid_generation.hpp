#ifndef COMBIGRID_SG_GENERATION_HPP
#define COMBIGRID_SG_GENERATION_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include "sgpp/combigrid/grids/sparse_grid.hpp"
#include "sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp"

namespace sgpp {
namespace combigrid {

// TODO: return type
SparseGrid& generateSparsegrid(const SGGenInstr& instruction);

}  // namespace combigrid
}  // namespace sgpp

#endif