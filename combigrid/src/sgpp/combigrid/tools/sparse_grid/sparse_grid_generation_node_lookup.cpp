#include <omp.h>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/concurrent_task_queue.hpp>
#include <sgpp/combigrid/miscellaneous/vector_map/vector_map.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/collections/kway_unique_merge_sorted_collections.hpp>
#include <sgpp/combigrid/tools/math/ceil.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

SGGenNodeLookup genSGNodeLookup(const SGGenInstr& genInstr, const LvlMIVec& miVec,
                                const std::vector<CTCoeffType> coeff) {
  const misc::VecMap<NodeGenFunc*, std::vector<GPCntType>> nodeCntPerType =
      sg_gen_node_lookup::getNodeCntRequiredPerNodeType(genInstr, miVec);

  return sg_gen_node_lookup::genLookupBasedOnNodeCntPerType(nodeCntPerType);
}

namespace sg_gen_node_lookup {

misc::VecMap<NodeGenFunc*, std::vector<GPCntType>> getNodeCntRequiredPerNodeType(
    const SGGenInstr& genInstr, const LvlMIVec& miVec) {
  const std::shared_ptr<LvlMI> componentWiseMax = miVec.componentWiseMax();
  const std::vector<NodeGenFunc*> uniqueNodeGenFuncs = genInstr.getUniqueNodeGenFuncs();
  const std::vector<size_t> dimWithUniqueNodes = getDimWithUniqueNodes(genInstr);

  std::vector<std::pair<NodeGenFunc*, std::vector<GPCntType>>> vecOfLocalNodeCnts(
      dimWithUniqueNodes.size());

#pragma omp parallel for schedule(static) if (dimWithUniqueNodes.size() >= 2)
  for (const size_t dim : dimWithUniqueNodes) {
    NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
    const std::vector<GPCntType> localGPCnts =
        getNodeCntRequiredByDim(dim, genInstr, *componentWiseMax);
    vecOfLocalNodeCnts[dim] = {nodeGenFunc, localGPCnts};
  }

  return turnVecOfLocalNodeCntsIntoMap(vecOfLocalNodeCnts, genInstr, uniqueNodeGenFuncs);
}

std::vector<size_t> getDimWithUniqueNodes(const SGGenInstr& genInstr) {
  std::vector<size_t> result;

  for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
    const NodeGenFunc* nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
    const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
    bool duplicate = false;

    for (const size_t prevDim : result) {
      if (nodeGenFunc == genInstr.getNodeGenFuncForDim(prevDim) &&
          lvl2GPCntFunc == genInstr.getLvl2GPCntFuncForDim(prevDim)) {
        duplicate = true;
        break;
      }
    }

    if (!duplicate) {
      result.push_back(dim);
    }
  }

  return result;
}

std::vector<GPCntType> getNodeCntRequiredByDim(const size_t dim, const SGGenInstr& genInstr,
                                               const LvlMI& componentWiseMax) {
  const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);

  std::vector<GPCntType> gpCnts(componentWiseMax[dim] + 1);

  for (LvlType lvl = 0; lvl <= componentWiseMax[dim]; lvl++) {
    gpCnts[lvl] = lvl2GPCntFunc(lvl);
  }

  return gpCnts;
}

misc::VecMap<NodeGenFunc*, std::vector<GPCntType>> turnVecOfLocalNodeCntsIntoMap(
    const std::vector<std::pair<NodeGenFunc*, std::vector<GPCntType>>>& vecOfLocalNodeCnts,
    const SGGenInstr& genInstr, const std::vector<NodeGenFunc*>& uniqueNodeGenFuncs) {
  misc::VecMap<NodeGenFunc*, std::vector<GPCntType>> result(uniqueNodeGenFuncs.size());

  if (vecOfLocalNodeCnts.size() == 1) {
    result.insert_or_assign(vecOfLocalNodeCnts[0].first, vecOfLocalNodeCnts[0].second);
    return result;
  }

  std::vector<std::pair<NodeGenFunc*, std::vector<GPCntType>>> localEntries(
      uniqueNodeGenFuncs.size());  // Required because VecMap is not thread safe

#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < uniqueNodeGenFuncs.size(); i++) {
    NodeGenFunc* const nodeGenFunc = uniqueNodeGenFuncs[i];
    std::vector<
        std::pair<std::vector<GPCntType>::const_iterator, std::vector<GPCntType>::const_iterator>>
        iterators;

    for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
      // There should be only one object per unique NodeGenFunc
      if (vecOfLocalNodeCnts[dim].first == nodeGenFunc) {
        iterators.push_back(
            {vecOfLocalNodeCnts[dim].second.begin(), vecOfLocalNodeCnts[dim].second.end()});
      }
    }

    const std::vector<GPCntType> mergedNodeCnts = kway_unique_merge_sorted_collections(iterators);
    localEntries[i] = {nodeGenFunc, mergedNodeCnts};
  }

  for (const auto& entry : localEntries) {
    result.insert_or_assign(entry.first, entry.second);
  }

  return result;
}

SGGenNodeLookup genLookupBasedOnNodeCntPerType(
    const misc::VecMap<NodeGenFunc*, std::vector<GPCntType>>& nodeCntPerType) {
  const std::vector<size_t> idxBoundaries = getIdxBoundaries(nodeCntPerType);

  SGGenNodeLookup lookup(idxBoundaries[idxBoundaries.size() - 1]);
  ConcurrentTaskQueue<std::vector<SGGenNodeLookupInsert>> queue((size_t)omp_get_max_threads());

  std::thread inserterThread = startLookupInserterThread(lookup, queue);
  populateTaskQueueForLookup(nodeCntPerType, idxBoundaries, queue);

  inserterThread.join();

  return lookup;
}

void populateTaskQueueForLookup(
    const misc::VecMap<NodeGenFunc*, std::vector<GPCntType>>& nodeCntPerType,
    const std::vector<size_t>& idxBoundaries,
    ConcurrentTaskQueue<std::vector<SGGenNodeLookupInsert>>& queue) {
  const size_t localBufferSize =
      std::min(tools::ceil(idxBoundaries[idxBoundaries.size() - 1], (size_t)omp_get_max_threads()),
               constants::sg_gen_node_lookup::FOR_ITERATIONS_TO_TASK_QUEUE_PUSH);

#pragma omp parallel
  {
    std::vector<SGGenNodeLookupInsert> localInserts;
    localInserts.reserve(localBufferSize);

#pragma omp for schedule(guided)
    for (size_t i = 0; i < idxBoundaries[idxBoundaries.size() - 1]; i++) {
      if (localInserts.size() >= constants::sg_gen_node_lookup::FOR_ITERATIONS_TO_TASK_QUEUE_PUSH) {
        queue.push(std::move(localInserts));
        localInserts.clear();
        localInserts.reserve(localBufferSize);
      }

      const size_t vecMapIdx = getVecMapIdxByNodeCntIdx(i, idxBoundaries);
      const std::pair<NodeGenFunc*, std::vector<GPCntType>> vecMapEntry = nodeCntPerType[vecMapIdx];

      const GPCntType nodeCnt = vecMapEntry.second[i - idxBoundaries[vecMapIdx]];
      const base::DataVector gridsPoints = vecMapEntry.first->genGPs(nodeCnt, false);
      localInserts.push_back({vecMapEntry.first, nodeCnt, gridsPoints});
    }

    if (localInserts.size() >= 1) {
      queue.push(std::move(localInserts));
    }
  }

  queue.close();
}

std::thread startLookupInserterThread(
    SGGenNodeLookup& lookup, ConcurrentTaskQueue<std::vector<SGGenNodeLookupInsert>>& queue) {
  return std::thread([&]() {
    std::vector<SGGenNodeLookupInsert> output;

    while (true) {
      const bool elementsLeft = queue.pop(output);

      if (!elementsLeft) {
        break;
      }

      for (const SGGenNodeLookupInsert& insert : output) {
        std::pair<NodeGenFunc*, GPCntType> key{insert.gpGenFunc, insert.gpCnt};
        lookup.emplace(std::move(key), std::move(insert.nodes));
      }
    }
  });
}

/*
Always adds a leading 0 for less edge cases in the other methods
*/
std::vector<size_t> getIdxBoundaries(
    const misc::VecMap<NodeGenFunc*, std::vector<GPCntType>>& nodeCntPerType) {
  std::vector<size_t> idxBoundaries(nodeCntPerType.size() + 1);

  for (size_t i = 0; i < nodeCntPerType.size(); i++) {
    const std::pair<NodeGenFunc*, std::vector<GPCntType>>& p = nodeCntPerType[i];
    idxBoundaries[i + 1] = idxBoundaries[i] + p.second.size();
  }

  return idxBoundaries;
}

size_t getVecMapIdxByNodeCntIdx(const size_t vecMapIdx, const std::vector<size_t>& idxBoundaries) {
  assert(idxBoundaries.size() >= 2 && vecMapIdx < idxBoundaries[idxBoundaries.size() - 1]);

  // Strange indices because idxBoundaries[0] is always 0
  for (size_t idx = 0; idx < idxBoundaries.size() - 1; idx++) {
    if (vecMapIdx < idxBoundaries[idx + 1]) {
      return idx;
    }
  }

  throw std::out_of_range("Specified index for a node count is out of bounds.");
}

}  // namespace sg_gen_node_lookup

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp