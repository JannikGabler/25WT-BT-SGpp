#ifndef COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_HPP
#define COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <unordered_map>

namespace sgpp {
namespace combigrid {
namespace tools {

std::unordered_map<std::pair<GPGenFunc, GPCntType>, base::DataVector> genSGGPLookup(
    const SGGenInstr& genInstr, const MIVec& miVec, const std::vector<CTCoeffType> coeff);

}

}  // namespace combigrid
}  // namespace sgpp

#endif