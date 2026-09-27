// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_reference.hpp
 * @brief Defines the mutable multi-index proxy reference @c MIRef.
 *
 * @c MIRef is the @c reference type of @c MIVec::iterator. It is a proxy:
 * assigning to it copies the entries into the referenced storage and never
 * rebinds the proxy. Copying the proxy object itself (e.g. @c auto e = vec[i])
 * yields another proxy to the same storage, not an independent copy; use
 * @c MI<T> e = vec[i] or @c toMI() for that. Comparisons are provided by the
 * operators in @c multiindex_view.hpp.
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Mutable proxy reference to one multi-index.
 *
 * Refers to @c nDim() consecutive entries of type @c T and is the
 * @c reference type of @c MIVec::iterator. Reference semantics:
 * - assignment (from @c MIRef, @c MIView or @c MI) copies the entries into
 *   the referenced storage; the proxy is never rebound. The sizes must match.
 * - copy construction creates a second proxy to the @em same storage.
 * - @c swap exchanges the referenced entries.
 *
 * The assignment operators are @c const (the constness of the proxy is
 * shallow, like that of a pointer); this is what C++20's
 * @c std::indirectly_writable requires from proxy references.
 *
 * @tparam T Integral element type.
 */
template <typename T>
class MIRef {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = T*;
  using const_iterator = T*;

  /**
   * @brief Constructs a proxy to @p nDim entries starting at @p data.
   * @param data Pointer to the first entry.
   * @param nDim Number of entries (dimensions).
   */
  MIRef(T* data, const size_t nDim) noexcept : data_(data), nDim_(nDim) {}

  /// @brief Creates a second proxy referring to the same storage as @p other.
  MIRef(const MIRef& other) noexcept = default;

  /**
   * @brief Copies the entries referenced by @p other into the storage
   * referenced by this proxy.
   * @pre Both proxies have the same size and do not partially overlap.
   */
  const MIRef& operator=(const MIRef& other) const noexcept {
    assign(other.data_, other.nDim_);
    return *this;
  }

  /**
   * @brief Copies the entries of @p other (an @c MIView, @c MI, ...) into the
   * storage referenced by this proxy.
   * @pre Same size and no partial overlap.
   */
  const MIRef& operator=(const MIView<T> other) const noexcept {
    assign(other.data(), other.size());
    return *this;
  }

  /// @brief Returns the number of dimensions.
  size_t nDim() const noexcept { return nDim_; }
  /// @brief Returns the number of dimensions (alias of @ref nDim()).
  size_t size() const noexcept { return nDim_; }
  /// @brief Returns @c true iff the proxy has zero dimensions.
  bool empty() const noexcept { return nDim_ == 0; }

  /// @brief Unchecked (debug-asserted) mutable access to the entry of dimension @p dim.
  T& operator[](const size_t dim) const noexcept {
    assert(dim < nDim_);
    return data_[dim];
  }
  /// @brief Returns the first entry.
  T& front() const noexcept { return (*this)[0]; }
  /// @brief Returns the last entry.
  T& back() const noexcept { return (*this)[nDim_ - 1]; }

  /// @brief Pointer to the first entry.
  T* data() const noexcept { return data_; }
  /// @brief Iterator to the first entry.
  T* begin() const noexcept { return data_; }
  /// @brief Iterator past the last entry.
  T* end() const noexcept { return data_ + nDim_; }

  /// @copydoc MIView::sumOfElems()
  T sumOfElems() const noexcept { return MIView<T>(*this).sumOfElems(); }
  /// @copydoc MIView::sumOfElems()
  template <typename U>
  U sumOfElems() const noexcept {
    return MIView<T>(*this).template sumOfElems<U>();
  }
  /// @copydoc MIView::productofElems()
  T productofElems() const noexcept { return MIView<T>(*this).productofElems(); }
  /// @copydoc MIView::productofElems()
  template <typename U>
  U productofElems() const noexcept {
    return MIView<T>(*this).template productofElems<U>();
  }

  /// @brief Returns an owning copy of the referenced entries.
  MI<T> toMI() const { return MI<T>(begin(), end()); }

  /// @brief Implicit conversion to an owning copy (see @ref toMI()).
  operator MI<T>() const { return toMI(); }

  /**
   * @brief Swaps the entries referenced by @p a and @p b.
   *
   * Takes the proxies by value so that it also applies to the prvalue proxies
   * returned by dereferencing an iterator (@c std::iter_swap calls
   * @c swap(*a, *b)).
   * @pre Same size.
   */
  friend void swap(const MIRef a, const MIRef b) noexcept {
    assert(a.nDim_ == b.nDim_);
    std::swap_ranges(a.data_, a.data_ + a.nDim_, b.data_);
  }

 private:
  T* data_;      ///< First referenced entry.
  size_t nDim_;  ///< Number of referenced entries.

  /// @brief Copies @p nDim entries from @p src into the referenced storage.
  void assign(const T* src, const size_t nDim) const noexcept {
    assert(nDim == nDim_);
    if (src != data_) {
      std::copy_n(src, nDim, data_);
    }
  }
};

}  // namespace combigrid
}  // namespace sgpp
