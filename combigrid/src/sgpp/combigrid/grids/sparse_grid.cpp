#include <cassert>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

/**********
Constructor
**********/

SparseGrid::SparseGrid(const size_t nDim) : nDim_(nDim), tensorGridData() {}

SparseGrid::SparseGrid(const size_t nDim, const size_t nTG) : nDim_(nDim), tensorGridData(nTG) {}

SparseGrid::SparseGrid(const SGGenInstr& genInstr) : nDim_(genInstr.nDim()) {
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> p = genInstr.genMIVecWithCoeff();

  tensorGridData.resize(p.first.nMI());

  tools::populateSG(genInstr, p.first, p.second, *this);
}

/*****
Getter
*****/

size_t SparseGrid::nDim() const { return nDim_; }

size_t SparseGrid::nTG() const { return tensorGridData.size(); }

const TensorGridCTData& SparseGrid::getTensorGrid(size_t idx) const { return tensorGridData[idx]; }

const TensorGridCTData& SparseGrid::getTensorGrid(const LvlMI& mi) const {
  // TODO
  throw base::not_implemented_exception("Operation is not implemented yet!");
}

const std::vector<TensorGridCTData>& SparseGrid::getTensorGrids() const { return tensorGridData; }

/*****
Setter
*****/

void SparseGrid::addTensorGrid(const TensorGridCTData& tg) { tensorGridData.push_back(tg); }

void SparseGrid::addTensorGrid(TensorGridCTData&& tg) { tensorGridData.push_back(std::move(tg)); }

void SparseGrid::setTensorGrid(const size_t idx, const TensorGridCTData& tg) {
  assert(idx < tensorGridData.size());

  tensorGridData[idx] = TensorGridCTData(tg);
}

void SparseGrid::setTensorGrid(const size_t idx, TensorGridCTData&& tg) {
  assert(idx < tensorGridData.size());

  tensorGridData[idx] = std::move(tg);
}

}  // namespace combigrid
}  // namespace sgpp
