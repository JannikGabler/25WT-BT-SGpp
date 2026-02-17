#include <omp.h>
#include <cassert>
#include <memory>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
#include <sgpp/combigrid/tools/multiindex_vector/multiindex_vector_component_wise_max.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace {}  // namespace

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
  clearCachedValues();

  for (size_t dim = 0; dim < nDim_; dim++) {
    data_[idx * nDim_ + dim] = mi[dim];
  }
}

bool MIVec::isDownwardsClosed() const { return tools::isMIVecDownwardsClosed(*this); }

MIVec MIVec::downwardsClosure() const {
  // TODO
  throw base::not_implemented_exception("This operation is not implemented yet!");
}

const std::shared_ptr<MI> MIVec::componentWiseMax() const {
  if (componentWiseMax_ == nullptr) {
    const MI result = tools::computeComponentWiseMax(*this);

    componentWiseMax_ = std::make_shared<MI>(std::move(result));
  }

  return componentWiseMax_;
}

const std::shared_ptr<std::vector<size_t>> MIVec::paretoMaxima(const bool isDownwardsClosed) const {
  const std::vector<int> a{5, 6};
  const std::vector<int> b{5};

  if (paretoMaxima_ == nullptr) {
    const std::vector<size_t> result = tools::computeParetoMaxima(*this, isDownwardsClosed);
    paretoMaxima_ = std::make_shared<std::vector<size_t>>(std::move(result));
  }

  return paretoMaxima_;
}

const std::shared_ptr<misc::MIVecLookup> MIVec::lookup() const {
  if (lookup_ == nullptr) {
    const misc::MIVecLookup result(*this);
    lookup_ = std::make_shared<misc::MIVecLookup>(result);
  }

  return lookup_;
}

void MIVec::clearCachedValues() const {
  componentWiseMax_.reset();
  paretoMaxima_.reset();
}

}  // namespace combigrid
}  // namespace sgpp
