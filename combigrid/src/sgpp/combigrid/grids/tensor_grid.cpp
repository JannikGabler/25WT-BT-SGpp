
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

TensorGrid::TensorGrid(const GPMI& nGPPerDim)
    : nGP_(nGPPerDim.productofElems()), nGPPerDim(nGPPerDim), gridPoints(nGP_ * nGPPerDim.size()) {}

TensorGrid::TensorGrid(const GPMI& nGPPerDim, const std::vector<base::DataVector>& gridPoints)
    : TensorGrid(nGPPerDim) {
  assert(gridPoints.size() == nGP_);

  const size_t nDim = nGPPerDim.size();

  for (size_t i = 0; i < gridPoints.size(); i++) {
    for (size_t dim = 0; dim < nDim; dim++) {
      this->gridPoints[i * nDim + dim] = gridPoints[i][dim];
    }
  }
}

size_t TensorGrid::nDim() const { return this->nGPPerDim.size(); }

base::DataVector TensorGrid::getGridPoint(const size_t idx) const {
  assert(idx < nGP_);

  const size_t nDim = this->nDim();
  base::DataVector gp(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    gp[dim] = gridPoints[nDim * idx + dim];
  }

  return gp;
}

base::DataVector TensorGrid::getGridPoint(const GPMI& mi) const {
  assert(mi.size() == nDim());

  return getGridPoint(mi.toLinearIndex());
}

// const std::vector<base::DataVector>& TensorGrid::getGridPoints() const { return this->gridPoints;
// }

void TensorGrid::setGridPoint(const size_t idx, const base::DataVector& gp) {
  assert(idx < nGP_);
  assert(gp.size() == nDim());

  const size_t nDim = this->nDim();

  for (size_t dim = 0; dim < nDim; dim++) {
    gridPoints[idx * nDim + dim] = gp[dim];
  }
}

void TensorGrid::setGridPoint(const GPMI& mi, const base::DataVector& gp) {
  assert(mi.nDim() == nDim());

  setGridPoint(mi.toLinearIndex(), gp);
}

base::DataVector TensorGrid::operator[](const size_t idx) const { return getGridPoint(idx); }

double TensorGrid::operator()(const size_t idx, const size_t dim) const {
  assert(idx < nGP_);
  assert(dim < nDim());

  return gridPoints[idx * nDim() + dim];
}

double& TensorGrid::operator()(const size_t idx, const size_t dim) {
  assert(idx < nGP_);
  assert(dim < nDim());

  return gridPoints[idx * nDim() + dim];
}

}  // namespace combigrid
}  // namespace sgpp
