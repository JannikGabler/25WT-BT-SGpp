#include <algorithm>
#include <cassert>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>

namespace sgpp {
namespace combigrid {

using iterator = std::vector<TensorGridCTData>::iterator;
using const_iterator = std::vector<TensorGridCTData>::const_iterator;
using reverse_iterator = std::vector<TensorGridCTData>::reverse_iterator;
using const_reverse_iterator = std::vector<TensorGridCTData>::const_reverse_iterator;

/*****************
Public Constructor
*****************/
SparseGrid::SparseGrid(const size_t nDim) : nDim_(nDim) {}

SparseGrid::SparseGrid(const size_t nDim, const size_t nTG) : nDim_(nDim), tensorGridData(nTG) {}

ExtensiveSparseGrid::ExtensiveSparseGrid(const SGGenInstr& genInstr) : SparseGrid(genInstr.nDim()) {
  const auto [miVec, coeffs] = genInstr.genFullMIVecWithCoeffs();

  tools::populateSG(*this, genInstr, miVec, coeffs);
}

/************
Public Getter
************/
size_t ExtensiveSparseGrid::nTGWithCoeffZero() const noexcept {
  return tensorGridWithCoeffZeroData.size();
}

const TensorGridCTData& ExtensiveSparseGrid::getTensorGridWithCoeffZero(size_t idx) const noexcept {
  assert(idx >= 0 && idx < tensorGridWithCoeffZeroData.size());

  return tensorGridWithCoeffZeroData[idx];
}

const_iterator ExtensiveSparseGrid::getTensorGridWithCoeffZero(const LvlMI& mi) const noexcept {
  return std::find_if(tensorGridWithCoeffZeroData.begin(), tensorGridWithCoeffZeroData.end(),
                      [&mi](const TensorGridCTData& data) { return data.mi == mi; });
}

std::span<const TensorGridCTData> ExtensiveSparseGrid::getTensorGridsWithCoeffZero()
    const noexcept {
  return tensorGridWithCoeffZeroData;
}

size_t ExtensiveSparseGrid::getMaxTGWithCoeffZeroGPCnt() const noexcept {
  return maxTGWithCoeffZeroGPCnt;
}

size_t ExtensiveSparseGrid::getMaxTGWithCoeffZeroSumOverGPCntsPerDim() const noexcept {
  return maxTGWithCoeffZeroSumOverGPCntsPerDim;
}

/************
Public Setter
************/

void ExtensiveSparseGrid::setTensorGridsWithCoeffZero(std::vector<TensorGridCTData>&& tgs) {
  assert(std::all_of(tgs.begin(), tgs.end(), [this](const TensorGridCTData& tg) {
    return tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_;
  }));

  tensorGridWithCoeffZeroData = std::move(tgs);
  recomputeMetaValues();
}

void ExtensiveSparseGrid::addTensorGridWithCoeffZero(const TensorGridCTData& tg) {
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  tensorGridWithCoeffZeroData.push_back(tg);
  updateMetaValuesAfterInsertion(tg);
}

void ExtensiveSparseGrid::addTensorGridWithCoeffZero(TensorGridCTData&& tg) {
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  tensorGridWithCoeffZeroData.push_back(std::move(tg));
  updateMetaValuesAfterInsertion(tensorGridWithCoeffZeroData.back());
}

void ExtensiveSparseGrid::setTensorGridWithCoeffZero(const size_t idx, const TensorGridCTData& tg) {
  assert(idx >= 0 && idx < tensorGridWithCoeffZeroData.size());
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  updateMetaValuesBeforeSwap(tensorGridWithCoeffZeroData[idx], tg);
  tensorGridWithCoeffZeroData[idx] = TensorGridCTData(tg);
}

void ExtensiveSparseGrid::setTensorGridWithCoeffZero(const size_t idx, TensorGridCTData&& tg) {
  assert(idx >= 0 && idx < tensorGridWithCoeffZeroData.size());
  assert(tg.tensorGrid.nDim() == nDim_ && tg.mi.nDim() == nDim_);

  updateMetaValuesBeforeSwap(tensorGridWithCoeffZeroData[idx], tg);
  tensorGridWithCoeffZeroData[idx] = std::move(tg);
}

/************
Public Helper
************/
void ExtensiveSparseGrid::resize(const size_t nTGWithCoeffNonZero, const size_t nTGWithCoeffZero) {
  SparseGrid::resize(nTGWithCoeffNonZero);
  tensorGridWithCoeffZeroData.resize(nTGWithCoeffZero);
}

void ExtensiveSparseGrid::reserve(const size_t nCoeffNonZero, const size_t nCoeffZero) {
  SparseGrid::reserve(nCoeffNonZero);
  tensorGridWithCoeffZeroData.reserve(nCoeffZero);
}

/**************
Public Iterator
**************/
iterator ExtensiveSparseGrid::begin() noexcept { return tensorGridWithCoeffZeroData.begin(); }
iterator ExtensiveSparseGrid::end() noexcept { return tensorGridWithCoeffZeroData.end(); }
const_iterator ExtensiveSparseGrid::begin() const noexcept {
  return tensorGridWithCoeffZeroData.begin();
}
const_iterator ExtensiveSparseGrid::end() const noexcept {
  return tensorGridWithCoeffZeroData.end();
}
const_iterator ExtensiveSparseGrid::cbegin() const noexcept {
  return tensorGridWithCoeffZeroData.cbegin();
}
const_iterator ExtensiveSparseGrid::cend() const noexcept {
  return tensorGridWithCoeffZeroData.cend();
}

reverse_iterator ExtensiveSparseGrid::rbegin() noexcept {
  return tensorGridWithCoeffZeroData.rbegin();
}
reverse_iterator ExtensiveSparseGrid::rend() noexcept { return tensorGridWithCoeffZeroData.rend(); }
const_reverse_iterator ExtensiveSparseGrid::rbegin() const noexcept {
  return tensorGridWithCoeffZeroData.rbegin();
}
const_reverse_iterator ExtensiveSparseGrid::rend() const noexcept {
  return tensorGridWithCoeffZeroData.rend();
}
const_reverse_iterator ExtensiveSparseGrid::crbegin() const noexcept {
  return tensorGridWithCoeffZeroData.crbegin();
}
const_reverse_iterator ExtensiveSparseGrid::crend() const noexcept {
  return tensorGridWithCoeffZeroData.crend();
}

/**************
Public Operator
**************/
bool ExtensiveSparseGrid::operator==(const ExtensiveSparseGrid& other) const {
  if (!SparseGrid::operator==(other)) {
    return false;
  }

  for (const TensorGridCTData& data : tensorGridWithCoeffZeroData) {
    const auto iter = std::find(other.tensorGridWithCoeffZeroData.begin(),
                                other.tensorGridWithCoeffZeroData.end(), data);

    if (iter == other.tensorGridWithCoeffZeroData.end()) {
      return false;
    }
  }

  return true;
}

/***************
Protected Helper
***************/
void ExtensiveSparseGrid::recomputeMetaValues() {
  maxTGWithCoeffZeroGPCnt = 0;
  maxTGWithCoeffZeroSumOverGPCntsPerDim = 0;

  for (const TensorGridCTData& tgData : tensorGridWithCoeffZeroData) {
    updateMetaValuesAfterInsertion(tgData);
  }
}

void ExtensiveSparseGrid::updateMetaValuesAfterInsertion(const TensorGridCTData& tg) {
  maxTGWithCoeffZeroGPCnt = std::max(maxTGWithCoeffZeroGPCnt, tg.tensorGrid.nGP());
  maxTGWithCoeffZeroSumOverGPCntsPerDim =
      std::max(maxTGWithCoeffZeroSumOverGPCntsPerDim, tg.tensorGrid.getNodesPerDim().size());
}

void ExtensiveSparseGrid::updateMetaValuesBeforeSwap(const TensorGridCTData& oldTG,
                                                     const TensorGridCTData& newTG) {
  if (oldTG.tensorGrid.nGP() == maxTGWithCoeffZeroGPCnt ||
      oldTG.tensorGrid.getNodesPerDim().size() == maxTGWithCoeffZeroSumOverGPCntsPerDim) {
    recomputeMetaValues();
  } else {
    updateMetaValuesAfterInsertion(newTG);
  }
}

}  // namespace combigrid
}  // namespace sgpp