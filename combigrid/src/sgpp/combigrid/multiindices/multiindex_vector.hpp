/**
 * @file multiindex_vector.hpp
 * @brief Defines the @c MIVec container template that stores a sequence of
 * fixed-dimension multi-indices in a contiguous, cache-friendly layout.
 */
#ifndef COMBIGRID_MULTIINDEX_VECTOR_HPP
#define COMBIGRID_MULTIINDEX_VECTOR_HPP

#include <algorithm>
#include <cassert>
#include <cstring>
#include <initializer_list>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector_element_proxy.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
#include <sgpp/combigrid/tools/multiindex_vector/multiindex_vector_component_wise_max.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <type_traits>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Contiguous storage for many multi-indices that all share the same
 * dimensionality.
 *
 * Internally the entries are stored in a flat @c std::vector using an
 * Array-of-Structures (AoS) layout: the @f$d@f$-th component of the
 * @f$i@f$-th multi-index lives at flat index @f$i \cdot \mathrm{nDim} + d@f$.
 * This layout is well suited to streaming access patterns over whole
 * multi-indices.
 *
 * @c MIVec lazily caches a few derived properties (component-wise maximum,
 * Pareto maxima, hash lookup). Any modifying operation invalidates the
 * cache.
 *
 * @tparam T Integral element type of the stored multi-indices.
 */
template <typename T>
class MIVec {
 public:
  /**********
  Constructor
  **********/
  /**
   * @brief Constructs an @c MIVec holding @p nMI default-initialized
   * multi-indices of dimension @p nDim.
   *
   * @param nDim Number of dimensions of every stored multi-index.
   * @param nMI  Number of multi-indices to store.
   */
  MIVec<T>(const size_t nDim, const size_t nMI)
      : nDim_(nDim), nMI_(nMI), data_(nMI * nDim), cacheCleared(true) {}

  /**
   * @brief Constructs an @c MIVec by copying a sequence of multi-indices.
   *
   * The dimensionality is inferred from the first multi-index. All
   * multi-indices in @p mi are expected to share that dimensionality.
   *
   * @param mi Source vector of multi-indices.
   */
  MIVec<T>(const std::vector<MI<T>>& mi)
      : nDim_(mi.size() == 0 ? 0 : mi[0].nDim()),
        nMI_(mi.size()),
        data_(nMI_ * nDim_),
        cacheCleared(true) {
    for (size_t i = 0; i < mi.size(); i++) {
      setMI(i, mi[i]);
    }
  }

  /**
   * @brief Constructs an @c MIVec by copying a sequence of raw vectors.
   *
   * The dimensionality is inferred from the first vector. All vectors are
   * expected to share that dimensionality.
   *
   * @param mi Source vector of raw multi-indices.
   */
  MIVec<T>(const std::vector<std::vector<T>>& mi)
      : nDim_(mi.size() == 0 ? 0 : mi[0].size()),
        nMI_(mi.size()),
        data_(nMI_ * nDim_),
        cacheCleared(true) {
    for (size_t i = 0; i < mi.size(); i++) {
      setMI(i, mi[i]);
    }
  }

  /**
   * @brief Constructs an @c MIVec from a brace-enclosed list of multi-indices.
   * @param initList Initializer list of multi-indices.
   */
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

  /*****
  Getter
  *****/
  /// @brief Returns the dimensionality of every stored multi-index.
  size_t nDim() const { return nDim_; }
  /// @brief Returns the number of stored multi-indices.
  size_t nMI() const { return nMI_; }

  /// @brief Read-only pointer to the underlying flat AoS storage.
  const T* data() const { return data_.data(); }

  /**
   * @brief Read-only access to a single component of a stored multi-index.
   * @param miIdx Index of the multi-index (0-based).
   * @param dim   Dimension within that multi-index.
   * @return Value of the @p dim -th component of the @p miIdx -th multi-index.
   */
  T operator()(size_t miIdx, size_t dim) const {
    assert(miIdx < nMI_ && dim < nDim_);
    return data_[miIdx * nDim_ + dim];
  }

  /**
   * @brief Mutable access to a single component, returning a proxy that
   * invalidates cached derived properties on assignment.
   * @param miIdx Index of the multi-index.
   * @param dim   Dimension within that multi-index.
   * @return Proxy referring to the addressed component.
   */
  MIVecElemProxy<T> operator()(size_t miIdx, size_t dim) {
    assert(miIdx < nMI_ && dim < nDim_);
    return MIVecElemProxy<T>(*this, data_[miIdx * nDim_ + dim]);
  }

  /**
   * @brief Returns a copy of the @p miIdx -th stored multi-index.
   * @param miIdx Index of the multi-index.
   * @return Newly constructed @c MI<T>.
   */
  MI<T> operator[](size_t miIdx) const {
    assert(miIdx < nMI_);
    MI<T> mi(nDim_);
    for (size_t dim = 0; dim < nDim_; dim++) {
      mi[dim] = data_[miIdx * nDim_ + dim];
    }
    return mi;
  }

  /*****
  Setter
  *****/
  /**
   * @brief Overwrites the @p idx -th stored multi-index with @p mi.
   *
   * Clears the lazily cached derived properties.
   *
   * @param idx Index of the multi-index slot to overwrite.
   * @param mi  New multi-index. Must have @c nDim_ entries.
   */
  void setMI(const size_t idx, const MI<T>& mi) {
    clearCachedValues();

    // for (size_t dim = 0; dim < nDim_; dim++) {
    //   data_[idx * nDim_ + dim] = mi[dim];
    // }
    std::copy_n(mi.begin(), mi.size(), data_.begin() + idx * nDim_);
  }

  /**
   * @brief Overwrites the @p idx -th stored multi-index with a raw vector.
   *
   * Clears the lazily cached derived properties.
   *
   * @param idx Index of the multi-index slot to overwrite.
   * @param mi  New multi-index. Must have @c nDim_ entries.
   */
  void setMI(size_t idx, const std::vector<T>& mi) {
    clearCachedValues();
    // for (size_t dim = 0; dim < nDim_; dim++) {
    //   data_[idx * nDim_ + dim] = mi[dim];
    // }
    std::copy_n(mi.begin(), mi.size(), data_.begin() + idx * nDim_);
  }

  /**
   * @brief Moves the multi-index from slot @p src to slot @p dest.
   *
   * After the call the contents of slot @p src are unspecified. Clears the
   * lazily cached derived properties.
   *
   * @param dest Destination slot index.
   * @param src  Source slot index.
   */
  void moveMI(const size_t dest, const size_t src) {
    assert(dest < nMI_ && src < nMI_);

    if (src == dest) {
      return;
    }
    clearCachedValues();

    T* basePtr = data_.data();
    T* destPtr = basePtr + dest * nDim_;
    T* srcPtr = basePtr + src * nDim_;

    moveMIImpl(destPtr, srcPtr, std::is_trivially_copyable<T>());
  }

  /**************
  Size operations
  **************/
  /**
   * @brief Resizes the container to hold @p nMI multi-indices.
   *
   * Existing entries with index @c <nMI are preserved. Newly added slots
   * are default-initialized.
   *
   * @param nMI New number of multi-indices.
   */
  void resize(const size_t nMI) {
    nMI_ = nMI;
    data_.resize(nMI * nDim_);
  }

  /// @brief Releases unused capacity from the underlying storage.
  void shrink_to_fit() { data_.shrink_to_fit(); }

  /*****************
  Utility operations
  *****************/
  /**
   * @brief Tests whether the stored set of multi-indices is downwards closed.
   *
   * A set @f$I@f$ is downwards closed if for every @f$\vec{\ell} \in I@f$
   * all multi-indices @f$\vec{\ell}'\le\vec{\ell}@f$ (component-wise) are
   * also in @f$I@f$. This property is required by the standard combination
   * technique.
   *
   * @return @c true if downwards closed, @c false otherwise.
   */
  bool isDownwardsClosed() const { return tools::isMIVecDownwardsClosed(*this); }

  /**
   * @brief Returns the downwards closure of the stored set, i.e. the
   * smallest downwards-closed superset.
   * @return New @c MIVec containing the downwards closure.
   */
  MIVec<T> downwardsClosure() const { return tools::genMIVecDownwardsClosure(*this); }

  /**
   * @brief Returns the lazily cached component-wise maximum across all
   * stored multi-indices.
   *
   * @return Shared pointer to the @c MI holding the per-dimension maxima.
   */
  const std::shared_ptr<MI<T>> componentWiseMax() const {
    if (componentWiseMax_ == nullptr) {
      const MI<T> result = tools::computeComponentWiseMax<T>(*this);
      componentWiseMax_ = std::make_shared<MI<T>>(std::move(result));
      cacheCleared = false;
    }
    return componentWiseMax_;
  }

  /**
   * @brief Returns the indices of the Pareto-maximal stored multi-indices.
   *
   * A multi-index is Pareto-maximal if no other stored multi-index
   * dominates it component-wise. The result is cached.
   *
   * @param isDownwardsClosed Hint that the stored set is already downwards
   * closed, which enables a faster algorithm.
   * @return Shared pointer to a vector with the indices of Pareto maxima.
   */
  const std::shared_ptr<std::vector<size_t>> paretoMaxima(
      const bool isDownwardsClosed = false) const {
    if (paretoMaxima_ == nullptr) {
      const std::vector<size_t> result = tools::computeParetoMaxima<T>(*this, isDownwardsClosed);
      paretoMaxima_ = std::make_shared<std::vector<size_t>>(std::move(result));
      cacheCleared = false;
    }
    return paretoMaxima_;
  }

  /**
   * @brief Returns a lazily built hash-based reverse lookup mapping each
   * stored multi-index to its slot index.
   * @return Shared pointer to the lookup structure.
   */
  const std::shared_ptr<misc::MIVecLookup<T>> lookup() const {
    if (lookup_ == nullptr) {
      const misc::MIVecLookup<T> result(*this);
      lookup_ = std::make_shared<misc::MIVecLookup<T>>(result);
      cacheCleared = false;
    }
    return lookup_;
  }

  /**
   * @brief Drops all lazily cached derived properties.
   *
   * Idempotent. Called automatically by all modifying member functions.
   */
  void clearCachedValues() const {
    if (!cacheCleared) {
      componentWiseMax_.reset();
      paretoMaxima_.reset();
      lookup_.reset();
      cacheCleared = true;
    }
  }

 private:
  const size_t nDim_;  ///< Dimensionality of every stored multi-index (fixed for the lifetime).
  size_t nMI_;         ///< Number of stored multi-indices.

  /**
   * @brief Flat AoS storage: entry at multi-index @p i and dimension @p d
   * lives at @c data_[i * nDim_ + d].
   */
  std::vector<T> data_;  // AoS: [idx][dim]

  mutable bool cacheCleared;  ///< @c true iff all cached pointers are currently null.
  mutable std::shared_ptr<MI<T>> componentWiseMax_;            ///< Cache for @ref componentWiseMax().
  mutable std::shared_ptr<std::vector<size_t>> paretoMaxima_;  ///< Cache for @ref paretoMaxima().
  mutable std::shared_ptr<misc::MIVecLookup<T>> lookup_;       ///< Cache for @ref lookup().

  /// @brief Trivially-copyable fast path for @ref moveMI() using @c std::memmove.
  void moveMIImpl(T* dest, T* src, std::true_type) { std::memmove(dest, src, nDim_ * sizeof(T)); }

  /// @brief Generic fallback for @ref moveMI() that uses @c std::move /
  /// @c std::move_backward depending on overlap direction.
  void moveMIImpl(T* dest, T* src, std::false_type) {
    if (dest < src) {
      std::move(src, src + nDim_, dest);
    } else {
      std::move_backward(src, src + nDim_, dest + nDim_);
    }
  }
};

}  // namespace combigrid
}  // namespace sgpp

#endif
