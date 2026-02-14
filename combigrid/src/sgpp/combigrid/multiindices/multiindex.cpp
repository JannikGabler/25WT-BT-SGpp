#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>

namespace sgpp {
namespace combigrid {

size_t MI::toLinearIndex() const {
  const size_t nDim = this->size();

  size_t idx = 0;
  for (size_t dim = 0; dim < nDim; dim++) {
    idx *= nDim;
    idx += static_cast<size_t>((*this)[dim]);
  }

  return idx;
}

size_t MI::productofElems() const {
  const size_t nDim = this->size();

  if (nDim == 0) {
    return 0;
  } else {
    size_t product = 1;
    for (size_t dim = 0; dim < nDim; dim++) {
      product *= static_cast<size_t>((*this)[dim]);
    }

    return product;
  }
}

size_t MI::nDim() const { return this->size(); }

bool MI::operator==(const MI& other) const {
  if (this->size() != other.size()) {
    return false;
  }

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] != other[dim]) {
      return false;
    }
  }

  return true;
}

bool MI::operator<(const MI& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] >= other[dim]) {
      return false;
    }
  }

  return true;
}

bool MI::operator>(const MI& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] <= other[dim]) {
      return false;
    }
  }

  return true;
}

bool MI::operator<=(const MI& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] > other[dim]) {
      return false;
    }
  }

  return true;
}

bool MI::operator>=(const MI& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] < other[dim]) {
      return false;
    }
  }

  return true;
}

}  // namespace combigrid

}  // namespace sgpp
