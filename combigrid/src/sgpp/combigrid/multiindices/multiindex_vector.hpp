#ifndef COMBIGRID_MULTIINDEX_VECTOR_HPP
#define COMBIGRID_MULTIINDEX_VECTOR_HPP

#include <initializer_list>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector_element_proxy.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
#include <sgpp/combigrid/tools/multiindex_vector/multiindex_vector_component_wise_max.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec {
 public:
  MIVec<T>(const size_t nDim, const size_t nMI)
      : nDim_(nDim), nMI_(nMI), data_(nMI * nDim), cacheCleared(true) {}

  MIVec<T>(const std::vector<MI<T>>& mi)
      : nDim_(mi.size() == 0 ? 0 : mi[0].nDim()),
        nMI_(mi.size()),
        data_(nMI_ * nDim_),
        cacheCleared(true) {
    for (size_t i = 0; i < mi.size(); i++) {
      setMI(i, mi[i]);
    }
  }

  MIVec<T>(const std::vector<std::vector<T>>& mi)
      : nDim_(mi.size() == 0 ? 0 : mi[0].size()),
        nMI_(mi.size()),
        data_(nMI_ * nDim_),
        cacheCleared(true) {
    for (size_t i = 0; i < mi.size(); i++) {
      setMI(i, mi[i]);
    }
  }

  MIVec<T>(const std::initializer_list<MI<T>> initList)
      : nDim_(initList.size() == 0 ? 0 : initList.begin()->nDim()),
        nMI_(initList.size()),
        data_(nMI_ * nDim_),
        cacheCleared(true) {
    size_t idx = 0;
    for (const MI<T>& mi : initList) {
      setMI(idx++, mi);
    }
  }

  size_t nDim() const { return nDim_; }
  size_t nMI() const { return nMI_; }

  const T* data() const { return data_.data(); }

  T operator()(size_t miIdx, size_t dim) const {
    assert(miIdx < nMI_ && dim < nDim_);
    return data_[miIdx * nDim_ + dim];
  }

  MIVecElemProxy<T> operator()(size_t miIdx, size_t dim) {
    assert(miIdx < nMI_ && dim < nDim_);
    return MIVecElemProxy<T>(*this, data_[miIdx * nDim_ + dim]);
  }

  MI<T> operator[](size_t miIdx) const {
    assert(miIdx < nMI_);
    MI<T> mi(nDim_);
    for (size_t dim = 0; dim < nDim_; dim++) {
      mi[dim] = data_[miIdx * nDim_ + dim];
    }
    return mi;
  }

  void setMI(size_t idx, const MI<T>& mi) {
    clearCachedValues();
    for (size_t dim = 0; dim < nDim_; dim++) {
      data_[idx * nDim_ + dim] = mi[dim];
    }
  }

  void setMI(size_t idx, const std::vector<T>& mi) {
    clearCachedValues();
    for (size_t dim = 0; dim < nDim_; dim++) {
      data_[idx * nDim_ + dim] = mi[dim];
    }
  }

  bool isDownwardsClosed() const { return tools::isMIVecDownwardsClosed(*this); }

  MIVec<T> downwardsClosure() const { return tools::genMIVecDownwardsClosure(*this); }

  const std::shared_ptr<MI<T>> componentWiseMax() const {
    if (componentWiseMax_ == nullptr) {
      const MI<T> result = tools::computeComponentWiseMax(*this);
      componentWiseMax_ = std::make_shared<MI<T>>(std::move(result));
      cacheCleared = false;
    }
    return componentWiseMax_;
  }

  const std::shared_ptr<std::vector<size_t>> paretoMaxima(bool isDownwardsClosed = false) const {
    if (paretoMaxima_ == nullptr) {
      const std::vector<size_t> result = tools::computeParetoMaxima(*this, isDownwardsClosed);
      paretoMaxima_ = std::make_shared<std::vector<size_t>>(std::move(result));
      cacheCleared = false;
    }
    return paretoMaxima_;
  }

  const std::shared_ptr<misc::MIVecLookup<T>> lookup() const {
    if (lookup_ == nullptr) {
      const misc::MIVecLookup<T> result(*this);
      lookup_ = std::make_shared<misc::MIVecLookup<T>>(result);
      cacheCleared = false;
    }
    return lookup_;
  }

  void clearCachedValues() const {
    if (!cacheCleared) {
      componentWiseMax_.reset();
      paretoMaxima_.reset();
      lookup_.reset();
      cacheCleared = true;
    }
  }

 private:
  const size_t nDim_;
  size_t nMI_;

  std::vector<T> data_;  // AoS: [idx][dim]

  mutable bool cacheCleared;
  mutable std::shared_ptr<MI<T>> componentWiseMax_;
  mutable std::shared_ptr<std::vector<size_t>> paretoMaxima_;
  mutable std::shared_ptr<misc::MIVecLookup<T>> lookup_;
};

}  // namespace combigrid
}  // namespace sgpp

#endif