#include <algorithm>
#include <cassert>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>

namespace sgpp {
namespace combigrid {

using iterator = std::vector<TensorGridCTData>::iterator;
using const_iterator = std::vector<TensorGridCTData>::const_iterator;
using reverse_iterator = std::vector<TensorGridCTData>::reverse_iterator;
using const_reverse_iterator = std::vector<TensorGridCTData>::const_reverse_iterator;

/// @name Getters
/// @{

size_t SparseGridBase::nDim() const noexcept { return nDim_; }

size_t SparseGridBase::nTG() const noexcept { return tensorGridData.size(); }

const TensorGridCTData& SparseGridBase::getTensorGrid(size_t idx) const noexcept {
  assert(idx < tensorGridData.size());

  return tensorGridData[idx];
}

const_iterator SparseGridBase::getTensorGrid(const LvlMI& mi) const noexcept {
  return std::find_if(tensorGridData.begin(), tensorGridData.end(),
                      [&mi](const TensorGridCTData& data) { return data.mi == mi; });
}

std::span<const TensorGridCTData> SparseGridBase::getTensorGrids() const noexcept {
  return tensorGridData;
}

size_t SparseGridBase::getMaxTGGPCnt() const noexcept { return maxTGGPCnt; }

size_t SparseGridBase::getMaxTGSumOverGPCntsPerDim() const noexcept {
  return maxTGSumOverGPCntsPerDim;
}

/// @}

/// @name Iterators
/// @{

iterator SparseGridBase::begin() noexcept { return tensorGridData.begin(); }
iterator SparseGridBase::end() noexcept { return tensorGridData.end(); }
const_iterator SparseGridBase::begin() const noexcept { return tensorGridData.begin(); }
const_iterator SparseGridBase::end() const noexcept { return tensorGridData.end(); }
const_iterator SparseGridBase::cbegin() const noexcept { return tensorGridData.cbegin(); }
const_iterator SparseGridBase::cend() const noexcept { return tensorGridData.cend(); }

reverse_iterator SparseGridBase::rbegin() noexcept { return tensorGridData.rbegin(); }
reverse_iterator SparseGridBase::rend() noexcept { return tensorGridData.rend(); }
const_reverse_iterator SparseGridBase::rbegin() const noexcept { return tensorGridData.rbegin(); }
const_reverse_iterator SparseGridBase::rend() const noexcept { return tensorGridData.rend(); }
const_reverse_iterator SparseGridBase::crbegin() const noexcept { return tensorGridData.crbegin(); }
const_reverse_iterator SparseGridBase::crend() const noexcept { return tensorGridData.crend(); }

/// @}

/// @name Operators
/// @{

bool SparseGridBase::operator==(const SparseGridBase& other) const {
  if (nDim() != other.nDim() || nTG() != other.nTG()) {
    return false;
  }

  for (const TensorGridCTData& data : tensorGridData) {
    const auto iter = std::find(other.tensorGridData.begin(), other.tensorGridData.end(), data);

    if (iter == other.tensorGridData.end()) {
      return false;
    }
  }

  return true;
}

/// @}

/// @name Protected Constructors
/// @{

SparseGridBase::SparseGridBase(const size_t nDim) : nDim_(nDim) {}

SparseGridBase::SparseGridBase(const size_t nDim, const size_t nTG)
    : nDim_(nDim), tensorGridData(nTG) {}

/// @}

/// @name Protected setters
/// @{

void SparseGridBase::setTensorGrids(std::vector<TensorGridCTData>&& tgs) {
  assert(std::all_of(tgs.begin(), tgs.end(), [this](const TensorGridCTData& tg) {
    return tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_;
  }));

  tensorGridData = std::move(tgs);
  recomputeMetaValues();
}

void SparseGridBase::addTensorGrid(const TensorGridCTData& tg) {
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  tensorGridData.push_back(tg);
  updateMetaValuesAfterInsertion(tg);
}

void SparseGridBase::addTensorGrid(TensorGridCTData&& tg) {
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  tensorGridData.push_back(std::move(tg));
  updateMetaValuesAfterInsertion(tensorGridData.back());
}

void SparseGridBase::setTensorGrid(const size_t idx, const TensorGridCTData& tg) {
  assert(idx < tensorGridData.size());
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  updateMetaValuesBeforeSwap(tensorGridData[idx], tg);
  tensorGridData[idx] = TensorGridCTData(tg);
}

void SparseGridBase::setTensorGrid(const size_t idx, TensorGridCTData&& tg) {
  assert(idx < tensorGridData.size());
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  updateMetaValuesBeforeSwap(tensorGridData[idx], tg);
  tensorGridData[idx] = std::move(tg);
}

// void SparseGrid::setMaxTGGPCnt(const size_t maximum) { maxTGGPCnt = maximum; }

// void SparseGrid::setMaxTGSumOverGPCntsPerDim(const size_t maximum) {
//   maxTGSumOverGPCntsPerDim = maximum;
// }

/// @}

/// @name Protected helpers
/// @{

void SparseGridBase::resize(size_t nTG) { tensorGridData.resize(nTG); }

void SparseGridBase::reserve(const size_t n) { tensorGridData.reserve(n); }

void SparseGridBase::recomputeMetaValues() {
  maxTGGPCnt = 0;
  maxTGSumOverGPCntsPerDim = 0;

  for (const TensorGridCTData& tgData : tensorGridData) {
    updateMetaValuesAfterInsertion(tgData);
  }
}

void SparseGridBase::updateMetaValuesAfterInsertion(const TensorGridCTData& tg) {
  maxTGGPCnt = std::max(maxTGGPCnt, tg.tensorGrid.nGP());
  maxTGSumOverGPCntsPerDim =
      std::max(maxTGSumOverGPCntsPerDim, tg.tensorGrid.getNodesPerDim().size());
}

void SparseGridBase::updateMetaValuesBeforeSwap(const TensorGridCTData& oldTG,
                                                const TensorGridCTData& newTG) {
  if (oldTG.tensorGrid.nGP() == maxTGGPCnt ||
      oldTG.tensorGrid.getNodesPerDim().size() == maxTGSumOverGPCntsPerDim) {
    recomputeMetaValues();
  } else {
    updateMetaValuesAfterInsertion(newTG);
  }
}

/// @}

}  // namespace combigrid
}  // namespace sgpp