// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file flat_nested_vector.hpp
 * @brief Defines @c FlatNestedVec, a ragged @c std::vector<std::vector<...<T>>>
 * of fixed nesting depth whose elements are stored in one contiguous array.
 */
#pragma once

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <limits>
#include <ranges>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector_view.hpp>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {

/// @brief Recursion of @c NestedStdVec.
template <typename T, std::size_t D>
struct NestedStdVecSelector {
  using type = std::vector<typename NestedStdVecSelector<T, D - 1>::type>;
};

/// @copydoc NestedStdVecSelector
template <typename T>
struct NestedStdVecSelector<T, 1> {
  using type = std::vector<T>;
};

/**
 * @brief @c std::vector nested @p D times, e.g.
 * @c NestedStdVec<double, 3> = @c std::vector<std::vector<std::vector<double>>>.
 */
template <typename T, std::size_t D>
using NestedStdVec = typename NestedStdVecSelector<T, D>::type;

/**
 * @brief Ragged @c std::vector<std::vector<...<T>>> of nesting depth @p D,
 * stored in flat arrays.
 *
 * <b>Terminology.</b> In the equivalent nested vector @c nested, an entry of
 * depth @f$k \in \{1, \ldots, D\}@f$ is @c nested[x1]...[xk]: a sub-vector for
 * @f$k < D@f$, an element of type @p T for @f$k = D@f$. Sub-vectors may have
 * different lengths, including zero.
 *
 * <b>Usage.</b> The container is a @c std::vector<T> of all elements (in the
 * order of a depth-first traversal) that additionally knows the nesting:
 * - @c size(), @c data(), the iterators, @c push_back(), @c reserve() etc.
 *   refer to the flat sequence of elements, so the container is a contiguous
 *   range of @p T and works with all standard algorithms.
 * - @c fv[x1][x2]...[xk] equals @c nested[x1][x2]...[xk]: a
 *   @c FlatNestedVecView for @f$k < D-1@f$, a @c std::span<T> for
 *   @f$k = D-1@f$ and a @c T& for @f$k = D@f$. None of them copies elements.
 * - @ref nested() returns the whole container as a view of depth @p D (whose
 *   @c size() is the number of top-level sub-vectors and whose iteration
 *   visits them).
 *
 * The container is built by converting a nested @c std::vector or
 * incrementally, mirroring the calls on a nested vector:
 * @code
 * FlatNestedVec<double, 3> fv;  // nested
 * fv.appendSubVec(1);           // nested.emplace_back();
 * fv.appendSubVec(2);           // nested.back().emplace_back();
 * fv.push_back(1.5);            // nested.back().back().push_back(1.5);
 * fv.appendSubVec(2, 4);        // nested.back().emplace_back(4);
 * double x = fv[0][1][3];       // nested[0][1][3]
 * @endcode
 *
 * <b>Storage and cost.</b> Compressed-sparse-row format over all depths: one
 * @c std::vector<T> with the elements and, for every depth @f$k < D@f$, an
 * offset array whose entries @c i and @c i+1 bound the children of the
 * @c i -th sub-vector of depth @f$k@f$. Compared to a nested @c std::vector
 * this saves one heap allocation and the 24-byte vector header per
 * sub-vector, and the elements are contiguous. @c fv[x1]...[xD] costs
 * @f$D-1@f$ dependent offset loads plus the element load (a nested vector
 * needs @f$D@f$ dependent pointer loads plus the element load).
 *
 * <b>Hot loops.</b> SG++ compiles with strict aliasing, so storing elements
 * does not make the compiler reload the container's array pointers and
 * offsets: in a loop, chained @c fv[x1]...[xD] writes compile like writes
 * through a local span (@c FlatNestedVec<double, 3>: 0.64 ns per element
 * either way, nested vector 0.67 ns; 4.5 ns chained with
 * @c -fno-strict-aliasing). Keep the view or span of the sub-vector being
 * worked on in a local variable (or iterate with range-for) when the loop also
 * - stores pointers to @p T, e.g. into other containers (GCC gives pointers to
 *   @c T and to @c const @c T one alias set, so they alias the container's
 *   element pointer: chained reads with a @c push_back of
 *   @c {const double*, std::size_t, double} per element were 1.35x slower),
 * - stores values of type @p Index, which alias the offsets, or
 * - calls functions the compiler cannot see into (e.g. recursion).
 * Otherwise every chained access reloads up to @f$2D@f$ pointers and offsets.
 * For large containers accessed randomly, a 32-bit @p Index pays off.
 *
 * <b>Invalidation.</b> Like @c std::vector: every operation that may
 * reallocate the elements or offsets (@c push_back, @c appendSubVec,
 * @c reserve, ...) invalidates all iterators, references, spans and views.
 *
 * @tparam T     Element type; not @c bool (@c std::vector<bool> is not contiguous).
 * @tparam D     Nesting depth, at least 1 (@p D == 1 is a plain @c std::vector<T>).
 * @tparam Index Unsigned integer type of the offsets. A narrower type (e.g.
 *               @c std::uint32_t) shrinks the offset arrays but limits the number
 *               of elements and sub-vectors of every depth to its maximum.
 */
template <typename T, std::size_t D, typename Index = std::size_t>
class FlatNestedVec {
  static_assert(D >= 1, "the nesting depth must be at least 1");
  static_assert(std::unsigned_integral<Index>, "Index must be an unsigned integer type");
  static_assert(!std::is_same_v<T, bool>, "std::vector<bool> is not contiguous; use char");

  using ElemVec = std::vector<T>;
  /// @brief Base pointers of the offset arrays of the depths 2, ..., D-1.
  using Bases = std::array<const Index*, (D >= 2 ? D - 2 : 0)>;

 public:
  /// @name Container types of the flat element sequence
  /// @{
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = typename ElemVec::iterator;
  using const_iterator = typename ElemVec::const_iterator;
  using reverse_iterator = typename ElemVec::reverse_iterator;
  using const_reverse_iterator = typename ElemVec::const_reverse_iterator;
  using index_type = Index;
  /// @}

  /// @name Types of the nested access
  /// @{
  /// @brief Result of @c operator[]: a view, a @c std::span<T> or a @c T&.
  using entry_type = FlatNestedVecEntry<T, D - 1, Index>;
  /// @brief Result of @c operator[] @c const.
  using const_entry_type = FlatNestedVecEntry<const T, D - 1, Index>;
  /// @brief Result of @ref nested(): a view of depth @p D (@c std::span<T> for @p D == 1).
  using nested_view_type = FlatNestedVecEntry<T, D, Index>;
  /// @brief Result of @ref nested() @c const.
  using const_nested_view_type = FlatNestedVecEntry<const T, D, Index>;
  /// @}

  /// @brief Nesting depth.
  static constexpr std::size_t DEPTH = D;

  /***********
  Constructors
  ***********/
  /// @brief Constructs an empty container without allocating.
  FlatNestedVec() noexcept = default;

  /**
   * @brief Constructs the flat representation of the nested vector @p nested.
   *
   * Allocates every array exactly once.
   */
  explicit FlatNestedVec(const NestedStdVec<T, D>& nested) {
    if constexpr (D == 1) {
      data_ = nested;
    } else {
      std::array<size_type, D> nEntriesPerDepth{};
      countEntries<1>(nested, nEntriesPerDepth);
      for (std::size_t k = 0; k + 1 < D; k++) {
        if (nEntriesPerDepth[k] == 0) continue;
        offsets_[k].reserve(nEntriesPerDepth[k] + 1);
        offsets_[k].push_back(0);
      }
      data_.reserve(nEntriesPerDepth[D - 1]);
      appendNested<1>(nested);
    }
  }

  /// @brief Converts back into a nested @c std::vector.
  NestedStdVec<T, D> toNested() const {
    if constexpr (D == 1) {
      return data_;
    } else {
      return toNestedImpl<D>(nested());
    }
  }

  /****************************
  Flat element sequence (sizes)
  ****************************/
  /// @brief Returns the total number of elements (entries of depth @p D).
  size_type size() const noexcept { return data_.size(); }
  /// @brief Returns @c true iff there are no elements (there may be empty sub-vectors).
  bool empty() const noexcept { return data_.empty(); }
  /// @brief Returns the number of elements that fit without reallocation.
  size_type capacity() const noexcept { return data_.capacity(); }

  /**
   * @brief Returns the total number of entries of depth @p depth.
   *
   * @c nEntries(1) is the number of top-level sub-vectors (@c nested.size()),
   * @c nEntries(D) equals @ref size().
   * @param depth Depth in @f$\{1, \ldots, D\}@f$.
   */
  size_type nEntries(const std::size_t depth) const noexcept {
    assert(1 <= depth && depth <= D);
    if constexpr (D > 1) {
      if (depth < D) return nSubVecs(offsets_[depth - 1]);
    }
    return data_.size();
  }

  /***************************
  Flat element sequence access
  ***************************/
  /// @brief Pointer to the contiguous elements.
  T* data() noexcept { return data_.data(); }
  /// @copydoc data()
  const T* data() const noexcept { return data_.data(); }

  /// @brief All elements as one contiguous span.
  std::span<T> flat() noexcept { return std::span<T>(data_); }
  /// @copydoc flat()
  std::span<const T> flat() const noexcept { return std::span<const T>(data_); }

  /// @brief First element (not the first sub-vector; that is @c (*this)[0]).
  T& front() noexcept { return data_.front(); }
  /// @copydoc front()
  const T& front() const noexcept { return data_.front(); }
  /// @brief Last element (not the last sub-vector).
  T& back() noexcept { return data_.back(); }
  /// @copydoc back()
  const T& back() const noexcept { return data_.back(); }

  /// @name Iterators over the flat element sequence
  /// @{
  iterator begin() noexcept { return data_.begin(); }
  iterator end() noexcept { return data_.end(); }
  const_iterator begin() const noexcept { return data_.begin(); }
  const_iterator end() const noexcept { return data_.end(); }
  const_iterator cbegin() const noexcept { return data_.cbegin(); }
  const_iterator cend() const noexcept { return data_.cend(); }
  reverse_iterator rbegin() noexcept { return data_.rbegin(); }
  reverse_iterator rend() noexcept { return data_.rend(); }
  const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }
  const_reverse_iterator rend() const noexcept { return data_.rend(); }
  const_reverse_iterator crbegin() const noexcept { return data_.crbegin(); }
  const_reverse_iterator crend() const noexcept { return data_.crend(); }
  /// @}

  /*************
  Nested access
  *************/
  /**
   * @brief Returns the top-level entry @p idx, i.e. @c nested[idx]: a
   * @c FlatNestedVecView of depth @p D-1 for @p D >= 3, a @c std::span<T> for
   * @p D == 2 and a @c T& for @p D == 1.
   *
   * Chain @c operator[] to reach deeper entries: @c fv[x1][x2]...[xD] is a @c T&.
   * @param idx Index of the entry; less than @c nEntries(1).
   */
  entry_type operator[](const size_type idx) noexcept {
    if constexpr (D == 1) {
      return data_[idx];
    } else {
      assert(idx < nEntries(1));
      return FlatNestedVecView<T, D, Index>::makeChild(offsets_[0].data() + idx, bases(),
                                                       data_.data());
    }
  }

  /// @copydoc operator[](size_type)
  const_entry_type operator[](const size_type idx) const noexcept {
    if constexpr (D == 1) {
      return data_[idx];
    } else {
      assert(idx < nEntries(1));
      return FlatNestedVecView<const T, D, Index>::makeChild(offsets_[0].data() + idx, bases(),
                                                             data_.data());
    }
  }

  /**
   * @brief Returns the whole container as a nested range of depth @p D: a
   * @c FlatNestedVecView whose @c size() and iteration refer to the top-level
   * sub-vectors (@c std::span<T> over the elements for @p D == 1).
   */
  nested_view_type nested() noexcept {
    if constexpr (D == 1) {
      return std::span<T>(data_);
    } else {
      return FlatNestedVecView<T, D, Index>(offsets_[0].data(), nEntries(1), bases(), data_.data());
    }
  }

  /// @copydoc nested()
  const_nested_view_type nested() const noexcept {
    if constexpr (D == 1) {
      return std::span<const T>(data_);
    } else {
      return FlatNestedVecView<const T, D, Index>(offsets_[0].data(), nEntries(1), bases(),
                                                  data_.data());
    }
  }

  /********
  Modifiers
  ********/
  /**
   * @brief Appends a sub-vector of depth @p depth with @p count default
   * children: @c nested.emplace_back(count) for @p depth == 1,
   * @c nested.back().emplace_back(count) for @p depth == 2, etc.
   *
   * The children are empty sub-vectors if @c depth + 1 < D and
   * value-initialized elements if @c depth + 1 == D.
   *
   * @param depth Depth of the new sub-vector in @f$\{1, \ldots, D-1\}@f$. For
   *              @p depth > 1 its parent @c nested.back()...back() (@c depth-1
   *              times) must exist, i.e. the last sub-vectors of the depths
   *              @c 1, ..., depth-2 must not be empty (checked by @c assert).
   * @param count Number of children of the new sub-vector.
   */
  void appendSubVec(const std::size_t depth, const size_type count = 0) {
    static_assert(D >= 2, "a FlatNestedVec of depth 1 has no sub-vectors");
    assert(1 <= depth && depth < D);
    if (depth > 1) {
      assert(lastSubVecExists(depth - 1) && "appendSubVec: nested.back()...back() is invalid");
      Index& parentEnd = offsets_[depth - 1 - 1].back();
      parentEnd = static_cast<Index>(parentEnd + 1);
    }

    std::vector<Index>& off = offsets_[depth - 1];
    if (off.empty()) off.push_back(0);
    assert(sumFitsIndex(off.back(), count) && "Index overflow");
    off.push_back(static_cast<Index>(off.back() + count));
    if (count == 0) return;

    if (depth + 1 < D) {
      std::vector<Index>& childOff = offsets_[depth];
      if (childOff.empty()) childOff.push_back(0);
      const Index childrenEnd = childOff.back();
      childOff.insert(childOff.end(), count, childrenEnd);
    } else {
      data_.resize(data_.size() + count);
    }
  }

  /**
   * @brief Appends an element to the last sub-vector of depth @p D-1:
   * @c nested.back()...back().push_back(value). As for the nested vector, the
   * last sub-vectors of the depths @c 1, ..., D-2 must not be empty (checked
   * by @c assert).
   */
  void push_back(const T& value) {
    assertLastSubVecExists();
    data_.push_back(value);
    growLastSubVec(1);
  }

  /// @copydoc push_back(const T&)
  void push_back(T&& value) {
    assertLastSubVecExists();
    data_.push_back(std::move(value));
    growLastSubVec(1);
  }

  /**
   * @brief Constructs an element in place at the end of the last sub-vector of
   * depth @p D-1 (precondition as for @c push_back).
   * @return Reference to the new element.
   */
  template <typename... Args>
  T& emplace_back(Args&&... args) {
    assertLastSubVecExists();
    T& elem = data_.emplace_back(std::forward<Args>(args)...);
    growLastSubVec(1);
    return elem;
  }

  /**
   * @brief Appends all elements of @p range to the last sub-vector of depth
   * @p D-1 (precondition as for @c push_back).
   */
  template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_reference_t<Range>, T>
  void append_range(Range&& range) {
    assertLastSubVecExists();
    const size_type oldSize = data_.size();
    if constexpr (std::ranges::common_range<Range>) {
      data_.insert(data_.end(), std::ranges::begin(range), std::ranges::end(range));
    } else {
      for (auto&& elem : range) data_.emplace_back(std::forward<decltype(elem)>(elem));
    }
    growLastSubVec(data_.size() - oldSize);
  }

  /**
   * @brief Removes the last element. Empty sub-vectors after the sub-vector
   * that contains it are kept.
   */
  void pop_back() noexcept {
    assert(!data_.empty());
    if constexpr (D > 1) {
      // The last element belongs to the first sub-vector whose end is the old size; all
      // (empty) sub-vectors after it share that end.
      std::vector<Index>& off = offsets_[D - 2];
      const Index oldSize = static_cast<Index>(data_.size());
      for (auto it = off.rbegin(); it != off.rend() && *it == oldSize; ++it) {
        *it = static_cast<Index>(oldSize - 1);
      }
    }
    data_.pop_back();
  }

  /// @brief Removes all elements and sub-vectors; keeps the capacities.
  void clear() noexcept {
    for (std::vector<Index>& off : offsets_) off.clear();
    data_.clear();
  }

  /// @brief Reserves capacity for at least @p nElems elements.
  void reserve(const size_type nElems) { data_.reserve(nElems); }

  /**
   * @brief Reserves capacity for at least @p nSubVecs sub-vectors of depth
   * @p depth in total.
   * @param depth    Depth in @f$\{1, \ldots, D-1\}@f$.
   * @param nSubVecs Number of sub-vectors of that depth.
   */
  void reserveSubVecs(const std::size_t depth, const size_type nSubVecs) {
    static_assert(D >= 2, "a FlatNestedVec of depth 1 has no sub-vectors");
    assert(1 <= depth && depth < D);
    offsets_[depth - 1].reserve(nSubVecs + 1);
  }

  /// @brief Releases unused capacity of all arrays.
  void shrink_to_fit() {
    for (std::vector<Index>& off : offsets_) off.shrink_to_fit();
    data_.shrink_to_fit();
  }

  /// @brief Swaps the contents with @p other.
  void swap(FlatNestedVec& other) noexcept {
    offsets_.swap(other.offsets_);
    data_.swap(other.data_);
  }

  /// @brief Swaps the contents of @p a and @p b.
  friend void swap(FlatNestedVec& a, FlatNestedVec& b) noexcept { a.swap(b); }

  /// @brief Returns @c true iff both containers have the same nesting and elements.
  friend bool operator==(const FlatNestedVec& a, const FlatNestedVec& b) {
    for (std::size_t k = 0; k + 1 < D; k++) {
      const size_type n = nSubVecs(a.offsets_[k]);
      // An offset array without sub-vectors may be empty or {0}; both mean the same.
      if (n != nSubVecs(b.offsets_[k])) return false;
      if (n != 0 && a.offsets_[k] != b.offsets_[k]) return false;
    }
    return a.data_ == b.data_;
  }

 private:
  /**
   * @brief Offset arrays of the depths 1, ..., D-1 (CSR format). @c offsets_[k]
   * is either empty (no sub-vectors of depth @c k+1) or holds @c n+1 entries
   * for @c n sub-vectors, starting with 0: sub-vector @c i of depth @c k+1 owns
   * the entries @c [offsets_[k][i], offsets_[k][i+1]) of depth @c k+2.
   * Hence @c offsets_[k].back() is the number of entries of depth @c k+2.
   * An empty array is allowed so that default construction does not allocate
   * and moved-from containers are valid and empty.
   */
  std::array<std::vector<Index>, D - 1> offsets_;
  ElemVec data_;  ///< All elements, depth-first.

  /// @brief Number of sub-vectors described by the offset array @p off.
  static size_type nSubVecs(const std::vector<Index>& off) noexcept {
    return off.empty() ? 0 : off.size() - 1;
  }

  /// @brief Returns @c true iff @p a + @p b is representable as @p Index.
  static constexpr bool sumFitsIndex(const size_type a, const size_type b) noexcept {
    constexpr size_type MAX = std::numeric_limits<Index>::max();
    return a <= MAX && b <= MAX - a;
  }

  /// @brief Base pointers of the offset arrays below depth 1 (see @c FlatNestedVecView).
  Bases bases() const noexcept {
    return [&]<std::size_t... K>(std::index_sequence<K...>) {
      return Bases{offsets_[K + 1].data()...};
    }(std::make_index_sequence<std::tuple_size_v<Bases>>{});
  }

  /**
   * @brief Returns @c true iff @c nested.back()...back() (@p depth times) is
   * valid, i.e. if there is a sub-vector of depth 1 and the last sub-vectors
   * of the depths @c 1, ..., depth-1 are not empty. Then that sub-vector is
   * the last sub-vector of depth @p depth.
   */
  bool lastSubVecExists(const std::size_t depth) const noexcept {
    if (nEntries(1) == 0) return false;
    for (std::size_t k = 1; k < depth; k++) {
      const std::vector<Index>& off = offsets_[k - 1];
      if (off[off.size() - 2] == off.back()) return false;
    }
    return true;
  }

  void assertLastSubVecExists() const noexcept {
    if constexpr (D > 1) {
      assert(lastSubVecExists(D - 1) && "nested.back()...back() is invalid");
    }
  }

  /// @brief Accounts for @p n elements appended to @c data_.
  void growLastSubVec(const size_type n) noexcept {
    if constexpr (D > 1) {
      assert(data_.size() <= std::numeric_limits<Index>::max() && "Index overflow");
      Index& end = offsets_[D - 2].back();
      end = static_cast<Index>(end + n);
    }
  }

  /// @brief Adds the number of entries of every depth >= @p Depth below @p vec.
  template <std::size_t Depth>
  static void countEntries(const NestedStdVec<T, D - Depth + 1>& vec,
                           std::array<size_type, D>& nEntriesPerDepth) {
    nEntriesPerDepth[Depth - 1] += vec.size();
    if constexpr (Depth < D) {
      for (const auto& sub : vec) countEntries<Depth + 1>(sub, nEntriesPerDepth);
    }
  }

  /// @brief Appends the entries of @p vec (of depth @p Depth) and everything below them.
  template <std::size_t Depth>
  void appendNested(const NestedStdVec<T, D - Depth + 1>& vec) {
    if constexpr (Depth == D) {
      data_.insert(data_.end(), vec.begin(), vec.end());
    } else {
      std::vector<Index>& off = offsets_[Depth - 1];
      for (const auto& sub : vec) {
        assert(sumFitsIndex(off.back(), sub.size()) && "Index overflow");
        off.push_back(static_cast<Index>(off.back() + sub.size()));
        appendNested<Depth + 1>(sub);
      }
    }
  }

  /// @brief Copies the sub-vector @p entry of remaining depth @p R into a nested vector.
  template <std::size_t R>
  static NestedStdVec<T, R> toNestedImpl(const FlatNestedVecEntry<const T, R, Index> entry) {
    if constexpr (R == 1) {
      return NestedStdVec<T, 1>(entry.begin(), entry.end());
    } else {
      NestedStdVec<T, R> result;
      result.reserve(entry.size());
      for (const auto child : entry) result.push_back(toNestedImpl<R - 1>(child));
      return result;
    }
  }
};

/*************************************************************
Compile-time checks of the range properties.
*************************************************************/
#if defined(__cpp_lib_ranges)
static_assert(std::ranges::contiguous_range<FlatNestedVec<double, 3>>);
static_assert(std::ranges::sized_range<FlatNestedVec<double, 3>>);
static_assert(std::same_as<std::ranges::range_reference_t<FlatNestedVec<double, 3>>, double&>);
#endif

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
