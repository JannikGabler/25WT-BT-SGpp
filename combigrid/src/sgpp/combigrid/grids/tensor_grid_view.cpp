// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

size_t TensorGridView::nGP() const noexcept {
  if (nDim_ == 0) {
    return 0;  // As TensorGrid
  }

  size_t result = 1;
  for (size_t dim = 0; dim < nDim_; dim++) {
    result *= nGPInDim(dim);
  }
  return result;
}

size_t TensorGridView::sumOfGPCntsPerDim() const noexcept {
  size_t result = 0;
  for (size_t dim = 0; dim < nDim_; dim++) {
    result += nGPInDim(dim);
  }
  return result;
}

GPMI TensorGridView::getGPCntPerDim() const {
  GPMI result(nDim_);
  for (size_t dim = 0; dim < nDim_; dim++) {
    result[dim] = nGPInDim(dim);
  }
  return result;
}

base::DataVector TensorGridView::getNodesPerDim() const {
  base::DataVector result(sumOfGPCntsPerDim());
  double* out = result.data();
  for (size_t dim = 0; dim < nDim_; dim++) {
    out = std::copy_n(nodesInDim(dim), nGPInDim(dim), out);
  }
  return result;
}

void TensorGridView::getGridPoint(size_t idx, base::DataVector& out) const {
  assert(idx < nGP() && out.size() >= nDim_);

  for (size_t dim = 0; dim < nDim_; dim++) {
    const GPCntType nGPInDim = this->nGPInDim(dim);
    out[dim] = nodesInDim(dim)[idx % nGPInDim];
    idx /= nGPInDim;
  }
}

base::DataVector TensorGridView::getGridPoint(const size_t idx) const {
  base::DataVector result(nDim_);
  getGridPoint(idx, result);
  return result;
}

TensorGrid TensorGridView::toTensorGrid() const {
  return TensorGrid(getGPCntPerDim(), getNodesPerDim());
}

bool TensorGridView::operator==(const TensorGridView& other) const {
  if (nDim_ != other.nDim_) {
    return false;
  }

  for (size_t dim = 0; dim < nDim_; dim++) {
    if (nGPInDim(dim) != other.nGPInDim(dim)) {
      return false;
    }
    for (GPCntType i = 0; i < nGPInDim(dim); i++) {
      if (!tools::nearlyEqual(nodesInDim(dim)[i], other.nodesInDim(dim)[i])) {
        return false;
      }
    }
  }

  return true;
}

}  // namespace combigrid
}  // namespace sgpp
