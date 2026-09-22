#pragma once

#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>

namespace sgpp {
namespace combigrid {

class ExtensiveSparseGrid : public SparseGrid {
  ExtensiveSparseGrid(const SGGenInstr& genInstruction);
};

}  // namespace combigrid
}  // namespace sgpp