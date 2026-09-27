/**
 * @file multiindex_vector.hpp
 * @brief Defines the @c MIVec container template that stores a sequence of
 * fixed-dimension multi-indices in a contiguous, cache-friendly layout.
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <ranges>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector_iterator.hpp>
#include <sgpp/combigrid/multiindices/multiindex_lexicographic_less.hpp>
#include <sgpp/combigrid/multiindices/multiindex_reference.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <sgpp/combigrid/tools/downwards_closedness.hpp>
#include <sgpp/combigrid/tools/multiindex_vector/multiindex_vector_component_wise_max.hpp>
#include <sgpp/combigrid/tools/multiindex_view/multiindex_view_utilities.hpp>
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
 * <b>Element access.</b> The stored multi-indices are not @c MI objects.
 * @c operator[] and the iterators hand out non-owning proxies instead:
 * @c MIView<T> (read-only) from a const @c MIVec and @c MIRef<T> (mutable)
 * from a non-const one. Accessing and comparing elements therefore never
 * creates an @c MI. Assigning to an @c MIRef copies entries into the
 * container; @c auto e = vec[i] creates a proxy (an alias), not a copy. Use
 * @c MI<T> e = vec[i] for an independent copy.
 *
 * <b>Standard algorithms.</b> @c MIVec is a random-access range whose
 * @c value_type is @c MI<T>. The classic @c <algorithm> functions and (from
 * C++20 on) the @c std::ranges algorithms work on it, including mutating ones
 * such as @c sort, @c stable_partition, @c remove_if + @ref erase, @c unique
 * and @c rotate. Note that the comparison operators of multi-indices are the
 * component-wise partial order: ordering algorithms need a strict weak
 * ordering such as @ref MILexLess.
 *
 * <b>Invalidation.</b> Iterators and proxies follow the rules of
 * @c std::vector: operations that may reallocate (@ref push_back beyond the
 * capacity, @ref reserve, @ref resize, @ref shrink_to_fit) invalidate all of
 * them; @ref erase and @ref pop_back invalidate those at or after the point
 * of erasure.
 *
 * <b>Cached values.</b> @c MIVec lazily caches a few derived properties
 * (component-wise maximum, Pareto maxima, hash lookup). Every non-const
 * operation that gives write access or modifies the container drops the
 * cache, including non-const @ref operator[], @ref begin, @ref end and
 * @ref data (use a const @c MIVec or @ref cbegin for read-only access that
 * keeps the cache). Consequently, writing through a mutable proxy or iterator
 * that was obtained @em before a cached value was computed invalidates that
 * value without the container noticing; obtain write access again (or call
 * @ref clearCachedValues) after computing cached values. Computing cached
 * values is not thread-safe.
 *
 * @tparam T Integral element type of the stored multi-indices.
 */
template <typename T>
class MIVec {
 public:
#ifndef SWIG
  /// @name Container types
  /// @{
  using value_type = MI<T>;                       ///< Owning element type.
  using reference = MIRef<T>;                     ///< Mutable proxy to a stored multi-index.
  using const_reference = MIView<T>;              ///< Read-only view of a stored multi-index.
  using iterator = MIVecIterator<T, false>;       ///< Mutable random-access proxy iterator.
  using const_iterator = MIVecIterator<T, true>;  ///< Read-only random-access proxy iterator.
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  /// @}
#endif

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
  MIVec(const size_t nDim, const size_t nMI)
      : nDim_(nDim), nMI_(nMI), data_(nMI * nDim), cacheCleared(true) {}

  /**
   * @brief Constructs an @c MIVec by copying a sequence of multi-indices.
   *
   * The dimensionality is inferred from the first multi-index. All
   * multi-indices in @p mi must share that dimensionality.
   *
   * @param mi Source vector of multi-indices.
   */
  MIVec(const std::vector<MI<T>>& mi)
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
   * The dimensionality is inferred from the first vector. All vectors must
   * share that dimensionality.
   *
   * @param mi Source vector of raw multi-indices.
   */
  MIVec(const std::vector<std::vector<T>>& mi)
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
   * @param initList Initializer list of multi-indices (all of the same dimensionality).
   */
  MIVec(const std::initializer_list<MI<T>> initList)
      : nDim_(initList.size() == 0 ? 0 : initList.begin()->nDim()),
        nMI_(initList.size()),
        data_(nMI_ * nDim_),
        cacheCleared(true) {
    size_t idx = 0;
    for (const MI<T>& mi : initList) {
      setMI(idx++, mi);
    }
  }

#ifndef SWIG
  /**
   * @brief Copy constructor.
   *
   * Copies the multi-indices and the cached component-wise maximum and Pareto
   * maxima. The cached lookup refers to the storage of @p other and is
   * therefore not copied.
   */
  MIVec(const MIVec& other)
      : nDim_(other.nDim_),
        nMI_(other.nMI_),
        data_(other.data_),
        cacheCleared(true),
        componentWiseMax_(other.componentWiseMax_),
        paretoMaxima_(other.paretoMaxima_) {
    updateCacheCleared();
  }

  /**
   * @brief Move constructor.
   *
   * Takes over the multi-indices and the cached component-wise maximum and
   * Pareto maxima; @p other is left empty (same dimensionality, no
   * multi-indices, no cached values).
   */
  MIVec(MIVec&& other) noexcept
      : nDim_(other.nDim_),
        nMI_(other.nMI_),
        data_(std::move(other.data_)),
        cacheCleared(true),
        componentWiseMax_(std::move(other.componentWiseMax_)),
        paretoMaxima_(std::move(other.paretoMaxima_)) {
    updateCacheCleared();
    other.resetAfterMove();
  }

  /// @brief Copy assignment; same semantics as the copy constructor.
  MIVec& operator=(const MIVec& other) {
    if (this != &other) {
      MIVec copy(other);
      *this = std::move(copy);
    }
    return *this;
  }

  /// @brief Move assignment; same semantics as the move constructor.
  MIVec& operator=(MIVec&& other) noexcept {
    if (this != &other) {
      nDim_ = other.nDim_;
      nMI_ = other.nMI_;
      data_ = std::move(other.data_);
      lookup_.reset();
      componentWiseMax_ = std::move(other.componentWiseMax_);
      paretoMaxima_ = std::move(other.paretoMaxima_);
      updateCacheCleared();
      other.resetAfterMove();
    }
    return *this;
  }

  ~MIVec() = default;
#endif

  /*****
  Getter
  *****/
  /// @brief Returns the dimensionality of every stored multi-index.
  size_t nDim() const noexcept { return nDim_; }
  /// @brief Returns the number of stored multi-indices.
  size_t nMI() const noexcept { return nMI_; }

  /// @brief Read-only pointer to the underlying flat AoS storage.
  const T* data() const noexcept { return data_.data(); }

  /**
   * @brief Read-only access to a single component of a stored multi-index.
   * @param miIdx Index of the multi-index (0-based).
   * @param dim   Dimension within that multi-index.
   * @return Value of the @p dim -th component of the @p miIdx -th multi-index.
   */
  T operator()(size_t miIdx, size_t dim) const noexcept {
    assert(miIdx < nMI_ && dim < nDim_);
    return data_[miIdx * nDim_ + dim];
  }

  /**
   * @brief Mutable access to a single component. Drops the cached values.
   * @param miIdx Index of the multi-index.
   * @param dim   Dimension within that multi-index.
   * @return Reference to the addressed component.
   */
  T& operator()(size_t miIdx, size_t dim) noexcept {
    assert(miIdx < nMI_ && dim < nDim_);
    clearCachedValues();
    return data_[miIdx * nDim_ + dim];
  }

#ifndef SWIG
  /// @brief Returns the number of stored multi-indices (alias of @ref nMI()).
  size_t size() const noexcept { return nMI_; }
  /// @brief Returns @c true iff no multi-index is stored.
  bool empty() const noexcept { return nMI_ == 0; }

  /// @brief Mutable pointer to the underlying flat AoS storage. Drops the cached values.
  T* data() noexcept {
    clearCachedValues();
    return data_.data();
  }

  /**
   * @brief Read-only view of the @p miIdx -th stored multi-index.
   * @param miIdx Index of the multi-index.
   * @return Non-owning view; no @c MI is created.
   */
  MIView<T> operator[](const size_t miIdx) const noexcept {
    assert(miIdx < nMI_);
    return MIView<T>(data_.data() + miIdx * nDim_, nDim_);
  }

  /**
   * @brief Mutable proxy to the @p miIdx -th stored multi-index. Drops the
   * cached values.
   * @param miIdx Index of the multi-index.
   * @return Proxy; assigning to it overwrites the stored multi-index.
   */
  MIRef<T> operator[](const size_t miIdx) noexcept {
    assert(miIdx < nMI_);
    clearCachedValues();
    return MIRef<T>(data_.data() + miIdx * nDim_, nDim_);
  }

  /// @brief Read-only view of the first multi-index.
  MIView<T> front() const noexcept { return (*this)[0]; }
  /// @brief Mutable proxy to the first multi-index. Drops the cached values.
  MIRef<T> front() noexcept { return (*this)[0]; }
  /// @brief Read-only view of the last multi-index.
  MIView<T> back() const noexcept { return (*this)[nMI_ - 1]; }
  /// @brief Mutable proxy to the last multi-index. Drops the cached values.
  MIRef<T> back() noexcept { return (*this)[nMI_ - 1]; }

  /*******
  Iterator
  *******/
  /// @brief Mutable iterator to the first multi-index. Drops the cached values.
  iterator begin() noexcept {
    clearCachedValues();
    return iterator(data_.data(), nDim_, 0);
  }
  /// @brief Mutable iterator past the last multi-index. Drops the cached values.
  iterator end() noexcept {
    clearCachedValues();
    return iterator(data_.data(), nDim_, static_cast<difference_type>(nMI_));
  }
  /// @brief Const iterator to the first multi-index.
  const_iterator begin() const noexcept { return const_iterator(data_.data(), nDim_, 0); }
  /// @brief Const iterator past the last multi-index.
  const_iterator end() const noexcept {
    return const_iterator(data_.data(), nDim_, static_cast<difference_type>(nMI_));
  }
  /// @brief Const iterator to the first multi-index.
  const_iterator cbegin() const noexcept { return begin(); }
  /// @brief Const iterator past the last multi-index.
  const_iterator cend() const noexcept { return end(); }

  /// @brief Mutable reverse iterator to the last multi-index. Drops the cached values.
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  /// @brief Mutable reverse iterator before the first multi-index. Drops the cached values.
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  /// @brief Const reverse iterator to the last multi-index.
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  /// @brief Const reverse iterator before the first multi-index.
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
  /// @brief Const reverse iterator to the last multi-index.
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  /// @brief Const reverse iterator before the first multi-index.
  const_reverse_iterator crend() const noexcept { return rend(); }
#endif

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
  void setMI(const size_t idx, const MI<T>& mi) { setMIImpl(idx, mi.data(), mi.size()); }

  /**
   * @brief Overwrites the @p idx -th stored multi-index with a raw vector.
   *
   * Clears the lazily cached derived properties.
   *
   * @param idx Index of the multi-index slot to overwrite.
   * @param mi  New multi-index. Must have @c nDim_ entries.
   */
  void setMI(const size_t idx, const std::vector<T>& mi) { setMIImpl(idx, mi.data(), mi.size()); }

#ifndef SWIG
  /**
   * @brief Overwrites the @p idx -th stored multi-index with the entries of a
   * view (@c MIView or @c MIRef), e.g. an element of another @c MIVec.
   *
   * Clears the lazily cached derived properties.
   *
   * @param idx Index of the multi-index slot to overwrite.
   * @param mi  View of the new multi-index. Must have @c nDim_ entries.
   */
  template <typename View,
            typename std::enable_if<tools::mi_view::MITypeTraits<View>::IsView::value,
                                    int>::type = 0>
  void setMI(const size_t idx, const View& mi) {
    const MIView<T> view(mi);
    setMIImpl(idx, view.data(), view.size());
  }
#endif

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

#ifndef SWIG
  /**
   * @brief Appends a copy of @p mi (an @c MI, @c MIView or @c MIRef).
   *
   * @p mi may refer to an element of this container. Clears the lazily
   * cached derived properties.
   *
   * @param mi Multi-index to append. Must have @c nDim_ entries.
   */
  void push_back(const MIView<T> mi) {
    assert(mi.size() == nDim_);
    clearCachedValues();

    const size_t oldSize = data_.size();
    const T* const storage = data_.data();
    const std::less<const T*> before;

    if (nDim_ > 0 && !before(mi.data(), storage) && before(mi.data(), storage + oldSize)) {
      // mi refers to an element of this container, which growing data_ may reallocate.
      const size_t offset = static_cast<size_t>(mi.data() - storage);
      data_.resize(oldSize + nDim_);
      std::copy_n(data_.data() + offset, nDim_, data_.data() + oldSize);
    } else {
      data_.insert(data_.end(), mi.data(), mi.data() + nDim_);
    }
    nMI_++;
  }

  /// @brief Removes the last multi-index. Clears the lazily cached derived properties.
  void pop_back() {
    assert(nMI_ > 0);
    clearCachedValues();
    nMI_--;
    data_.resize(nMI_ * nDim_);
  }

  /**
   * @brief Removes the multi-indices in @c [first,last).
   *
   * Clears the lazily cached derived properties.
   *
   * @param first Iterator to the first multi-index to remove.
   * @param last  Iterator past the last multi-index to remove.
   * @return Iterator to the multi-index that followed the removed ones.
   */
  iterator erase(const const_iterator first, const const_iterator last) {
    assert(first.base() == data_.data() && last.base() == data_.data());
    assert(0 <= first.index() && first.index() <= last.index() &&
           static_cast<size_t>(last.index()) <= nMI_);
    clearCachedValues();

    const size_t firstIdx = static_cast<size_t>(first.index());
    const size_t lastIdx = static_cast<size_t>(last.index());
    data_.erase(data_.begin() + static_cast<difference_type>(firstIdx * nDim_),
                data_.begin() + static_cast<difference_type>(lastIdx * nDim_));
    nMI_ -= lastIdx - firstIdx;

    return iterator(data_.data(), nDim_, first.index());
  }

  /**
   * @brief Removes the multi-index at @p pos.
   *
   * Clears the lazily cached derived properties.
   *
   * @param pos Iterator to the multi-index to remove.
   * @return Iterator to the multi-index that followed the removed one.
   */
  iterator erase(const const_iterator pos) { return erase(pos, pos + 1); }

  /// @brief Removes all multi-indices; the dimensionality is kept.
  void clear() noexcept {
    clearCachedValues();
    nMI_ = 0;
    data_.clear();
  }

  /**
   * @brief Swaps the contents of this container and @p other.
   *
   * The cached lookups refer to the respective storage and are dropped.
   */
  void swap(MIVec& other) noexcept {
    std::swap(nDim_, other.nDim_);
    std::swap(nMI_, other.nMI_);
    data_.swap(other.data_);
    componentWiseMax_.swap(other.componentWiseMax_);
    paretoMaxima_.swap(other.paretoMaxima_);
    lookup_.reset();
    other.lookup_.reset();
    updateCacheCleared();
    other.updateCacheCleared();
  }

  /// @brief Swaps the contents of @p a and @p b (see @ref swap(MIVec&)).
  friend void swap(MIVec& a, MIVec& b) noexcept { a.swap(b); }
#endif

  /**************
  Size operations
  **************/
  /**
   * @brief Resizes the container to hold @p nMI multi-indices.
   *
   * Existing entries with index @c <nMI are preserved. Newly added slots
   * are default-initialized. Clears the lazily cached derived properties.
   *
   * @param nMI New number of multi-indices.
   */
  void resize(const size_t nMI) {
    clearCachedValues();
    nMI_ = nMI;
    data_.resize(nMI * nDim_);
  }

#ifndef SWIG
  /**
   * @brief Reserves storage for at least @p nMI multi-indices.
   * @param nMI Number of multi-indices to reserve storage for.
   */
  void reserve(const size_t nMI) {
    const T* const oldStorage = data_.data();
    data_.reserve(nMI * nDim_);
    if (data_.data() != oldStorage) {
      dropLookup();
    }
  }
#endif

  /// @brief Releases unused capacity from the underlying storage.
  void shrink_to_fit() {
    const T* const oldStorage = data_.data();
    data_.shrink_to_fit();
    if (data_.data() != oldStorage) {
      dropLookup();
    }
  }

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
      lookup_ = std::make_shared<misc::MIVecLookup<T>>(*this);
      cacheCleared = false;
    }
    return lookup_;
  }

  /**
   * @brief Drops all lazily cached derived properties.
   *
   * Idempotent. Called automatically by all modifying member functions.
   */
  void clearCachedValues() const noexcept {
    if (!cacheCleared) {
      componentWiseMax_.reset();
      paretoMaxima_.reset();
      lookup_.reset();
      cacheCleared = true;
    }
  }

 private:
  size_t nDim_;  ///< Dimensionality of every stored multi-index.
  size_t nMI_;   ///< Number of stored multi-indices.

  /**
   * @brief Flat AoS storage: entry at multi-index @p i and dimension @p d
   * lives at @c data_[i * nDim_ + d]. Invariant: @c data_.size() == nMI_ * nDim_.
   */
  std::vector<T> data_;  // AoS: [idx][dim]

  mutable bool cacheCleared;  ///< @c true iff all cached pointers are currently null.
  mutable std::shared_ptr<MI<T>> componentWiseMax_;  ///< Cache for @ref componentWiseMax().
  mutable std::shared_ptr<std::vector<size_t>> paretoMaxima_;  ///< Cache for @ref paretoMaxima().
  /// @brief Cache for @ref lookup(). Refers to this object and its storage; never shared.
  mutable std::shared_ptr<misc::MIVecLookup<T>> lookup_;

  /// @brief Recomputes @ref cacheCleared from the cached pointers.
  void updateCacheCleared() const noexcept {
    cacheCleared = componentWiseMax_ == nullptr && paretoMaxima_ == nullptr && lookup_ == nullptr;
  }

  /// @brief Drops only the cached lookup (e.g. after the storage moved in memory).
  void dropLookup() const noexcept {
    lookup_.reset();
    updateCacheCleared();
  }

  /// @brief Leaves a moved-from object empty and without cached values.
  void resetAfterMove() noexcept {
    nMI_ = 0;
    data_.clear();
    componentWiseMax_.reset();
    paretoMaxima_.reset();
    lookup_.reset();
    cacheCleared = true;
  }

  /// @brief Copies @p size entries from @p src into slot @p idx (see @ref setMI).
  void setMIImpl(const size_t idx, const T* const src, const size_t size) {
    assert(idx < nMI_ && size == nDim_);
    clearCachedValues();
    std::copy_n(src, std::min(size, nDim_), data_.data() + idx * nDim_);
  }

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

#ifndef SWIG
/*************************************************************
Compile-time checks of the iterator and range properties.
Regressions of the proxy machinery fail here instead of in
algorithm instantiations deep inside the standard library.
*************************************************************/
static_assert(std::is_same<std::iterator_traits<MIVec<unsigned int>::iterator>::iterator_category,
                           std::random_access_iterator_tag>::value,
              "MIVec::iterator must dispatch to random-access algorithm implementations");
static_assert(
    std::is_same<std::iterator_traits<MIVec<unsigned int>::const_iterator>::iterator_category,
                 std::random_access_iterator_tag>::value,
    "MIVec::const_iterator must dispatch to random-access algorithm implementations");
static_assert(std::is_same<std::iterator_traits<MIVec<unsigned int>::iterator>::value_type,
                           MI<unsigned int>>::value,
              "The value_type of MIVec::iterator must be the owning MI");
static_assert(std::is_same<std::iterator_traits<MIVec<unsigned int>::iterator>::reference,
                           MIRef<unsigned int>>::value,
              "The reference of MIVec::iterator must be the proxy MIRef");
static_assert(std::is_convertible<MIRef<unsigned int>, MI<unsigned int>>::value,
              "Classic algorithms create value_type temporaries from dereferenced iterators");
static_assert(std::is_convertible<MIRef<unsigned int>, MIView<unsigned int>>::value &&
                  std::is_convertible<const MI<unsigned int>&, MIView<unsigned int>>::value,
              "MIView must be constructible from every multi-index type");

#if defined(__cpp_lib_ranges)
static_assert(std::random_access_iterator<MIVec<unsigned int>::iterator>);
static_assert(std::random_access_iterator<MIVec<unsigned int>::const_iterator>);
static_assert(std::same_as<std::iter_value_t<MIVec<unsigned int>::iterator>, MI<unsigned int>>);
static_assert(
    std::same_as<std::iter_reference_t<MIVec<unsigned int>::iterator>, MIRef<unsigned int>>);
static_assert(std::same_as<std::iter_common_reference_t<MIVec<unsigned int>::iterator>,
                           MIView<unsigned int>>);
static_assert(std::indirectly_writable<MIVec<unsigned int>::iterator, MI<unsigned int>>);
static_assert(std::indirectly_writable<MIVec<unsigned int>::iterator, const MI<unsigned int>&>);
static_assert(std::indirectly_writable<MIVec<unsigned int>::iterator, MIRef<unsigned int>>);
static_assert(std::indirectly_writable<MIVec<unsigned int>::iterator, MIView<unsigned int>>);
static_assert(!std::indirectly_writable<MIVec<unsigned int>::const_iterator, MI<unsigned int>>);
static_assert(std::indirectly_swappable<MIVec<unsigned int>::iterator>);
static_assert(std::permutable<MIVec<unsigned int>::iterator>);
static_assert(std::sortable<MIVec<unsigned int>::iterator, MILexLess>);
static_assert(std::ranges::random_access_range<MIVec<unsigned int>>);
static_assert(std::ranges::random_access_range<const MIVec<unsigned int>>);
static_assert(std::ranges::sized_range<MIVec<unsigned int>>);
static_assert(std::ranges::common_range<MIVec<unsigned int>>);
static_assert(std::equality_comparable_with<MIRef<unsigned int>, MI<unsigned int>>);
static_assert(std::equality_comparable_with<MIView<unsigned int>, MIRef<unsigned int>>);
#endif
#endif

}  // namespace combigrid
}  // namespace sgpp
