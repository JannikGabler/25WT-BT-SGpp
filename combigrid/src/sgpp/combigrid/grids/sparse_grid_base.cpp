// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
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

SparseGridBase::SparseGridBase(const size_t nDim) : nDim_(nDim), levels_(nDim, 0) {}

void SparseGridBase::setStorage(LvlMIVec levels, std::vector<CTCoeffType> coefficients,
                                NodeTable domainNodeTable, NodeTable unitNodeTable) {
  assert(levels.nDim() == nDim_ && domainNodeTable.nDim() == nDim_ &&
         unitNodeTable.nDim() == nDim_);
  assert(levels.size() == coefficients.size());

  levels_ = std::move(levels);
  coefficients_ = std::move(coefficients);
  domainNodeTable_ = std::move(domainNodeTable);
  unitNodeTable_ = std::move(unitNodeTable);
  computeMetaValues();
}

/**
 * One pass over the contiguous levels (in parallel: a deltas sparse grid in high dimensions has
 * hundreds of thousands of tensor grids), with the counts read from the node table.
 */
void SparseGridBase::computeMetaValues() {
  const size_t nDim = nDim_;
  const size_t nTG = this->nTG();
  const LvlType* const lvls = levels_.data();
  const NodeTable& table = domainNodeTable_;
  size_t maxGPCnt = 0;
  size_t maxSum = 0;
  const bool parallel = nTG >= constants::sparse_grid_storage::MIN_TG_FOR_CONCURRENCY;

#pragma omp parallel for schedule(static) reduction(max : maxGPCnt, maxSum) if (parallel)
  for (size_t tg = 0; tg < nTG; tg++) {
    size_t product = (nDim == 0) ? 0 : 1;
    size_t sum = 0;
    for (size_t dim = 0; dim < nDim; dim++) {
      const GPCntType nNodes = table.nNodes(dim, lvls[tg * nDim + dim]);
      product *= nNodes;
      sum += nNodes;
    }
    maxGPCnt = std::max(maxGPCnt, product);
    maxSum = std::max(maxSum, sum);
  }

  maxTGGPCnt_ = maxGPCnt;
  maxTGSumOverGPCntsPerDim_ = maxSum;
}

}  // namespace combigrid
}  // namespace sgpp
