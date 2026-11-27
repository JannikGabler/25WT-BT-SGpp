// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/grids/cross_call_operator_data_info.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/tools/sparse_grid/tensor_grid_processing_order.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

SparseGridBase::const_iterator SparseGridBase::getDomainTensorGrid(
    const LvlMIView& mi) const noexcept {
  return findTensorGrid(domainTensorGrids(), mi);
}

SparseGridBase::const_iterator SparseGridBase::getUnitTensorGrid(
    const LvlMIView& mi) const noexcept {
  return findTensorGrid(unitTensorGrids(), mi);
}

void SparseGridBase::clearCrossCallOperatorDataCache() const {
  crossCallOperatorDataCache_.clear();
}

std::vector<CrossCallOperatorDataInfo> SparseGridBase::getCrossCallOperatorDataCacheEntries()
    const {
  return crossCallOperatorDataCache_.entries();
}

size_t SparseGridBase::getCrossCallOperatorDataCacheBytes() const {
  return crossCallOperatorDataCache_.memoryBytes();
}

SparseGridBase::const_iterator SparseGridBase::findTensorGrid(const TensorGridRange range,
                                                              const LvlMIView& mi) noexcept {
  return std::ranges::find_if(range,
                              [&mi](const TensorGridCTDataView& data) { return data.mi == mi; });
}

// TODO: Maybe change logic to only compare the nDim, nTG, levels, coefficients, node gen funcs,
// lvl2GPCnt funcs, and domain bounds. The node tables are derived from these values and do not need
// to be compared.
bool SparseGridBase::operator==(const SparseGridBase& other) const {
  if (nDim() != other.nDim() || nTG() != other.nTG()) {
    return false;
  }

  const TensorGridRange otherTGs = other.domainTensorGrids();
  for (const TensorGridCTDataView data : domainTensorGrids()) {
    if (std::ranges::find(otherTGs, data) == otherTGs.end()) {
      return false;
    }
  }

  return true;
}

SparseGridBase::SparseGridBase(const SparseGridSetup& setup)
    : nDim_(setup.nDim()), setup_(setup), levels_(setup.nDim(), 0) {}

void SparseGridBase::setStorage(LvlMIVec levels, std::vector<CTCoeffType> coefficients,
                                NodeTable domainNodeTable, NodeTable unitNodeTable) {
  assert(levels.nDim() == nDim_ && domainNodeTable.nDim() == nDim_ &&
         unitNodeTable.nDim() == nDim_);
  assert(levels.size() == coefficients.size());

  // Numbers start at 1 (0 marks "never synchronized" in the refinement state)
  static std::atomic<uint64_t> lastStorageVersion{0};
  storageVersion_ = lastStorageVersion.fetch_add(1, std::memory_order_relaxed) + 1;

  levels_ = std::move(levels);
  coefficients_ = std::move(coefficients);
  domainNodeTable_ = std::move(domainNodeTable);
  unitNodeTable_ = std::move(unitNodeTable);
  computeMetaValues();
}

/**
 * One pass over the contiguous levels (in parallel: a deltas sparse grid in high dimensions has
 * hundreds of thousands of tensor grids), with the counts read from the node table; then the
 * serial counting sort of the size classes (O(nTG)).
 */
void SparseGridBase::computeMetaValues() {
  const size_t nDim = nDim_;
  const size_t nTG = this->nTG();
  const LvlType* const lvls = levels_.data();
  const NodeTable& table = domainNodeTable_;
  std::vector<tools::sparse_grid::SizeClassType> sizeClasses(nTG);
  size_t maxGPCnt = 0, maxSum = 0, totalGPCnt = 0;
  const bool parallel = nTG >= constants::sparse_grid_storage::MIN_TG_FOR_CONCURRENCY;

#pragma omp parallel for schedule(static) if (parallel) reduction(max : maxGPCnt, maxSum) \
    reduction(+ : totalGPCnt)
  for (size_t tg = 0; tg < nTG; tg++) {
    size_t product = (nDim == 0) ? 0 : 1;
    size_t sum = 0;
    for (size_t dim = 0; dim < nDim; dim++) {
      const GPCntType nNodes = table.nNodes(dim, lvls[tg * nDim + dim]);
      product *= nNodes;
      sum += nNodes;
    }
    sizeClasses[tg] = tools::sparse_grid::sizeClassOf(product);
    maxGPCnt = std::max(maxGPCnt, product);
    maxSum = std::max(maxSum, sum);
    totalGPCnt += product;
  }

  maxTGGPCnt_ = maxGPCnt;
  maxTGSumOverGPCntsPerDim_ = maxSum;
  tgProcessingOrder_ = tools::sparse_grid::largestFirstOrder(sizeClasses, totalGPCnt);
}

}  // namespace combigrid
}  // namespace sgpp
