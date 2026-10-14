// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file flat_nested_vector_view.hpp
 * @brief Defines @c FlatNestedVecView, the non-owning view of a sub-vector of a
 * @c FlatNestedVec, and @c FlatNestedVecEntry, the type obtained by indexing one.
 */
#pragma once

#include <array>
#include <cassert>
#include <compare>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T, std::size_t R, typename Index>
class FlatNestedVecView;

/**
 * @brief Selects the type that represents an entry of nesting depth @p R of a
 * @c FlatNestedVec, i.e. what @c nested[x1]...[xk] is in the equivalent nested
 * @c std::vector when @p R levels of nesting remain below it.
 *
 * - @p R >= 2: @c FlatNestedVecView<T, R, Index>,
 * - @p R == 1: @c std::span<T> (the elements of a sub-vector are contiguous),
 * - @p R == 0: @c T& (a single element).
 *
 * @p T is @c const-qualified for read-only access.
 */
template <typename T, std::size_t R, typename Index>
struct FlatNestedVecEntrySelector {
  using type = FlatNestedVecView<T, R, Index>;
};

/// @copydoc FlatNestedVecEntrySelector
template <typename T, typename Index>
struct FlatNestedVecEntrySelector<T, 1, Index> {
  using type = std::span<T>;
};

/// @copydoc FlatNestedVecEntrySelector
template <typename T, typename Index>
struct FlatNestedVecEntrySelector<T, 0, Index> {
  using type = T&;
};

/// @brief Shorthand for @c FlatNestedVecEntrySelector<T, R, Index>::type.
template <typename T, std::size_t R, typename Index>
using FlatNestedVecEntry = typename FlatNestedVecEntrySelector<T, R, Index>::type;

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#if defined(__cpp_lib_ranges)
namespace std {
namespace ranges {

/// @brief @c FlatNestedVecView is a view: copying it is @f$O(1)@f$ and it owns no elements.
template <typename T, std::size_t R, typename Index>
inline constexpr bool enable_view<sgpp::combigrid::misc::FlatNestedVecView<T, R, Index>> = true;

/// @brief Iterators of a @c FlatNestedVecView stay valid after the view object is destroyed.
template <typename T, std::size_t R, typename Index>
inline constexpr bool
    enable_borrowed_range<sgpp::combigrid::misc::FlatNestedVecView<T, R, Index>> = true;

}  // namespace ranges
}  // namespace std
#endif

namespace sgpp {
namespace combigrid {
namespace misc {

template <typename T, std::size_t D, typename Index>
class FlatNestedVec;

/**
 * @brief Non-owning view of a sub-vector of a @c FlatNestedVec that has
 * @p R >= 2 levels of nesting below it, i.e. the counterpart of a
 * @c std::vector<std::vector<...<T>>> of depth @p R.
 *
 * Indexing the view with @c operator[] yields its children: views of depth
 * @p R-1, or @c std::span<T> for @p R == 2. Iterating over the view visits the
 * same children in order. @ref flat() returns all elements below the view as
 * one contiguous @c std::span<T>.
 *
 * Like @c std::span, the view is shallow-const: a @c const view still grants
 * write access to the elements if @p T is not @c const. Use
 * @c FlatNestedVecView<const T, R, Index> for read-only access; a mutable view
 * converts to it implicitly.
 *
 * <b>Cost.</b> The view consists of @p R+1 pointers and integers, is trivially
 * copyable and never allocates; pass it by value. Indexing costs two loads of
 * the offset array (one of them only needed for the size of the child). The
 * view stores the base pointers of all deeper offset arrays itself, so code
 * that keeps a view in a local variable does not reload them from the
 * container on every access.
 *
 * <b>Invalidation.</b> The view refers to the storage of the @c FlatNestedVec
 * and follows the rules of its iterators: every operation that may reallocate
 * (e.g. @c push_back, @c appendSubVec, @c reserve) invalidates it. Writes
 * through the container are visible through the view and vice versa.
 *
 * @tparam T     Element type, @c const-qualified for read-only views.
 * @tparam R     Remaining nesting depth below the view (at least 2).
 * @tparam Index Unsigned integer type of the offsets (see @c FlatNestedVec).
 */
template <typename T, std::size_t R, typename Index>
class FlatNestedVecView {
  static_assert(R >= 2, "sub-vectors of depth 1 are represented by std::span<T>");

  /// @brief Base pointers of the offset arrays of the depths below the children.
  using Bases = std::array<const Index*, R - 2>;

 public:
  class iterator;

  /// @name Container types
  /// @{
  using element_type = T;                                  ///< Element type of the leaves.
  using value_type = FlatNestedVecEntry<T, R - 1, Index>;  ///< Type of a child.
  using reference = value_type;                            ///< Children are returned by value.
  using const_iterator = iterator;                         ///< Same as @ref iterator.
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using index_type = Index;
  /// @}

  /// @brief Remaining nesting depth below the view.
  static constexpr std::size_t DEPTH = R;

  /**********
  Constructor
  **********/
  /// @brief Constructs an empty view (no children).
  FlatNestedVecView() noexcept = default;

  /// @brief Converts a mutable view into a read-only view of the same sub-vector.
  template <typename U>
    requires(std::is_const_v<T> && !std::is_const_v<U> && std::is_same_v<const U, T>)
  FlatNestedVecView(const FlatNestedVecView<U, R, Index>& other) noexcept
      : off_(other.off_), size_(other.size_), deeper_(other.deeper_), data_(other.data_) {}

  /*****
  Getter
  *****/
  /// @brief Returns the number of children.
  size_type size() const noexcept { return size_; }
  /// @brief Returns @c true iff the view has no children.
  bool empty() const noexcept { return size_ == 0; }

  /**
   * @brief Returns the @p idx -th child: a view of depth @p R-1, or
   * @c std::span<T> if @p R == 2.
   * @param idx Index of the child; less than @ref size().
   */
  value_type operator[](const size_type idx) const noexcept {
    assert(idx < size_);
    return makeChild(off_ + idx, deeper_, data_);
  }

  /// @brief Returns the first child.
  value_type front() const noexcept { return (*this)[0]; }
  /// @brief Returns the last child.
  value_type back() const noexcept { return (*this)[size_ - 1]; }

  /**
   * @brief Returns all elements below the view, in order, as one contiguous
   * span.
   *
   * Costs @f$2(R-1)@f$ loads of offsets.
   */
  std::span<T> flat() const noexcept {
    if (size_ == 0) return {};
    std::size_t first = off_[0];
    std::size_t last = off_[size_];
    for (std::size_t k = 0; k < R - 2; k++) {
      // An empty range may lie in a depth without any entries (null offset array).
      if (first == last) return {};
      first = deeper_[k][first];
      last = deeper_[k][last];
    }
    return std::span<T>(data_ + first, last - first);
  }

  /*******
  Iterator
  *******/
  /// @brief Iterator to the first child.
  iterator begin() const noexcept { return iterator(off_, deeper_, data_); }
  /// @brief Iterator past the last child.
  iterator end() const noexcept { return iterator(off_ + size_, deeper_, data_); }
  /// @brief Iterator to the first child.
  iterator cbegin() const noexcept { return begin(); }
  /// @brief Iterator past the last child.
  iterator cend() const noexcept { return end(); }

  /**
   * @brief Random-access iterator over the children of a @c FlatNestedVecView.
   *
   * Dereferencing yields the child by value (a view or @c std::span), so the
   * iterator models C++20's @c std::random_access_iterator but declares only
   * @c std::input_iterator_tag as its C++17 @c iterator_category (like the
   * iterators of the standard range adaptors that return prvalues).
   */
  class iterator {
   public:
    using value_type = FlatNestedVecView::value_type;
    using reference = FlatNestedVecView::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = void;  ///< There is no addressable child object.
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;

    /// @brief Constructs a singular iterator.
    iterator() noexcept = default;

    /// @brief Returns the child the iterator points to.
    reference operator*() const noexcept { return makeChild(off_, deeper_, data_); }
    /// @brief Returns the child @p n positions ahead.
    reference operator[](const difference_type n) const noexcept {
      return makeChild(off_ + n, deeper_, data_);
    }

    /// @name Iterator arithmetic
    /// @{
    iterator& operator++() noexcept {
      ++off_;
      return *this;
    }
    iterator operator++(int) noexcept {
      iterator old(*this);
      ++off_;
      return old;
    }
    iterator& operator--() noexcept {
      --off_;
      return *this;
    }
    iterator operator--(int) noexcept {
      iterator old(*this);
      --off_;
      return old;
    }
    iterator& operator+=(const difference_type n) noexcept {
      off_ += n;
      return *this;
    }
    iterator& operator-=(const difference_type n) noexcept {
      off_ -= n;
      return *this;
    }
    friend iterator operator+(iterator it, const difference_type n) noexcept { return it += n; }
    friend iterator operator+(const difference_type n, iterator it) noexcept { return it += n; }
    friend iterator operator-(iterator it, const difference_type n) noexcept { return it -= n; }
    friend difference_type operator-(const iterator& a, const iterator& b) noexcept {
      return a.off_ - b.off_;
    }
    /// @}

    /// @name Iterator comparison
    /// @brief Compare positions; both iterators must belong to the same view.
    /// @{
    friend bool operator==(const iterator& a, const iterator& b) noexcept {
      return a.off_ == b.off_;
    }
    friend std::strong_ordering operator<=>(const iterator& a, const iterator& b) noexcept {
      return a.off_ <=> b.off_;
    }
    /// @}

   private:
    friend class FlatNestedVecView;

    iterator(const Index* const off, const Bases& deeper, T* const data) noexcept
        : off_(off), deeper_(deeper), data_(data) {}

    const Index* off_ = nullptr;  ///< Offset entry of the current child.
    Bases deeper_{};              ///< See @c FlatNestedVecView::deeper_.
    T* data_ = nullptr;           ///< See @c FlatNestedVecView::data_.
  };

 private:
  template <typename, std::size_t, typename>
  friend class FlatNestedVecView;
  template <typename, std::size_t, typename>
  friend class FlatNestedVec;

  /**
   * @brief Offset entries of the children: child @c i owns the entries
   * @c [off_[i], off_[i+1]) of the next depth. Points into an offset array of
   * the container (@ref size_ + 1 readable entries).
   */
  const Index* off_ = nullptr;
  size_type size_ = 0;  ///< Number of children.
  /**
   * @brief Base pointers of the offset arrays of the @p R-2 depths below the
   * children, from the grandchildren downwards (empty for @p R == 2).
   */
  Bases deeper_{};
  T* data_ = nullptr;  ///< First element of the container (not of the view).

  FlatNestedVecView(const Index* const off, const size_type size, const Bases& deeper,
                    T* const data) noexcept
      : off_(off), size_(size), deeper_(deeper), data_(data) {}

  /**
   * @brief Constructs the child whose offset entry is @p off, i.e. the entries
   * @c [off[0], off[1]) of the next depth.
   *
   * Static and taking all state explicitly so that @c FlatNestedVec can index
   * its root and @ref iterator its position without constructing a view first.
   */
  static value_type makeChild(const Index* const off, const Bases& deeper,
                              T* const data) noexcept {
    const std::size_t first = off[0];
    const std::size_t count = off[1] - off[0];
    if constexpr (R == 2) {
      return std::span<T>(data + first, count);
    } else {
      return FlatNestedVecView<T, R - 1, Index>(deeper[0] + first, count, tail(deeper), data);
    }
  }

  /// @brief Base pointers of the child's deeper depths (the guard keeps the size valid for R == 2).
  using ChildBases = std::array<const Index*, (R > 2 ? R - 3 : 0)>;

  /// @brief Drops the first base pointer: the bases of the child's deeper depths.
  static ChildBases tail(const Bases& bases) noexcept {
    static_assert(R > 2, "children of depth 1 (std::span) need no base pointers");
    return [&]<std::size_t... K>(std::index_sequence<K...>) {
      return ChildBases{bases[K + 1]...};
    }(std::make_index_sequence<R - 3>{});
  }
};

/*************************************************************
Compile-time checks of the view and range properties.
*************************************************************/
static_assert(std::is_trivially_copyable_v<FlatNestedVecView<double, 3, std::size_t>>,
              "FlatNestedVecView is passed by value and must stay trivially copyable");

#if defined(__cpp_lib_ranges)
static_assert(std::ranges::view<FlatNestedVecView<double, 3, std::size_t>>);
static_assert(std::ranges::borrowed_range<FlatNestedVecView<double, 3, std::size_t>>);
static_assert(std::ranges::random_access_range<FlatNestedVecView<double, 3, std::size_t>>);
static_assert(std::ranges::sized_range<FlatNestedVecView<double, 2, std::size_t>>);
static_assert(
    std::same_as<std::ranges::range_reference_t<FlatNestedVecView<double, 2, std::size_t>>,
                 std::span<double>>);
static_assert(
    std::same_as<std::ranges::range_reference_t<FlatNestedVecView<double, 3, std::size_t>>,
                 FlatNestedVecView<double, 2, std::size_t>>);
#endif

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
