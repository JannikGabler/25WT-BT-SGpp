#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

SparseGrid::SparseGrid(const size_t nDim) : nDim_(nDim), tensorGridData() {}

SparseGrid::SparseGrid(const SGGenInstr& genInstruction) : nDim_(genInstruction.nDim()) {
  // TODO
  throw base::not_implemented_exception("Function is not implemented yet!");
}

size_t SparseGrid::nDim() const { return nDim_; }

const TensorGridCTData& SparseGrid::getTensorGrid(size_t idx) const { return tensorGridData[idx]; }

const TensorGridCTData& SparseGrid::getTensorGrid(const MI& mi) const {
  // TODO
  throw base::not_implemented_exception("Operation is not implemented yet!");
}

const std::vector<TensorGridCTData>& SparseGrid::getTensorGrids() const { return tensorGridData; }

}  // namespace combigrid
}  // namespace sgpp
