#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>

namespace sgpp {
namespace combigrid {}  // namespace combigrid

}  // namespace sgpp

template <typename T>
size_t sgpp::combigrid::MI<T>::toLinearIndex() const {
  const size_t nDim = this->size();

  size_t idx = 0;
  for (size_t dim = 0; dim < nDim; dim++) {
    idx *= nDim;
    idx += static_cast<size_t>((*this)[dim]);
  }

  return idx;
}

template <typename T>
size_t sgpp::combigrid::MI<T>::productofElems() const {
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

template <typename T>
size_t sgpp::combigrid::MI<T>::nDim() const {
  return this->size();
}

template <typename T>
bool sgpp::combigrid::MI<T>::operator<(MI<T>& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] >= other[dim]) {
      return false;
    }
  }

  return true;
}

template <typename T>
bool sgpp::combigrid::MI<T>::operator>(MI<T>& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] <= other[dim]) {
      return false;
    }
  }

  return true;
}

template <typename T>
bool sgpp::combigrid::MI<T>::operator<=(MI<T>& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] > other[dim]) {
      return false;
    }
  }

  return true;
}

template <typename T>
bool sgpp::combigrid::MI<T>::operator>=(MI<T>& other) const {
  assert(this->size() == other.size());

  for (size_t dim = 0; dim < this->size(); dim++) {
    if ((*(this))[dim] < other[dim]) {
      return false;
    }
  }

  return true;
}