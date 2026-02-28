#ifndef COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_GRID_POINT_LOOKUP_HPP
#define COMBIGRID_TOOLS_SPARSE_GRID_GENERATION_GRID_POINT_LOOKUP_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/concurrent_task_queue.hpp>
#include <sgpp/combigrid/miscellaneous/vector_map/vector_map.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <thread>

namespace sgpp {
namespace combigrid {
namespace tools {

SGGenNodeLookup genSGNodeLookup(const SGGenInstr& genInstr, const LvlMIVec& miVec,
                                const std::vector<CTCoeffType> coeff);

/******************
Internal operations
******************/
namespace sg_gen_node_lookup {

struct SGGenNodeLookupInsert {
  NodeGenFunc gpGenFunc;
  GPCntType gpCnt;
  base::DataVector nodes;
};

std::vector<size_t> getDimWithUniqueNodes(const SGGenInstr& genInstr);

misc::VecMap<NodeGenFunc, std::vector<GPCntType>> getNodeCntRequiredPerNodeType(
    const SGGenInstr& genInstr, const LvlMIVec& miVec);

std::vector<GPCntType> getNodeCntRequiredByDim(size_t dim, const SGGenInstr& genInstr,
                                               const LvlMI& componentWiseMax);

misc::VecMap<NodeGenFunc, std::vector<GPCntType>> turnVecOfLocalNodeCntsIntoMap(
    const std::vector<std::pair<NodeGenFunc, std::vector<GPCntType>>>& vecOfLocalGPCnts,
    const SGGenInstr& genInstr, const std::vector<NodeGenFunc>& uniqueNodeGenFuncs);

SGGenNodeLookup genLookupBasedOnNodeCntPerType(
    const misc::VecMap<NodeGenFunc, std::vector<GPCntType>>& nodeCntPerType);

void populateTaskQueueForLookup(
    const misc::VecMap<NodeGenFunc, std::vector<GPCntType>>& nodeCntPerType,
    const std::vector<size_t>& idxBoundaries,
    ConcurrentTaskQueue<std::vector<SGGenNodeLookupInsert>>& queue);

std::thread startLookupInserterThread(
    SGGenNodeLookup& lookup, ConcurrentTaskQueue<std::vector<SGGenNodeLookupInsert>>& queue);

std::vector<size_t> getIdxBoundaries(
    const misc::VecMap<NodeGenFunc, std::vector<GPCntType>>& nodeCntPerType);

size_t getVecMapIdxByNodeCntIdx(size_t gpCntIdx, const std::vector<size_t>& idxBoundaries);

}  // namespace sg_gen_node_lookup

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif