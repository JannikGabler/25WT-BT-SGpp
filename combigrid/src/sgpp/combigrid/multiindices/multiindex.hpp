/**
 * @file multiindex.hpp
 * @brief Defines the @c MI multi-index container template.
 *
 * A multi-index is the fundamental object of the combination technique:
 * it is a fixed-length tuple @f$(i_1, \dots, i_d)@f$ of integral values,
 * one per dimension. Depending on context, the entries denote refinement
 * levels (@c LvlType) or per-dimension grid-point indices (@c GPCntType).
 */
#ifndef COMBIGRID_MULTIINDEX_HPP
#define COMBIGRID_MULTIINDEX_HPP

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Lightweight multi-index container.
 *
 * @c MI wraps a @c std::vector and exposes the standard container interface
 * together with multi-index-specific helpers (@c toLinearIndex,
 * @c productofElems, @c sumOfElems) and lexicographic / element-wise
 * comparison operators. Implicit conversion to and from @c std::vector
 * makes @c MI interoperable with code that expects raw vectors.
 *
 * @tparam T Integral element type (e.g. @c LvlType, @c GPCntType).
 */
template <typename T>
class MI {
 public:
  using value_type = T;
  using allocator_type = std::allocator<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;
  using reverse_iterator = typename std::vector<T>::reverse_iterator;
  using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

  /* =========================
     Konstruktoren
     ========================= */
  /// @brief Constructs an empty multi-index.
  MI() = default;

  /**
   * @brief Constructs a multi-index of the given dimension, default-initialized.
   * @param count Number of dimensions.
   */
  explicit MI(size_type count) : data_(count) {}

  /**
   * @brief Constructs a multi-index of the given dimension, all entries set to @p value.
   * @param count Number of dimensions.
   * @param value Initial value for every entry.
   */
  MI(size_type count, value_type value) : data_(count, value) {}

  /**
   * @brief Constructs a multi-index from a brace-enclosed list of values.
   * @param init Initializer list providing the entries.
   */
  MI(std::initializer_list<value_type> init) : data_(init) {}

  /**
   * @brief Constructs a multi-index by copying an existing @c std::vector.
   * @param data Vector whose contents are copied into the new multi-index.
   */
  MI(std::vector<value_type>& data) : data_(data) {}

  /**
   * @brief Constructs a multi-index from an iterator range.
   * @tparam InputIt Forward-iterator type.
   * @param first Iterator to the first entry.
   * @param last  Iterator past the last entry.
   */
  template <class InputIt>
  MI(InputIt first, InputIt last) : data_(first, last) {}

  /* =========================
   Implicit convertion
   ========================= */
  /// @brief Implicit conversion to a mutable underlying @c std::vector.
  operator std::vector<value_type>&() noexcept { return data_; }

  /// @brief Implicit conversion to a read-only underlying @c std::vector.
  operator const std::vector<value_type>&() const noexcept { return data_; }

  /* =========================
   Additional methods
   ========================= */
  // size_t toLinearIndex() const {
  //   const size_t nDim = this->size();
  //   size_t idx = 0;
  //   for (size_t dim = 0; dim < nDim; dim++) {
  //     idx *= nDim;
  //     idx += static_cast<size_t>((*this)[dim]);
  //   }
  //   return idx;
  // }

  /**
   * @brief Maps the multi-index to a linear index using row-major (last-dim-fastest)
   * layout.
   *
   * The linear index is computed as
   * @f$\mathrm{idx} = i_0 + n_0 \cdot (i_1 + n_1 \cdot (i_2 + \cdots))@f$
   * where @c nPerDim provides the per-dimension extents @f$n_k@f$.
   *
   * @param nPerDim Per-dimension extents (size of the index space along each axis).
   * @return Linear index into a flattened tensor of shape @c nPerDim.
   */
  size_t toLinearIndex(const MI<T>& nPerDim) const {
    if (nDim() == 0) {
      return 0;
    }

    size_t idx = static_cast<size_t>(data_[nDim() - 1]);

    for (size_t dim = nDim() - 2; dim != std::numeric_limits<size_t>::max(); dim--) {
      idx *= nPerDim[dim + 1];
      idx += static_cast<size_t>(data_[dim]);
    }
    return idx;
  }

  /**
   * @brief Computes the product of all entries in @c value_type precision.
   * @return Product of entries, or @c 0 for an empty multi-index.
   */
  value_type productofElems() const {
    const size_t nDim = this->size();
    if (nDim == 0) {
      return 0;
    } else {
      value_type product = 1;
      for (size_t dim = 0; dim < nDim; dim++) {
        product *= (*this)[dim];
      }
      return product;
    }
  }

  /**
   * @brief Computes the product of all entries, accumulating in a wider type @c U.
   * @tparam U Accumulator type to avoid overflow for large products.
   * @return Product of entries, or @c 0 for an empty multi-index.
   */
  template <typename U>
  U productofElems() const {
    const size_t nDim = this->size();

    if (nDim == 0) {
      return 0;
    } else {
      U product = 1;
      for (size_t dim = 0; dim < nDim; dim++) {
        product *= static_cast<U>((*this)[dim]);
      }
      return product;
    }
  }

  /**
   * @brief Computes the sum of all entries in @c value_type precision.
   * @return Sum of entries, or @c 0 for an empty multi-index.
   */
  value_type sumOfElems() const {
    const size_t nDim = this->size();

    value_type sum = 0;
    for (size_t dim = 0; dim < nDim; dim++) {
      sum += (*this)[dim];
    }
    return sum;
  }

  /**
   * @brief Computes the sum of all entries, accumulating in a wider type @c U.
   * @tparam U Accumulator type to avoid overflow for large sums.
   * @return Sum of entries, or @c 0 for an empty multi-index.
   */
  template <typename U>
  U sumOfElems() const {
    const size_t nDim = this->size();

    U sum = 0;
    for (size_t dim = 0; dim < nDim; dim++) {
      sum += static_cast<U>((*this)[dim]);
    }
    return sum;
  }

  /// @brief Returns the number of dimensions (alias of @ref size()).
  size_t nDim() const { return this->size(); }

  /* =========================
     Elementzugriff
     ========================= */
  /// @brief Unchecked mutable access to the entry at position @p pos.
  reference operator[](size_type pos) { return data_[pos]; }
  /// @brief Unchecked read-only access to the entry at position @p pos.
  const_reference operator[](size_type pos) const { return data_[pos]; }

  /// @brief Bounds-checked mutable access to the entry at position @p pos.
  reference at(size_type pos) { return data_.at(pos); }
  /// @brief Bounds-checked read-only access to the entry at position @p pos.
  const_reference at(size_type pos) const { return data_.at(pos); }

  /// @brief Returns a mutable reference to the first entry.
  reference front() { return data_.front(); }
  /// @brief Returns a read-only reference to the first entry.
  const_reference front() const { return data_.front(); }

  /// @brief Returns a mutable reference to the last entry.
  reference back() { return data_.back(); }
  /// @brief Returns a read-only reference to the last entry.
  const_reference back() const { return data_.back(); }

  /// @brief Pointer to the underlying contiguous storage (mutable).
  pointer data() noexcept { return data_.data(); }
  /// @brief Pointer to the underlying contiguous storage (const).
  const_pointer data() const noexcept { return data_.data(); }

  /* =========================
     Iteratoren
     ========================= */
  /// @brief Iterator to the first entry.
  iterator begin() noexcept { return data_.begin(); }
  /// @brief Const iterator to the first entry.
  const_iterator begin() const noexcept { return data_.begin(); }
  /// @brief Const iterator to the first entry.
  const_iterator cbegin() const noexcept { return data_.cbegin(); }

  /// @brief Iterator past the last entry.
  iterator end() noexcept { return data_.end(); }
  /// @brief Const iterator past the last entry.
  const_iterator end() const noexcept { return data_.end(); }
  /// @brief Const iterator past the last entry.
  const_iterator cend() const noexcept { return data_.cend(); }

  /// @brief Reverse iterator to the last entry.
  reverse_iterator rbegin() noexcept { return data_.rbegin(); }
  /// @brief Const reverse iterator to the last entry.
  const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }

  /// @brief Reverse iterator past the first entry.
  reverse_iterator rend() noexcept { return data_.rend(); }
  /// @brief Const reverse iterator past the first entry.
  const_reverse_iterator rend() const noexcept { return data_.rend(); }

  /* =========================
     Kapazität
     ========================= */
  /// @brief Returns @c true if the multi-index has zero dimensions.
  bool empty() const noexcept { return data_.empty(); }
  /// @brief Returns the number of stored entries (the dimension).
  size_type size() const noexcept { return data_.size(); }
  /// @brief Returns the current capacity of the underlying storage.
  size_type capacity() const noexcept { return data_.capacity(); }

  /// @brief Reserves storage for at least @p new_cap entries.
  void reserve(size_type new_cap) { data_.reserve(new_cap); }
  /// @brief Releases unused capacity from the underlying storage.
  void shrink_to_fit() { data_.shrink_to_fit(); }

  /* =========================
     Modifikatoren
     ========================= */
  /// @brief Removes all entries (resulting in a 0-dimensional multi-index).
  void clear() noexcept { data_.clear(); }

  /// @brief Appends @p value as a new entry, increasing the dimension by one.
  void push_back(value_type value) { data_.push_back(value); }

  /// @brief Removes the last entry, decreasing the dimension by one.
  void pop_back() { data_.pop_back(); }

  /// @brief Inserts @p value before @p pos and returns an iterator to the inserted entry.
  iterator insert(const_iterator pos, value_type value) { return data_.insert(pos, value); }

  /// @brief Erases the entry at @p pos and returns an iterator to the next entry.
  iterator erase(const_iterator pos) { return data_.erase(pos); }

  /// @brief Erases the half-open range @c [first,last) and returns an iterator to the next entry.
  iterator erase(const_iterator first, const_iterator last) { return data_.erase(first, last); }

  /// @brief Resizes the multi-index to @p count dimensions, default-initializing new entries.
  void resize(size_type count) { data_.resize(count); }

  /// @brief Resizes the multi-index to @p count dimensions, filling new entries with @p value.
  void resize(size_type count, value_type value) { data_.resize(count, value); }

  /// @brief Swaps contents with @p other in O(1).
  void swap(MI& other) noexcept { data_.swap(other.data_); }

  /* =========================
     Vergleichsoperatoren
     ========================= */
  /// @name Comparison operators
  /// @brief Element-wise comparisons.
  ///
  /// Equality and inequality are defined element-wise. The relational
  /// operators (@c <, @c <=, @c >, @c >=) require the relation to hold in
  /// @em every dimension simultaneously (component-wise / partial order),
  /// not lexicographically. All comparisons throw @c std::logic_error if
  /// the operands have different sizes.
  /// @{
  friend bool operator==(const MI& a, const MI& b) { return cmpVecForEqual(a.data_, b.data_); }
  friend bool operator==(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForEqual(a.data_, b);
  }
  friend bool operator==(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForEqual(a, b.data_);
  }

  friend bool operator!=(const MI& a, const MI& b) { return cmpVecForUnequal(a.data_, b.data_); }
  friend bool operator!=(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForUnequal(a.data_, b);
  }
  friend bool operator!=(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForUnequal(a, b.data_);
  }

  friend bool operator<(const MI& a, const MI& b) { return cmpVecForLess(a.data_, b.data_); }
  friend bool operator<(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForLess(a.data_, b);
  }
  friend bool operator<(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForLess(a, b.data_);
  }

  friend bool operator<=(const MI& a, const MI& b) { return cmpVecForLessEqual(a.data_, b.data_); }
  friend bool operator<=(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForLessEqual(a.data_, b);
  }
  friend bool operator<=(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForLessEqual(a, b.data_);
  }

  friend bool operator>(const MI& a, const MI& b) { return cmpVecGreater(a.data_, b.data_); }
  friend bool operator>(const MI& a, const std::vector<value_type>& b) {
    return cmpVecGreater(a.data_, b);
  }
  friend bool operator>(const std::vector<value_type>& a, const MI& b) {
    return cmpVecGreater(a, b.data_);
  }

  friend bool operator>=(const MI& a, const MI& b) { return cmpVecGreaterEqual(a.data_, b.data_); }
  friend bool operator>=(const MI& a, const std::vector<value_type>& b) {
    return cmpVecGreaterEqual(a.data_, b);
  }
  friend bool operator>=(const std::vector<value_type>& a, const MI& b) {
    return cmpVecGreaterEqual(a, b.data_);
  }
  /// @}

  /* =========================
     Arithmetische Operatoren
     ========================= */
  /// @name Arithmetic operators
  /// @brief Element-wise multi-index arithmetic.
  ///
  /// All operations are component-wise. They throw @c std::logic_error if
  /// the operands have different sizes.
  /// @{

  /// @brief Component-wise addition (in-place).
  MI& operator+=(const MI& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] += other.data_[i];
    }
    return *this;
  }

  /// @brief Component-wise addition with a @c std::vector (in-place).
  MI& operator+=(const std::vector<value_type>& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] += other[i];
    }
    return *this;
  }

  /// @brief Component-wise subtraction (in-place).
  MI& operator-=(const MI& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] -= other.data_[i];
    }
    return *this;
  }

  /// @brief Component-wise subtraction with a @c std::vector (in-place).
  MI& operator-=(const std::vector<value_type>& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] -= other[i];
    }
    return *this;
  }

  /// @brief Component-wise addition.
  friend MI operator+(MI lhs, const MI& rhs) {
    lhs += rhs;
    return lhs;
  }

  /// @brief Component-wise addition with a @c std::vector.
  friend MI operator+(MI lhs, const std::vector<value_type>& rhs) {
    lhs += rhs;
    return lhs;
  }

  /// @brief Component-wise addition with a @c std::vector.
  friend MI operator+(const std::vector<value_type>& lhs, MI rhs) {
    rhs += lhs;
    return rhs;
  }

  /// @brief Component-wise subtraction.
  friend MI operator-(MI lhs, const MI& rhs) {
    lhs -= rhs;
    return lhs;
  }

  /// @brief Component-wise subtraction with a @c std::vector right-hand side.
  friend MI operator-(MI lhs, const std::vector<value_type>& rhs) {
    lhs -= rhs;
    return lhs;
  }

  /// @brief Component-wise subtraction with a @c std::vector left-hand side.
  friend MI operator-(std::vector<value_type> lhs, const MI& rhs) {
    check_same_size(lhs, rhs);
    MI result(lhs);
    result -= rhs;
    return result;
  }
  /// @}

 private:
  std::vector<value_type> data_;  ///< Backing storage of the multi-index entries.

  /**
   * @brief Throws @c std::logic_error if @p a and @p b have different sizes.
   * @throws std::logic_error if sizes mismatch.
   */
  static void check_same_size(const MI& a, const MI& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }
  /// @copydoc check_same_size(const MI&, const MI&)
  static void check_same_size(const MI& a, const std::vector<value_type>& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }
  /// @copydoc check_same_size(const MI&, const MI&)
  static void check_same_size(const std::vector<value_type>& a, const MI& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }
  /// @copydoc check_same_size(const MI&, const MI&)
  static void check_same_size(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }

  /// @brief Returns @c true iff @p a and @p b agree in every dimension.
  static bool cmpVecForEqual(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] != b[dim]) return false;
    }
    return true;
  }

  /// @brief Returns @c true iff @p a and @p b differ in at least one dimension.
  static bool cmpVecForUnequal(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] != b[dim]) return true;
    }
    return false;
  }

  /// @brief Returns @c true iff @p a is strictly less than @p b in every dimension.
  static bool cmpVecForLess(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] >= b[dim]) return false;
    }
    return true;
  }

  /// @brief Returns @c true iff @p a is less than or equal to @p b in every dimension.
  static bool cmpVecForLessEqual(const std::vector<value_type>& a,
                                 const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] > b[dim]) return false;
    }
    return true;
  }

  /// @brief Returns @c true iff @p a is strictly greater than @p b in every dimension.
  static bool cmpVecGreater(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] <= b[dim]) return false;
    }
    return true;
  }

  /// @brief Returns @c true iff @p a is greater than or equal to @p b in every dimension.
  static bool cmpVecGreaterEqual(const std::vector<value_type>& a,
                                 const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] < b[dim]) return false;
    }
    return true;
  }
};

}  // namespace combigrid
}  // namespace sgpp

#endif
