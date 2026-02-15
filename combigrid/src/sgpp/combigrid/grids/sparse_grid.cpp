#include "sgpp/combigrid/grids/sparse_grid.hpp"
#include "sgpp/base/exception/not_implemented_exception.hpp"

namespace sgpp {
namespace combigrid {

size_t SparseGrid::nDim() const { return nDim_; }

const TensorGrid& SparseGrid::getTensorGrid(size_t idx) const { return this->tensorGrids[idx]; }

const TensorGrid& SparseGrid::getTensorGrid(const MI& mi) const {
  // TODO
  throw base::not_implemented_exception("Operation is not implemented yet!");
}

const std::vector<TensorGrid>& SparseGrid::getTensorGrids() const { return this->tensorGrids; }

}  // namespace combigrid
}  // namespace sgpp
