#ifndef COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_HPP
#define COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/vector_map/vector_map.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

// SGGenGPLookup genSGGPLookup(const SGGenInstr& genInstr, const MIVec& miVec,
//                             const std::vector<CTCoeffType> coeff);

/******************
Internal operations
******************/
namespace sg_gen {

std::vector<size_t> getDimWithUniqueGP(const SGGenInstr& genInstr);

// misc::VecMap<GPGenFunc, std::vector<GPCntType>> getGPCntRequiredPerGPTypes(
//     const SGGenInstr& genInstr, const MIVec& miVec);

// std::vector<GPCntType> getGPCntRequiredByDim(size_t dim, const SGGenInstr& genInstr,
//                                              const MI& componentWiseMax);

// misc::VecMap<GPGenFunc, std::vector<GPCntType>> turnVecOfLocalGPCntsIntoMap(
//     const std::vector<std::pair<GPGenFunc, std::vector<GPCntType>>>& vecOfLocalGPCnts,
//     const SGGenInstr& genInstr, const std::vector<GPGenFunc>& uniqueGPGenFuncs);

// std::vector<size_t> getIdxBoundaries(
//     const misc::VecMap<GPGenFunc, std::vector<GPCntType>>& gpCntPerType);

// size_t getGPGenFuncIdxByGPCntIdx(size_t gpCntIdx, const std::vector<size_t>& idxBoundaries);

}  // namespace sg_gen

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif