#ifndef COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_HPP
#define COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_rectangular_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/vector_map/vector_map.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

SparseGrid genSG(const SGGenInstr& genInstr);

TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr, const SGGenNodeLookup& lookup);

/******************
Internal operations
******************/
namespace sg_gen {

std::vector<size_t> getGPCntPerDim(const LvlMI& mi, const SGGenInstr& genInstr);

misc::DiscRectBB<size_t> getBBForIteration(const std::vector<size_t>& gpCntPerDim);

std::vector<base::DataVector> getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                                  const std::vector<size_t>& gpCntPerDim,
                                                  const SGGenNodeLookup& lookup);

}  // namespace sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif