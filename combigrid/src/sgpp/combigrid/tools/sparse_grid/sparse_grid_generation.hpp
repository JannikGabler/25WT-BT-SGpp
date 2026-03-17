#pragma once

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

void populateSG(const SGGenInstr& genInstr, const LvlMIVec& miVec,
                const std::vector<CTCoeffType>& coeffs, SparseGrid& out);

TensorGrid genTGForMI(const LvlMI& mi, const SGGenInstr& genInstr, const SGGenNodeLookup& lookup);

/******************
Internal operations
******************/
namespace sg_gen {

GPMI getGPCntPerDim(const LvlMI& mi, const SGGenInstr& genInstr);

misc::DiscRectBB<GPCntType> getBBForIteration(GPMI gpCntPerDim);

std::vector<base::DataVector> getNodesPerDimForTG(const LvlMI& mi, const SGGenInstr& genInstr,
                                                  const GPMI& gpCntPerDim,
                                                  const SGGenNodeLookup& lookup);

}  // namespace sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp