#include <algorithm>
#include <cassert>
#include <memory>
#include <sgpp/base/exception/not_implemented_exception.hpp>
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

SparseGrid::SparseGrid(const SGGenInstr& genInstr)
    : nDim_(genInstr.nDim()), genInstr(genInstr.clone()) {
  const auto [miVec, coeffs] = genInstr.genReducedMIVecWithCoeffs();

  // tensorGridData.resize(miVec.nMI());
  tools::populateSG(*this, genInstr, miVec, coeffs);
}

/************
Public Getter
************/

size_t SparseGrid::nDim() const { return nDim_; }

size_t SparseGrid::nTG() const { return tensorGridData.size(); }

const TensorGridCTData& SparseGrid::getTensorGrid(size_t idx) const { return tensorGridData[idx]; }

const_iterator SparseGrid::getTensorGrid(const LvlMI& mi) const {
  return std::find_if(tensorGridData.begin(), tensorGridData.end(),
                      [&mi](const TensorGridCTData& data) { return data.mi == mi; });
}

std::span<const TensorGridCTData> SparseGrid::getTensorGrids() const { return tensorGridData; }

const std::shared_ptr<const SGGenInstr> SparseGrid::getGenInstr() const { return genInstr; }

size_t SparseGrid::getMaxTGGPCnt() const { return maxTGGPCnt; }

size_t SparseGrid::getMaxTGSumOverGPCntsPerDim() const { return maxTGSumOverGPCntsPerDim; }

/************
Public Setter
************/

void SparseGrid::setTensorGrids(std::vector<TensorGridCTData>&& tgs) {
  tensorGridData = std::move(tgs);
  recomputeMetaValues();
}

void SparseGrid::addTensorGrid(const TensorGridCTData& tg) {
  tensorGridData.push_back(tg);
  updateMetaValuesAfterInsertion(tg);
}

void SparseGrid::addTensorGrid(TensorGridCTData&& tg) {
  tensorGridData.push_back(std::move(tg));
  updateMetaValuesAfterInsertion(tensorGridData.back());
}

void SparseGrid::setTensorGrid(const size_t idx, const TensorGridCTData& tg) {
  assert(idx >= 0 && idx < tensorGridData.size());

  updateMetaValuesBeforeSwap(tensorGridData[idx], tg);
  tensorGridData[idx] = TensorGridCTData(tg);
}

void SparseGrid::setTensorGrid(const size_t idx, TensorGridCTData&& tg) {
  assert(idx >= 0 && idx < tensorGridData.size());

  updateMetaValuesBeforeSwap(tensorGridData[idx], tg);
  tensorGridData[idx] = std::move(tg);
}

// void SparseGrid::setGenInstr(const SGGenInstr& genInstr) {
//   assert(genInstr.nDim() == nDim_);

//   this->genInstr = genInstr.clone();
// }

// void SparseGrid::setGenInstr(std::shared_ptr<const SGGenInstr>&& genInstr) {
//   assert(genInstr->nDim() == nDim_);

//   this->genInstr = std::move(genInstr);
// }

// void SparseGrid::setMaxTGGPCnt(const size_t maximum) { maxTGGPCnt = maximum; }

// void SparseGrid::setMaxTGSumOverGPCntsPerDim(const size_t maximum) {
//   maxTGSumOverGPCntsPerDim = maximum;
// }

/************
Public Helper
************/
void SparseGrid::resize(size_t nTG) { tensorGridData.resize(nTG); }

void SparseGrid::reserve(const size_t n) { tensorGridData.reserve(n); }

/**************
Public Iterator
**************/
iterator SparseGrid::begin() noexcept { return tensorGridData.begin(); }
iterator SparseGrid::end() noexcept { return tensorGridData.end(); }
const_iterator SparseGrid::begin() const noexcept { return tensorGridData.begin(); }
const_iterator SparseGrid::end() const noexcept { return tensorGridData.end(); }
const_iterator SparseGrid::cbegin() const noexcept { return tensorGridData.cbegin(); }
const_iterator SparseGrid::cend() const noexcept { return tensorGridData.cend(); }

reverse_iterator SparseGrid::rbegin() noexcept { return tensorGridData.rbegin(); }
reverse_iterator SparseGrid::rend() noexcept { return tensorGridData.rend(); }
const_reverse_iterator SparseGrid::rbegin() const noexcept { return tensorGridData.rbegin(); }
const_reverse_iterator SparseGrid::rend() const noexcept { return tensorGridData.rend(); }
const_reverse_iterator SparseGrid::crbegin() const noexcept { return tensorGridData.crbegin(); }
const_reverse_iterator SparseGrid::crend() const noexcept { return tensorGridData.crend(); }

/**************
Public Operator
**************/

bool SparseGrid::operator==(const SparseGrid& other) const {
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

/***************
Protected Helper
***************/
void SparseGrid::recomputeMetaValues() {
  maxTGGPCnt = 0;
  maxTGSumOverGPCntsPerDim = 0;

  for (const TensorGridCTData& tgData : tensorGridData) {
    updateMetaValuesAfterInsertion(tgData);
  }
}

void SparseGrid::updateMetaValuesAfterInsertion(const TensorGridCTData& tg) {
  maxTGGPCnt = std::max(maxTGGPCnt, tg.tensorGrid.nGP());
  maxTGSumOverGPCntsPerDim =
      std::max(maxTGSumOverGPCntsPerDim, tg.tensorGrid.getNodesPerDim().size());
}

void SparseGrid::updateMetaValuesBeforeSwap(const TensorGridCTData& oldTG,
                                            const TensorGridCTData& newTG) {
  if (oldTG.tensorGrid.nGP() == maxTGGPCnt ||
      oldTG.tensorGrid.getNodesPerDim().size() == maxTGSumOverGPCntsPerDim) {
    recomputeMetaValues();
  } else {
    updateMetaValuesAfterInsertion(newTG);
  }
}

}  // namespace combigrid
}  // namespace sgpp