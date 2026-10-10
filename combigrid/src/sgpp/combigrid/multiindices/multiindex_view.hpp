// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_view.hpp
 * @brief Defines the read-only multi-index view @c MIView and the comparison
 * operators of the views.
 *
 * @c MIView (read-only, this file) and @c MIRef (mutable, see
 * @c multiindex_reference.hpp) refer to @f$d@f$ consecutive entries, e.g. one
 * multi-index inside an @c MIVec or the storage of an @c MI. They compare
 * exactly like @c MI (with @c MI and with each other) without materializing
 * an @c MI, and convert to an owning @c MI on demand.
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_common_reference.hpp>
#include <sgpp/combigrid/tools/multiindex_view/multiindex_view_utilities.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIRef;  ///< Forward declaration to avoid circular includes.

/**
 * @brief Read-only, non-owning view of one multi-index.
 *
 * Refers to @c nDim() consecutive entries of type @c T. Like
 * @c std::string_view, assigning one @c MIView to another rebinds the view;
 * it can never modify the referenced entries. The referenced storage must
 * outlive the view.
 *
 * Implicitly constructible from @c MI and @c MIRef, so functions taking an
 * @c MIView accept all three without copying. Implicitly convertible to an
 * owning @c MI.
 *
 * @tparam T Integral element type.
 */
template <typename T>
class MIView {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = const T*;
  using const_pointer = const T*;
  using reference = const T&;
  using const_reference = const T&;
  using iterator = const T*;
  using const_iterator = const T*;

  /// @brief Constructs an empty (zero-dimensional) view.
  MIView() noexcept = default;

  /**
   * @brief Constructs a view of @p nDim entries starting at @p data.
   * @param data Pointer to the first entry.
   * @param nDim Number of entries (dimensions).
   */
  MIView(const T* data, const size_t nDim) noexcept : data_(data), nDim_(nDim) {}

  /// @brief Views the entries of @p mi. @p mi must outlive the view.
  MIView(const MI<T>& mi) noexcept  // NOLINT(runtime/explicit): implicit by design.
      : data_(mi.data()), nDim_(mi.size()) {}

  /// @brief Read-only view of the entries referenced by @p ref.
  MIView(const MIRef<T>& ref) noexcept  // NOLINT(runtime/explicit): implicit by design.
      : data_(ref.data()), nDim_(ref.size()) {}

  /// @brief Returns the number of dimensions.
  size_t nDim() const noexcept { return nDim_; }
  /// @brief Returns the number of dimensions (alias of @ref nDim()).
  size_t size() const noexcept { return nDim_; }
  /// @brief Returns @c true iff the view has zero dimensions.
  bool empty() const noexcept { return nDim_ == 0; }

  /// @brief Unchecked (debug-asserted) access to the entry of dimension @p dim.
  const T& operator[](const size_t dim) const noexcept {
    assert(dim < nDim_);
    return data_[dim];
  }
  /// @brief Returns the first entry.
  const T& front() const noexcept { return (*this)[0]; }
  /// @brief Returns the last entry.
  const T& back() const noexcept { return (*this)[nDim_ - 1]; }

  /// @brief Pointer to the first entry.
  const T* data() const noexcept { return data_; }
  /// @brief Iterator to the first entry.
  const T* begin() const noexcept { return data_; }
  /// @brief Iterator past the last entry.
  const T* end() const noexcept { return data_ + nDim_; }

  /// @brief Sum of all entries, same semantics as @c MI::sumOfElems().
  T sumOfElems() const noexcept { return sumOfElems<T>(); }

  /// @brief Sum of all entries accumulated in @p U, same semantics as @c MI::sumOfElems<U>().
  template <typename U>
  U sumOfElems() const noexcept {
    U sum = 0;
    for (size_t dim = 0; dim < nDim_; dim++) {
      sum += static_cast<U>(data_[dim]);
    }
    return sum;
  }

  /// @brief Product of all entries (@c 0 if empty), same semantics as @c MI::productofElems().
  T productofElems() const noexcept { return productofElems<T>(); }

  /**
   * @brief Product of all entries accumulated in @p U (@c 0 if empty), same
   * semantics as @c MI::productofElems<U>().
   */
  template <typename U>
  U productofElems() const noexcept {
    if (nDim_ == 0) {
      return 0;
    }
    U product = 1;
    for (size_t dim = 0; dim < nDim_; dim++) {
      product *= static_cast<U>(data_[dim]);
    }
    return product;
  }

  /// @brief Returns an owning copy of the viewed entries.
  MI<T> toMI() const { return MI<T>(begin(), end()); }

  /// @brief Implicit conversion to an owning copy (see @ref toMI()).
  operator MI<T>() const { return toMI(); }

 private:
  const T* data_ = nullptr;  ///< First viewed entry.
  size_t nDim_ = 0;          ///< Number of viewed entries.
};

/// @name Comparison operators involving views
/// @brief Comparisons of @c MIView / @c MIRef with each other and with @c MI.
///
/// Identical semantics to the comparison operators of @c MI: equality is
/// element-wise; the relational operators require the relation to hold in
/// @em every dimension (component-wise partial order, @em not a strict weak
/// ordering; use @c MILexLess for sorting). All of them throw
/// @c std::logic_error if the operands have different sizes. No operand is
/// converted to an @c MI.
/// @{
template <typename A, typename B,
          typename std::enable_if<tools::mi_view::areViewComparable<A, B>(), int>::type = 0>
bool operator==(const A& a, const B& b) {
  return tools::mi_view::equal<typename A::value_type>(a, b);
}

template <typename A, typename B,
          typename std::enable_if<tools::mi_view::areViewComparable<A, B>(), int>::type = 0>
bool operator!=(const A& a, const B& b) {
  return !tools::mi_view::equal<typename A::value_type>(a, b);
}

template <typename A, typename B,
          typename std::enable_if<tools::mi_view::areViewComparable<A, B>(), int>::type = 0>
bool operator<(const A& a, const B& b) {
  return tools::mi_view::lessInEveryDim<typename A::value_type>(a, b);
}

template <typename A, typename B,
          typename std::enable_if<tools::mi_view::areViewComparable<A, B>(), int>::type = 0>
bool operator<=(const A& a, const B& b) {
  return tools::mi_view::lessEqualInEveryDim<typename A::value_type>(a, b);
}

template <typename A, typename B,
          typename std::enable_if<tools::mi_view::areViewComparable<A, B>(), int>::type = 0>
bool operator>(const A& a, const B& b) {
  return tools::mi_view::lessInEveryDim<typename A::value_type>(b, a);
}

template <typename A, typename B,
          typename std::enable_if<tools::mi_view::areViewComparable<A, B>(), int>::type = 0>
bool operator>=(const A& a, const B& b) {
  return tools::mi_view::lessEqualInEveryDim<typename A::value_type>(b, a);
}
/// @}

}  // namespace combigrid
}  // namespace sgpp
