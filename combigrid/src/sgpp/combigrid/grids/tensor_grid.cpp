
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/**********
Constructor
**********/
TensorGrid::TensorGrid() noexcept : nGP_(0), nGPPerDim(), nodesPerDim(0) {}

TensorGrid::TensorGrid(const GPMI& nGPPerDim) noexcept
    : nGP_(nGPPerDim.productofElems<size_t>()),
      nGPPerDim(nGPPerDim),
      nodesPerDim(nGPPerDim.sumOfElems<size_t>()) {}

TensorGrid::TensorGrid(const GPMI& nGPPerDim, base::DataVector&& nodesPerDim) noexcept
    : nGP_(nGPPerDim.productofElems<size_t>()),
      nGPPerDim(nGPPerDim),
      nodesPerDim(std::move(nodesPerDim)) {
  assert(this->nodesPerDim.size() == nGPPerDim.sumOfElems<size_t>());
}

/*****
Getter
*****/

size_t TensorGrid::nDim() const { return this->nGPPerDim.size(); }

size_t TensorGrid::nGP() const { return nGP_; }

const GPMI& TensorGrid::getGPCntPerDim() const { return nGPPerDim; }

void TensorGrid::getGridPoint(size_t idx, base::DataVector& out) const {
  assert(idx < nGP_ && out.size() >= nDim());

  const size_t nDim = this->nDim();
  size_t vectorOffset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const GPCntType nGPInDim = nGPPerDim[dim];

    const size_t localIdx = idx % nGPInDim;
    idx /= nGPInDim;

    out[dim] = nodesPerDim[vectorOffset + localIdx];
    vectorOffset += nGPInDim;
  }
}

// base::DataVector TensorGrid::getGridPoint(const size_t idx) const {
//   assert(idx < nGP_);

//   const size_t nDim = this->nDim();
//   base::DataVector gp(nDim);

//   // for (size_t dim = 0; dim < nDim; dim++) {
//   //   gp[dim] = gridPoints[nDim * idx + dim];
//   // }
//   std::copy_n(gridPoints.begin() + idx * nDim, nDim, gp.begin());

//   return gp;
// }

void TensorGrid::getGridPoint(const GPMI& mi, base::DataVector& out) const {
  assert(mi.size() >= nDim() && out.size() >= nDim() && mi < nGPPerDim);

  const size_t nDim = this->nDim();
  size_t vectorOffset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const GPCntType nGPInDim = nGPPerDim[dim];
    const size_t localIdx = mi[dim];

    out[dim] = nodesPerDim[vectorOffset + localIdx];
    vectorOffset += nGPInDim;
  }
}

void TensorGrid::getGridPointAndMI(size_t idx, base::DataVector& outGP, GPMI& outMI) const {
  assert(idx < nGP_ && outGP.size() >= nDim() && outMI.size() >= nDim());

  const size_t nDim = this->nDim();
  size_t vectorOffset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const GPCntType nGPInDim = nGPPerDim[dim];

    const size_t localIdx = idx % nGPInDim;
    idx /= nGPInDim;

    outGP[dim] = nodesPerDim[vectorOffset + localIdx];
    outMI[dim] = static_cast<GPCntType>(localIdx);

    vectorOffset += nGPInDim;
  }
}

// base::DataVector TensorGrid::getGridPoint(const GPMI& mi) const {
//   assert(mi.size() == nDim());

//   return getGridPoint(mi.toLinearIndex(nGPPerDim));
// }

/*****
Setter
*****/

// void TensorGrid::setGridPoint(const size_t idx, const base::DataVector& gp) {
//   assert(idx < nGP_);
//   assert(gp.size() == nDim());

//   const size_t nDim = this->nDim();

//   // for (size_t dim = 0; dim < nDim; dim++) {
//   //   gridPoints[idx * nDim + dim] = gp[dim];
//   // }
//   std::copy_n(gp.begin(), nDim, gridPoints.begin() + idx * nDim);
// }

// void TensorGrid::setGridPoint(const GPMI& mi, const base::DataVector& gp) {
//   assert(mi.nDim() == nDim());

//   setGridPoint(mi.toLinearIndex(nGPPerDim), gp);
// }

/********
Operators
********/

// base::DataVector TensorGrid::operator[](const size_t idx) const { return getGridPoint(idx); }

// double TensorGrid::operator()(const size_t idx, const size_t dim) const {
//   assert(idx < nGP_);
//   assert(dim < nDim());

//   return gridPoints[idx * nDim() + dim];
// }

// double& TensorGrid::operator()(const size_t idx, const size_t dim) {
//   assert(idx < nGP_);
//   assert(dim < nDim());

//   return gridPoints[idx * nDim() + dim];
// }

bool TensorGrid::operator==(const TensorGrid& other) const {
  return nGP() == other.nGP() && nGPPerDim == other.nGPPerDim &&
         tools::nearlyEqual(nodesPerDim, other.nodesPerDim);
}

}  // namespace combigrid
}  // namespace sgpp
