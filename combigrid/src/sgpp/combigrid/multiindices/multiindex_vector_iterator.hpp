// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_iterator.hpp
 * @brief Random-access proxy iterators over the multi-indices of an @c MIVec.
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_reference.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {

/**
 * @brief Random-access iterator over the multi-indices stored in an @c MIVec.
 *
 * Dereferencing yields a proxy by value instead of a true reference:
 * @c MIRef<T> for the mutable and @c MIView<T> for the const iterator. The
 * @c value_type is the owning @c MI<T>, so temporaries created by algorithms
 * (@c value_type tmp = std::move(*it)) are independent copies.
 *
 * Iterator category:
 * - @c iterator_concept is @c std::random_access_iterator_tag; the iterator
 *   models C++20's @c std::random_access_iterator (and, for the mutable
 *   iterator, @c std::permutable / @c std::sortable), so the @c std::ranges
 *   algorithms accept it.
 * - @c iterator_category is @c std::random_access_iterator_tag as well.
 *   Strictly, C++17 forward iterators must return @c value_type&, which a
 *   proxy iterator cannot; @c std::vector<bool>::iterator declares the same
 *   category for the same reason. libstdc++ and libc++ support such
 *   iterators in their classic algorithms (they use @c value_type
 *   temporaries, @c swap(*a, *b) and @c *a = std::move(*b)), which is
 *   verified by the algorithm tests of @c MIVec.
 *
 * The iterator stores the index of the multi-index instead of a pointer, so
 * that iterator arithmetic also works for zero-dimensional multi-indices.
 *
 * @tparam T       Element type of the multi-indices.
 * @tparam IsConst @c true for the const iterator.
 */
template <typename T, bool IsConst>
class MIVecIterator {
 public:
  using value_type = MI<T>;  ///< Owning element type.
  /// @brief Proxy returned by dereferencing: @c MIView<T> (const) or @c MIRef<T>.
  using reference = typename std::conditional<IsConst, MIView<T>, MIRef<T>>::type;
  using difference_type = std::ptrdiff_t;
  using pointer = void;  ///< There is no addressable element object.
  using iterator_category = std::random_access_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;
  /// @brief Pointer to the flat storage of the @c MIVec.
  using storage_pointer = typename std::conditional<IsConst, const T*, T*>::type;

  /// @brief Constructs a singular iterator.
  MIVecIterator() noexcept = default;

  /**
   * @brief Constructs an iterator to the @p miIdx -th multi-index.
   * @param base  Pointer to the flat storage of the @c MIVec.
   * @param nDim  Dimensionality of the stored multi-indices (the stride).
   * @param miIdx Index of the multi-index the iterator points to.
   */
  MIVecIterator(const storage_pointer base, const size_t nDim, const difference_type miIdx) noexcept
      : base_(base), nDim_(nDim), miIdx_(miIdx) {}

  /// @brief Converts a mutable iterator into a const iterator.
  template <bool OtherIsConst, typename std::enable_if<IsConst && !OtherIsConst, int>::type = 0>
  MIVecIterator(const MIVecIterator<T, OtherIsConst>& other) noexcept
      : base_(other.base()), nDim_(other.nDim()), miIdx_(other.index()) {}

  /// @brief Pointer to the flat storage of the @c MIVec.
  storage_pointer base() const noexcept { return base_; }
  /// @brief Dimensionality of the multi-indices (stride of the storage).
  size_t nDim() const noexcept { return nDim_; }
  /// @brief Index of the multi-index the iterator points to.
  difference_type index() const noexcept { return miIdx_; }

  /// @brief Returns a proxy to the multi-index the iterator points to.
  reference operator*() const noexcept {
    assert(miIdx_ >= 0);
    return reference(base_ + static_cast<size_t>(miIdx_) * nDim_, nDim_);
  }
  /// @brief Returns a proxy to the multi-index @p n positions ahead.
  reference operator[](const difference_type n) const noexcept { return *(*this + n); }

  /// @name Iterator arithmetic
  /// @{
  MIVecIterator& operator++() noexcept {
    ++miIdx_;
    return *this;
  }
  MIVecIterator operator++(int) noexcept {
    MIVecIterator old(*this);
    ++miIdx_;
    return old;
  }
  MIVecIterator& operator--() noexcept {
    --miIdx_;
    return *this;
  }
  MIVecIterator operator--(int) noexcept {
    MIVecIterator old(*this);
    --miIdx_;
    return old;
  }
  MIVecIterator& operator+=(const difference_type n) noexcept {
    miIdx_ += n;
    return *this;
  }
  MIVecIterator& operator-=(const difference_type n) noexcept {
    miIdx_ -= n;
    return *this;
  }
  friend MIVecIterator operator+(MIVecIterator it, const difference_type n) noexcept {
    return it += n;
  }
  friend MIVecIterator operator+(const difference_type n, MIVecIterator it) noexcept {
    return it += n;
  }
  friend MIVecIterator operator-(MIVecIterator it, const difference_type n) noexcept {
    return it -= n;
  }
  friend difference_type operator-(const MIVecIterator& a, const MIVecIterator& b) noexcept {
    assertSameRange(a, b);
    return a.miIdx_ - b.miIdx_;
  }
  /// @}

  /// @name Iterator comparison
  /// @brief Compare positions; both iterators must belong to the same @c MIVec.
  /// @{
  friend bool operator==(const MIVecIterator& a, const MIVecIterator& b) noexcept {
    assertSameRange(a, b);
    return a.miIdx_ == b.miIdx_;
  }
  friend bool operator!=(const MIVecIterator& a, const MIVecIterator& b) noexcept {
    return !(a == b);
  }
  friend bool operator<(const MIVecIterator& a, const MIVecIterator& b) noexcept {
    assertSameRange(a, b);
    return a.miIdx_ < b.miIdx_;
  }
  friend bool operator>(const MIVecIterator& a, const MIVecIterator& b) noexcept { return b < a; }
  friend bool operator<=(const MIVecIterator& a, const MIVecIterator& b) noexcept {
    return !(b < a);
  }
  friend bool operator>=(const MIVecIterator& a, const MIVecIterator& b) noexcept {
    return !(a < b);
  }
  /// @}

 private:
  storage_pointer base_ = nullptr;  ///< Flat storage of the @c MIVec.
  size_t nDim_ = 0;                 ///< Dimensionality (stride).
  difference_type miIdx_ = 0;       ///< Index of the current multi-index.

  /// @brief Debug check that @p a and @p b iterate over the same storage.
  static void assertSameRange(const MIVecIterator& a, const MIVecIterator& b) noexcept {
    assert(a.base_ == b.base_ && a.nDim_ == b.nDim_);
    static_cast<void>(a);
    static_cast<void>(b);
  }
};

/**
 * @brief Customization of @c std::ranges::iter_move for @c MIVec iterators.
 *
 * Returns the proxy itself: assigning it to another element copies the
 * @f$d@f$ entries, and constructing a @c value_type from it copies them into
 * a new @c MI. The referenced multi-index is left unchanged.
 */
template <typename T, bool IsConst>
typename MIVecIterator<T, IsConst>::reference iter_move(
    const MIVecIterator<T, IsConst>& it) noexcept {
  return *it;
}

/**
 * @brief Customization of @c std::ranges::iter_swap for mutable @c MIVec
 * iterators: swaps the entries of the two referenced multi-indices in place.
 */
template <typename T>
void iter_swap(const MIVecIterator<T, false>& a, const MIVecIterator<T, false>& b) noexcept {
  swap(*a, *b);
}

}  // namespace combigrid
}  // namespace sgpp
