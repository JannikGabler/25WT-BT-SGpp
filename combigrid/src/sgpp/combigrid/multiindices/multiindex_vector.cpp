#include <cassert>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

MIVec::MIVec(const size_t nDim, const size_t nMI) : nDim_(nDim), nMI_(nMI), data_(nMI * nDim) {}

MIVec::MIVec(const std::vector<MI>& mi)
    : nDim_(mi.size() == 0 ? 0 : mi[0].nDim()), nMI_(mi.size()), data_(nMI_ * nDim_) {
  for (size_t i = 0; i < mi.size(); i++) {
    setMI(i, mi[i]);
  }
}

size_t MIVec::nDim() const { return this->nDim_; }

size_t MIVec::nMI() const { return this->nMI_; }

const MIType* MIVec::data() const { return data_.data(); }

MIType MIVec::operator()(const size_t miIdx, const size_t dim) const {
  assert(miIdx < nMI_ && dim < nDim_);

  return data_[miIdx * nDim_ + dim];
}

MIType& MIVec::operator()(const size_t miIdx, const size_t dim) {
  assert(miIdx < nMI_ && dim < nDim_);

  return data_[miIdx * nDim_ + dim];
}

MI MIVec::operator[](const size_t miIdx) const {
  assert(miIdx < nMI_);

  MI mi(nDim_);

  for (size_t dim = 0; dim < nDim_; dim++) {
    mi[dim] = data_[miIdx * nDim_ + dim];
  }

  return mi;
}

void MIVec::setMI(const size_t idx, const MI& mi) {
  for (size_t dim = 0; dim < nDim_; dim++) {
    data_[idx * nDim_ + dim] = mi[dim];
  }
}

bool MIVec::isDownwardsClosed() const {
  // TODO
  throw base::not_implemented_exception("This operation is not implemented yet!");
}

MIVec MIVec::downwardsClosure() const {
  const std::vector<size_t> paretoMaxima = this->paretoMaxima();
}

std::vector<size_t> MIVec::paretoMaxima(const bool isDownwardsClosed) const {
  return computeParetoMaxima(*this, isDownwardsClosed);
}

}  // namespace combigrid
}  // namespace sgpp
