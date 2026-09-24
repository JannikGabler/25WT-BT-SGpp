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

/**********
Constructor
**********/

SparseGrid::SparseGrid(const size_t nDim)
    : nDim_(nDim),
      tensorGridDataStartIdx(0),
      tensorGridData(),
      genInstr(),
      maxTGGPCnt(0),
      maxTGSumOverGPCntsPerDim(0) {}

SparseGrid::SparseGrid(const size_t nDim, const size_t nTG)
    : nDim_(nDim),
      tensorGridDataStartIdx(0),
      tensorGridData(nTG),
      genInstr(),
      maxTGGPCnt(0),
      maxTGSumOverGPCntsPerDim(0) {}

SparseGrid::SparseGrid(const SGGenInstr& genInstr)
    : nDim_(genInstr.nDim()), tensorGridDataStartIdx(0), genInstr(genInstr.clone()) {
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> p = genInstr.genReducedMIVecWithCoeffs();

  tensorGridData.resize(p.first.nMI());
  tools::populateSG(genInstr, p.first, p.second, *this);
}

/*****
Getter
*****/

size_t SparseGrid::nDim() const { return nDim_; }

size_t SparseGrid::nTG() const { return tensorGridData.size() - tensorGridDataStartIdx; }

const TensorGridCTData& SparseGrid::getTensorGrid(const size_t idx) const {
  assert(idx >= 0 && idx < nTG());
  return tensorGridData[tensorGridDataStartIdx + idx];
}

SparseGrid::const_iterator SparseGrid::getTensorGrid(const LvlMI& mi) const {
  return std::ranges::find(begin(), end(), mi, &TensorGridCTData::mi);
}

const std::vector<TensorGridCTData>& SparseGrid::getTensorGrids() const { return tensorGridData; }

const std::shared_ptr<const SGGenInstr> SparseGrid::getGenInstr() const { return genInstr; }

size_t SparseGrid::getMaxTGGPCnt() const { return maxTGGPCnt; }

size_t SparseGrid::getMaxTGSumOverGPCntsPerDim() const { return maxTGSumOverGPCntsPerDim; }

/*****
Setter
*****/

void SparseGrid::addTensorGrid(const TensorGridCTData& tg) {
  assert(tg.coefficient != 0);

  tensorGridData.push_back(tg);
}

void SparseGrid::addTensorGrid(TensorGridCTData&& tg) {
  assert(tg.coefficient != 0);

  tensorGridData.push_back(std::move(tg));
}

void SparseGrid::setTensorGrid(const size_t idx, const TensorGridCTData& tg) {
  assert(idx >= 0 && idx < nTG());
  assert(tg.coefficient != 0);

  tensorGridData[tensorGridDataStartIdx + idx] = TensorGridCTData(tg);
}

void SparseGrid::setTensorGrid(const size_t idx, TensorGridCTData&& tg) {
  assert(idx >= 0 && idx < nTG());
  assert(tg.coefficient != 0);

  tensorGridData[tensorGridDataStartIdx + idx] = std::move(tg);
}

void SparseGrid::setGenInstr(const SGGenInstr& genInstr) {
  assert(genInstr.nDim() == nDim_);

  this->genInstr = genInstr.clone();
}

void SparseGrid::setGenInstr(std::shared_ptr<const SGGenInstr>&& genInstr) {
  assert(genInstr->nDim() == nDim_);

  this->genInstr = std::move(genInstr);
}

void SparseGrid::setMaxTGGPCnt(const size_t maximum) { maxTGGPCnt = maximum; }

void SparseGrid::setMaxTGSumOverGPCntsPerDim(const size_t maximum) {
  maxTGSumOverGPCntsPerDim = maximum;
}

/*******
Iterator
*******/
SparseGrid::iterator SparseGrid::begin() noexcept {
  return tensorGridData.begin() + static_cast<std::ptrdiff_t>(tensorGridDataStartIdx);
}

SparseGrid::iterator SparseGrid::end() noexcept { return tensorGridData.end(); }

SparseGrid::const_iterator SparseGrid::begin() const noexcept {
  return tensorGridData.begin() + static_cast<std::ptrdiff_t>(tensorGridDataStartIdx);
}

SparseGrid::const_iterator SparseGrid::end() const noexcept { return tensorGridData.end(); }

SparseGrid::const_iterator SparseGrid::cbegin() const noexcept { return begin(); }

SparseGrid::const_iterator SparseGrid::cend() const noexcept { return end(); }

SparseGrid::reverse_iterator SparseGrid::rbegin() noexcept { return reverse_iterator(end()); }

SparseGrid::reverse_iterator SparseGrid::rend() noexcept { return reverse_iterator(begin()); }

SparseGrid::const_reverse_iterator SparseGrid::rbegin() const noexcept {
  return const_reverse_iterator(end());
}

SparseGrid::const_reverse_iterator SparseGrid::rend() const noexcept {
  return const_reverse_iterator(begin());
}

SparseGrid::const_reverse_iterator SparseGrid::crbegin() const noexcept { return rbegin(); }

SparseGrid::const_reverse_iterator SparseGrid::crend() const noexcept { return rend(); }

/*******
Operator
*******/

bool SparseGrid::operator==(const SparseGrid& other) const {
  if (nDim() != other.nDim() || nTG() != other.nTG()) {
    return false;
  }

  const std::span<const TensorGridCTData> otherTGs = other.getTensorGrids();

  return std::ranges::all_of(getTensorGrids(), [&otherTGs](const TensorGridCTData& data) {
    return std::ranges::find(otherTGs, data) != otherTGs.end();
  });
}

}  // namespace combigrid
}  // namespace sgpp
