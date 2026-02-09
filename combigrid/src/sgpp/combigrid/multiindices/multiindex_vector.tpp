// #ifndef COMBIGRID_MULTIINDEX_VECTOR_TPP
// #define COMBIGRID_MULTIINDEX_VECTOR_TPP

#include <cassert>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>

namespace sgpp {
namespace combigrid {

template <typename T>
MIVec<T>::MIVec(const size_t nDim, const size_t nMI) : nDim_(nDim), nMI_(nMI), data(nMI * nDim) {}

template <typename T>
MIVec<T>::MIVec(const std::vector<MI<T>>& mi)
    : nDim_(mi.size() == 0 ? 0 : mi[0].nDim), nMI_(mi.size()), data() {}

template <typename T>
inline size_t MIVec<T>::nDim() const {
  return this->nDim_;
}

template <typename T>
inline size_t MIVec<T>::nMI() const {
  return this->nMI_;
}

template <typename T>
inline T MIVec<T>::operator()(size_t miIdx, size_t dim) const {
  assert(miIdx < this->nMI_ && dim < this->nDim_);

  return this->data[miIdx * nDim_ + dim];
}

template <typename T>
inline T& MIVec<T>::operator()(size_t miIdx, size_t dim) {
  assert(miIdx < nMI_ && dim < nDim_);

  return this->data[miIdx * nDim_ + dim];
}

template <typename T>
inline MI<T> MIVec<T>::operator[](const size_t miIdx) const {
  assert(miIdx < nMI_);

  MI<T> mi(nDim_);

  for (size_t dim = 0; dim < nDim_; dim++) {
    mi[dim] = data[miIdx * nDim_ + dim];
  }

  return mi;
}

template <typename T>
inline void MIVec<T>::setMI(const size_t idx, const MI<T> mi) {
  assert(mi.nDim() == nDim_);

  for (size_t dim = 0; dim < nDim_; dim++) {
    data[idx * nDim_ + dim] = mi[dim];
  }
}

template <typename T>
bool MIVec<T>::isDownwardsClosed() const {
  throw sgpp::base::not_implemented_exception("This operation is not implemented yet!");
}

template <typename T>
std::vector<size_t> MIVec<T>::paretoMaximum(const bool assumeDownwardsClosed) const {
  return computeParetoMaxima(*this);
}

}  // namespace combigrid
}  // namespace sgpp

// #endif