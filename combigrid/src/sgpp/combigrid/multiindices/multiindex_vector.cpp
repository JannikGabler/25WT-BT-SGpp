// #ifndef COMBIGRID_MULTIINDEX_VECTOR_TPP
// #define COMBIGRID_MULTIINDEX_VECTOR_TPP

#include <cassert>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/multiindices/paretoMaxima.hpp>
#include "sgpp/combigrid/miscellaneous/multiindex_lookup.hpp"

namespace sgpp {
namespace combigrid {

MIVec::MIVec(const size_t nDim, const size_t nMI) : nDim_(nDim), nMI_(nMI), data(nMI * nDim) {}

MIVec::MIVec(const std::vector<MI>& mi)
    : nDim_(mi.size() == 0 ? 0 : mi[0].nDim()), nMI_(mi.size()), data() {}

size_t MIVec::nDim() const { return this->nDim_; }

size_t MIVec::nMI() const { return this->nMI_; }

MIType MIVec::operator()(size_t miIdx, size_t dim) const {
  assert(miIdx < this->nMI_ && dim < this->nDim_);

  return this->data[miIdx * nDim_ + dim];
}

MIType& MIVec::operator()(size_t miIdx, size_t dim) {
  assert(miIdx < nMI_ && dim < nDim_);

  return this->data[miIdx * nDim_ + dim];
}

MI MIVec::operator[](const size_t miIdx) const {
  assert(miIdx < nMI_);

  MI mi(nDim_);

  for (size_t dim = 0; dim < nDim_; dim++) {
    mi[dim] = data[miIdx * nDim_ + dim];
  }

  return mi;
}

MIVec::MILookupView MIVec::getLookupView(const size_t miIdx) const {
  return {&data[miIdx * nDim_], nDim_};
}

void MIVec::setMI(const size_t idx, const MI mi) {
  assert(mi.nDim() == nDim_);

  for (size_t dim = 0; dim < nDim_; dim++) {
    data[idx * nDim_ + dim] = mi[dim];
  }
}

bool MIVec::isDownwardsClosed() const {
  // TODO
  throw sgpp::base::not_implemented_exception("This operation is not implemented yet!");
}

std::vector<size_t> MIVec::paretoMaximum(const bool isDownwardsClosed) const {
  return computeParetoMaxima(*this, isDownwardsClosed);
}

}  // namespace combigrid
}  // namespace sgpp

// #endif