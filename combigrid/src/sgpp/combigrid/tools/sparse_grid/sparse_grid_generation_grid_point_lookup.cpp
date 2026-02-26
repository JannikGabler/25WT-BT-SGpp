#include <omp.h>
#include <cassert>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/concurrency/concurrent_task_queue.hpp>
#include <sgpp/combigrid/miscellaneous/vector_map/vector_map.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/collections/kway_unique_merge_sorted_collections.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_grid_point_lookup.hpp>
#include <stdexcept>
#include <utility>
#include <vector>
#include "sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

namespace {
struct SGGenGPLookupInsert {
  GPGenFunc gpGenFunc;
  GPCntType gpCnt;
  base::DataVector nodes;
};

}  // namespace

SGGenGPLookup genSGGPLookup(const SGGenInstr& genInstr, const MIVec& miVec,
                            const std::vector<CTCoeffType> coeff) {
  assert(genInstr.nDim() >= 1);

  const misc::VecMap<GPGenFunc, std::vector<GPCntType>> gpCntPerType =
      sg_gen_gp_lookup::getGPCntRequiredPerGPTypes(genInstr, miVec);
}

namespace sg_gen_gp_lookup {

std::vector<size_t> getDimWithUniqueGP(const SGGenInstr& genInstr) {
  std::vector<size_t> result;

  for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
    const GPGenFunc gpGenFunc = genInstr.getGPGenFuncForDim(dim);
    const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
    bool duplicate = false;

    for (const size_t prevDim : result) {
      if (gpGenFunc == genInstr.getGPGenFuncForDim(prevDim) ||
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

misc::VecMap<GPGenFunc, std::vector<GPCntType>> getGPCntRequiredPerGPType(
    const SGGenInstr& genInstr, const MIVec& miVec) {
  assert(genInstr.nDim() >= 1);

  const std::shared_ptr<MI> componentWiseMax = miVec.componentWiseMax();
  const std::vector<GPGenFunc> uniqueGPGenFuncs = genInstr.getUniqueGPGenFuncs();
  const std::vector<size_t> dimWithUniqueGP = getDimWithUniqueGP(genInstr);

  std::vector<std::pair<GPGenFunc, std::vector<GPCntType>>> vecOfLocalGPCnts(
      dimWithUniqueGP.size());

#pragma omp parallel for schedule(static) if (dimWithUniqueGP.size() >= 2)
  for (const size_t dim : dimWithUniqueGP) {
    const GPGenFunc gpGenFunc = genInstr.getGPGenFuncForDim(dim);
    const std::vector<GPCntType> localGPCnts =
        getGPCntRequiredByDim(dim, genInstr, *componentWiseMax);
    vecOfLocalGPCnts[dim] = {gpGenFunc, localGPCnts};
  }

  return turnVecOfLocalGPCntsIntoMap(vecOfLocalGPCnts, genInstr, uniqueGPGenFuncs);
}

std::vector<GPCntType> getGPCntRequiredByDim(const size_t dim, const SGGenInstr& genInstr,
                                             const MI& componentWiseMax) {
  const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);

  std::vector<GPCntType> gpCnts(componentWiseMax[dim] + 1);

  for (MIType lvl = 0; lvl <= componentWiseMax[dim]; lvl++) {
    gpCnts.push_back(lvl2GPCntFunc(lvl));
  }

  return gpCnts;
}

misc::VecMap<GPGenFunc, std::vector<GPCntType>> turnVecOfLocalGPCntsIntoMap(
    const std::vector<std::pair<GPGenFunc, std::vector<GPCntType>>>& vecOfLocalGPCnts,
    const SGGenInstr& genInstr, const std::vector<GPGenFunc>& uniqueGPGenFuncs) {
  misc::VecMap<GPGenFunc, std::vector<GPCntType>> result(uniqueGPGenFuncs.size());

  if (vecOfLocalGPCnts.size() == 1) {
    result.insert_or_assign(vecOfLocalGPCnts[0].first, vecOfLocalGPCnts[0].second);
    return result;
  }

#pragma omp parallel for schedule(static)
  for (const GPGenFunc gpGenFunc : uniqueGPGenFuncs) {
    std::vector<
        std::pair<std::vector<GPCntType>::const_iterator, std::vector<GPCntType>::const_iterator>>
        iterators;

    for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
      if (vecOfLocalGPCnts[dim].first == gpGenFunc) {
        iterators.push_back(
            {vecOfLocalGPCnts[dim].second.begin(), vecOfLocalGPCnts[dim].second.end()});
      }
    }

    const std::vector<GPCntType> mergedGPCnts = kway_unique_merge_sorted_collections(iterators);

    result.insert_or_assign(gpGenFunc, mergedGPCnts);
  }

  return result;
}

SGGenGPLookup genLookupBasedOnGPCntPerType(
    const misc::VecMap<GPGenFunc, std::vector<GPCntType>>& gpCntPerType) {
  const std::vector<size_t> idxBoundaries = getIdxBoundaries(gpCntPerType);
  const std::vector<size_t> part =
      partitionRangeForConcurrency(idxBoundaries[idxBoundaries.size() - 1],
                                   size_t minLengthForConcurrency, size_t minLengthPerThread);

  SGGenGPLookup lookup(idxBoundaries[idxBoundaries.size() - 1]);
  ConcurrentTaskQueue<std::vector<SGGenGPLookupInsert>> queue(4);

#pragma omp parallel num_threads(part.size() - 1) if (part.size() > 2)
  {
    const size_t threadId = (size_t)omp_get_thread_num();
    const size_t startIdx = part[threadId];
    const size_t endIdx = part[threadId + 1] - 1;
    // populateTaskQueueForLookup
  }

  queue.close();
}

/*
Inclusive endIdx
*/
void populateTaskQueueForLookup(const size_t startIdx, const size_t endIdx,
                                const std::vector<size_t>& idxBoundaries,
                                const misc::VecMap<GPGenFunc, std::vector<GPCntType>>& gpCntPerType,
                                ConcurrentTaskQueue<std::vector<SGGenGPLookupInsert>>& queue) {
  size_t gpGenFuncIdx = getGPGenFuncIdxByGPCntIdx(startIdx, idxBoundaries);
  std::pair<GPGenFunc, std::vector<GPCntType>> vecMapEntry = gpCntPerType[gpGenFuncIdx];
  std::vector<SGGenGPLookupInsert> localGPVec(?);

  for (size_t idx = startIdx; idx <= endIdx; idx++) {
    if (idx >= idxBoundaries[gpGenFuncIdx] || localGPVec.size() >= ?) { // We want to push our results into the task queue
      queue.push(std::move(localGPVec));

      if (idx >= idxBoundaries[gpGenFuncIdx]) {  // We need to go to the next gpGenFunc
        gpGenFuncIdx++;
        vecMapEntry = gpCntPerType[gpGenFuncIdx];
      }
    }

    const base::DataVector gridsPoints = ;  // Generate gridpoints;
    const SGGenGPLookupInsert insert { vecMapEntry.first, vecMapEntry.second, gridsPoints }
  }
}

std::vector<size_t> getIdxBoundaries(
    const misc::VecMap<GPGenFunc, std::vector<GPCntType>>& gpCntPerType) {
  std::vector<size_t> idxBoundaries(gpCntPerType.size());

  if (gpCntPerType.size() >= 1) {
    idxBoundaries[0] = gpCntPerType[0].second.size();

    for (size_t i = 1; i < gpCntPerType.size(); i++) {
      const std::pair<GPGenFunc, std::vector<GPCntType>>& p = gpCntPerType[i];
      idxBoundaries[i] = idxBoundaries[i - 1] + p.second.size();
    }
  }

  return idxBoundaries;
}

size_t getGPGenFuncIdxByGPCntIdx(const size_t gpCntIdx, const std::vector<size_t>& idxBoundaries) {
  assert(idxBoundaries.size() >= 1 && gpCntIdx < idxBoundaries[idxBoundaries.size() - 1]);

  for (size_t idx = 0; idx < idxBoundaries.size(); idx++) {
    if (gpCntIdx < idxBoundaries[idx]) {
      return gpCntIdx;
    }
  }

  throw std::out_of_range("Specified index for a grid point count is out of bounds.");
}

}  // namespace sg_gen_gp_lookup

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp