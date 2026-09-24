#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/grids/extensive_sparse_grid.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

/**********
Constructor
**********/

ExtensiveSparseGrid::ExtensiveSparseGrid(const SGGenInstr& genInstr) : SparseGrid(genInstr.nDim()) {
  const auto [lvlMIVec, coeffs] = genInstr.genFullMIVecWithCoeffs();

  tensorGridData.resize(lvlMIVec.nMI());
  tools::populateSG(genInstr, lvlMIVec, coeffs, *this);

  const auto part2Start =
      std::stable_partition(tensorGridData.begin(), tensorGridData.end(),
                            [](const TensorGridCTData& tg) { return tg.coefficient == 0; });
  tensorGridDataStartIdx = static_cast<size_t>(std::distance(tensorGridData.begin(), part2Start));
}

/*****
Getter
*****/

size_t ExtensiveSparseGrid::nTGIncludingZeroCoeffs() const { return tensorGridData.size(); }

const TensorGridCTData& ExtensiveSparseGrid::getTensorGridIncludingZeroCoeffs(
    const size_t idx) const {
  assert(idx >= 0 && idx < nTGIncludingZeroCoeffs());

  return tensorGridData[idx];
}

SparseGrid::const_iterator ExtensiveSparseGrid::findTensorGridIncludingZeroCoeffs(
    const LvlMI& mi) const {
  return std::ranges::find(beginIncludingZeroCoeffs(), endIncludingZeroCoeffs(), mi,
                           &TensorGridCTData::mi);
}

std::span<TensorGridCTData> ExtensiveSparseGrid::getTensorGridsIncludingZeroCoeffs() {
  return tensorGridData;
}
std::span<const TensorGridCTData> ExtensiveSparseGrid::getTensorGridsIncludingZeroCoeffs() const {
  return tensorGridData;
}

std::span<TensorGridCTData> ExtensiveSparseGrid::getTensorGridsWithCoeffZero() noexcept {
  return std::span<TensorGridCTData>(tensorGridData).first(tensorGridDataStartIdx);
}
std::span<const TensorGridCTData> ExtensiveSparseGrid::getTensorGridsWithCoeffZero()
    const noexcept {
  return std::span<const TensorGridCTData>(tensorGridData).first(tensorGridDataStartIdx);
}

/*****
Setter
*****/

void ExtensiveSparseGrid::addTensorGrid(const TensorGridCTData& tg) {
  if (tg.coefficient == 0) {
    tensorGridData.insert(tensorGridData.begin() + static_cast<ptrdiff_t>(tensorGridDataStartIdx),
                          tg);
    tensorGridDataStartIdx++;
  } else {
    SparseGrid::addTensorGrid(tg);
  }
}

void ExtensiveSparseGrid::addTensorGrid(TensorGridCTData&& tg) {
  if (tg.coefficient == 0) {
    tensorGridData.insert(tensorGridData.begin() + static_cast<ptrdiff_t>(tensorGridDataStartIdx),
                          std::move(tg));
    tensorGridDataStartIdx++;
  } else {
    SparseGrid::addTensorGrid(std::move(tg));
  }
}

/*******
Iterator
*******/
/// @name Iterators over all tensor grids (including c == 0).
/// @{
SparseGrid::iterator ExtensiveSparseGrid::beginIncludingZeroCoeffs() noexcept {
  return tensorGridData.begin();
}
SparseGrid::iterator ExtensiveSparseGrid::endIncludingZeroCoeffs() noexcept {
  return tensorGridData.end();
}
SparseGrid::const_iterator ExtensiveSparseGrid::beginIncludingZeroCoeffs() const noexcept {
  return tensorGridData.begin();
}
SparseGrid::const_iterator ExtensiveSparseGrid::endIncludingZeroCoeffs() const noexcept {
  return tensorGridData.end();
}
SparseGrid::const_iterator ExtensiveSparseGrid::cbeginIncludingZeroCoeffs() const noexcept {
  return tensorGridData.cbegin();
}
SparseGrid::const_iterator ExtensiveSparseGrid::cendIncludingZeroCoeffs() const noexcept {
  return tensorGridData.cend();
}

SparseGrid::reverse_iterator ExtensiveSparseGrid::rbeginIncludingZeroCoeffs() noexcept {
  return tensorGridData.rbegin();
}
SparseGrid::reverse_iterator ExtensiveSparseGrid::rendIncludingZeroCoeffs() noexcept {
  return tensorGridData.rend();
}
SparseGrid::const_reverse_iterator ExtensiveSparseGrid::rbeginIncludingZeroCoeffs() const noexcept {
  return tensorGridData.rbegin();
}
SparseGrid::const_reverse_iterator ExtensiveSparseGrid::rendIncludingZeroCoeffs() const noexcept {
  return tensorGridData.rend();
}
SparseGrid::const_reverse_iterator ExtensiveSparseGrid::crbeginIncludingZeroCoeffs()
    const noexcept {
  return tensorGridData.crbegin();
}
SparseGrid::const_reverse_iterator ExtensiveSparseGrid::crendIncludingZeroCoeffs() const noexcept {
  return tensorGridData.crend();
}

/*******
Operator
*******/

bool ExtensiveSparseGrid::operator==(const ExtensiveSparseGrid& other) const {
  if (nDim() != other.nDim() || nTGIncludingZeroCoeffs() != other.nTGIncludingZeroCoeffs()) {
    return false;
  }

  const std::span<const TensorGridCTData> otherTGs = other.getTensorGridsIncludingZeroCoeffs();

  return std::ranges::all_of(getTensorGridsIncludingZeroCoeffs(),
                             [&otherTGs](const TensorGridCTData& data) {
                               return std::ranges::find(otherTGs, data) != otherTGs.end();
                             });
}

}  // namespace combigrid
}  // namespace sgpp