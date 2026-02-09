
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <vector>

// bool sgpp::combigrid::TensorGrid::containsABoundary() {
//     return this->gridPoints
// }

namespace sgpp {
namespace combigrid {

TensorGrid::TensorGrid(const MI& nGPPerDim)
    : nGPPerDim(nGPPerDim),
      gridPoints(nGPPerDim.productofElems(), base::DataVector(nGPPerDim.size())) {}

TensorGrid::TensorGrid(const MI& nGPPerDim, const std::vector<base::DataVector>& gridPoints)
    : nGPPerDim(nGPPerDim), gridPoints(gridPoints) {
  assert(nGPPerDim.productofElems() != gridPoints.size());
}

size_t TensorGrid::nDim() const { return this->nGPPerDim.size(); }

const base::DataVector& TensorGrid::getGridPoint(const size_t idx) const {
  return this->gridPoints[idx];
}

const base::DataVector& TensorGrid::getGridPoint(const MI& mi) const {
  assert(mi < this->nGPPerDim);
  return this->gridPoints[mi.toLinearIndex()];
}

const std::vector<base::DataVector>& TensorGrid::getGridPoints() const { return this->gridPoints; }

void TensorGrid::setGridPoint(const size_t idx, base::DataVector gp) { this->gridPoints[idx] = gp; }

void TensorGrid::setGridPoint(const MI& mi, base::DataVector gp) {
  this->gridPoints[mi.toLinearIndex()] = gp;
}

}  // namespace combigrid
}  // namespace sgpp
