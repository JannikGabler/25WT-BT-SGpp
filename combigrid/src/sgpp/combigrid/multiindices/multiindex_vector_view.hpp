// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_view.hpp
 * @brief Defines @c MIVecView, a read-only, non-owning view of a contiguous
 * range of the multi-indices stored in an @c MIVec.
 */
#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector_iterator.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVecView;  ///< Defined below; declared here for the specializations in @c std::ranges.

}  // namespace combigrid
}  // namespace sgpp

#if defined(__cpp_lib_ranges)
namespace std {
namespace ranges {

/// @brief @c MIVecView is a view: copying it is @f$O(1)@f$ and it owns no elements.
template <typename T>
inline constexpr bool enable_view<sgpp::combigrid::MIVecView<T>> = true;

/// @brief Iterators of an @c MIVecView stay valid after the view object is destroyed.
template <typename T>
inline constexpr bool enable_borrowed_range<sgpp::combigrid::MIVecView<T>> = true;

}  // namespace ranges
}  // namespace std
#endif

namespace sgpp {
namespace combigrid {

/**
 * @brief Read-only, non-owning view of a contiguous range of multi-indices
 * that share the same dimensionality, e.g. the first @f$n@f$ multi-indices
 * of an @c MIVec.
 *
 * This is the counterpart of @c std::span<const MI<T>> for the flat storage
 * of @c MIVec (which holds no @c MI objects a @c std::span could refer to).
 * The view only consists of a pointer and three integers: creating, copying
 * and slicing it (@ref first, @ref last, @ref subview) takes constant time and
 * never allocates or copies multi-indices. Pass it by value.
 *
 * <b>Element access.</b> Like a const @c MIVec, @c operator[] and the
 * iterators hand out @c MIView<T> proxies; no @c MI is created. Use
 * @c MI<T> e = view[i] for an independent copy.
 *
 * <b>Iterators.</b> The view is a random-access, sized, borrowed range whose
 * iterator is @c MIVec::const_iterator. The iterators of a view obtained
 * from an @c MIVec (directly or by slicing) are interchangeable with the const
 * iterators of that @c MIVec: they compare equal at the same position and
 * their @c index() is the position within the @c MIVec, not within the view.
 *
 * <b>Invalidation.</b> The view refers to the storage of the @c MIVec and
 * follows the rules of its iterators: it is invalidated by every operation
 * that may reallocate or that removes viewed multi-indices. Writing to viewed
 * multi-indices through the @c MIVec is visible through the view.
 *
 * @tparam T Integral element type of the viewed multi-indices.
 */
template <typename T>
class MIVecView {
 public:
  /// @name Container types
  /// @{
  using value_type = MI<T>;                       ///< Owning element type.
  using reference = MIView<T>;                    ///< Read-only view of a viewed multi-index.
  using const_reference = MIView<T>;              ///< Read-only view of a viewed multi-index.
  using iterator = MIVecIterator<T, true>;        ///< Read-only random-access proxy iterator.
  using const_iterator = MIVecIterator<T, true>;  ///< Same as @ref iterator.
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  /// @}

  /**********
  Constructor
  **********/
  /// @brief Constructs an empty view (no multi-indices, zero dimensions).
  MIVecView() noexcept = default;

  /**
   * @brief Constructs a view of @p nMI multi-indices of dimension @p nDim
   * that are stored back to back starting at @p data.
   *
   * @param data Pointer to the first entry of the first multi-index.
   * @param nDim Number of dimensions of every viewed multi-index.
   * @param nMI  Number of viewed multi-indices.
   */
  MIVecView(const T* const data, const size_t nDim, const size_t nMI) noexcept
      : data_(data), nDim_(nDim), nMI_(nMI) {}

  /*****
  Getter
  *****/
  /// @brief Returns the dimensionality of every viewed multi-index.
  size_t nDim() const noexcept { return nDim_; }
  /// @brief Returns the number of viewed multi-indices.
  size_t nMI() const noexcept { return nMI_; }
  /// @brief Returns the number of viewed multi-indices (alias of @ref nMI()).
  size_t size() const noexcept { return nMI_; }
  /// @brief Returns @c true iff no multi-index is viewed.
  bool empty() const noexcept { return nMI_ == 0; }

  /**
   * @brief Read-only pointer to the flat AoS storage of the viewed
   * multi-indices: the entry of the @f$i@f$-th viewed multi-index in dimension
   * @f$d@f$ lives at @c data()[i * nDim() + d].
   */
  const T* data() const noexcept { return data_; }

  /**
   * @brief Read-only access to a single component of a viewed multi-index.
   * @param miIdx Index of the multi-index within the view (0-based).
   * @param dim   Dimension within that multi-index.
   * @return Value of the @p dim -th component of the @p miIdx -th multi-index.
   */
  T operator()(const size_t miIdx, const size_t dim) const noexcept {
    assert(miIdx < nMI_ && dim < nDim_);
    return data_[miIdx * nDim_ + dim];
  }

  /**
   * @brief Read-only view of the @p miIdx -th viewed multi-index.
   * @param miIdx Index of the multi-index within the view (0-based).
   * @return Non-owning view; no @c MI is created.
   */
  MIView<T> operator[](const size_t miIdx) const noexcept {
    assert(miIdx < nMI_);
    return MIView<T>(data_ + miIdx * nDim_, nDim_);
  }

  /// @brief Read-only view of the first viewed multi-index.
  MIView<T> front() const noexcept { return (*this)[0]; }
  /// @brief Read-only view of the last viewed multi-index.
  MIView<T> back() const noexcept { return (*this)[nMI_ - 1]; }

  /*******
  Subviews
  *******/
  /**
   * @brief View of the first @p count viewed multi-indices.
   * @param count Number of multi-indices of the new view; at most @ref nMI().
   */
  MIVecView first(const size_t count) const noexcept {
    assert(count <= nMI_);
    return MIVecView(data_, nDim_, count, offset_);
  }

  /**
   * @brief View of the last @p count viewed multi-indices.
   * @param count Number of multi-indices of the new view; at most @ref nMI().
   */
  MIVecView last(const size_t count) const noexcept {
    assert(count <= nMI_);
    return subview(nMI_ - count, count);
  }

  /**
   * @brief View of all viewed multi-indices from position @p offset on.
   * @param offset Index of the first multi-index of the new view; at most @ref nMI().
   */
  MIVecView subview(const size_t offset) const noexcept {
    assert(offset <= nMI_);
    return subview(offset, nMI_ - offset);
  }

  /**
   * @brief View of the @p count viewed multi-indices starting at position
   * @p offset.
   * @param offset Index of the first multi-index of the new view; at most @ref nMI().
   * @param count  Number of multi-indices of the new view; at most @c nMI()-offset.
   */
  MIVecView subview(const size_t offset, const size_t count) const noexcept {
    assert(offset <= nMI_ && count <= nMI_ - offset);
    return MIVecView(data_ + offset * nDim_, nDim_, count, offset_ + offset);
  }

  /*******
  Iterator
  *******/
  /// @brief Iterator to the first viewed multi-index.
  const_iterator begin() const noexcept {
    return const_iterator(storage(), nDim_, static_cast<difference_type>(offset_));
  }
  /// @brief Iterator past the last viewed multi-index.
  const_iterator end() const noexcept {
    return const_iterator(storage(), nDim_, static_cast<difference_type>(offset_ + nMI_));
  }
  /// @brief Iterator to the first viewed multi-index.
  const_iterator cbegin() const noexcept { return begin(); }
  /// @brief Iterator past the last viewed multi-index.
  const_iterator cend() const noexcept { return end(); }

  /// @brief Reverse iterator to the last viewed multi-index.
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  /// @brief Reverse iterator before the first viewed multi-index.
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
  /// @brief Reverse iterator to the last viewed multi-index.
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  /// @brief Reverse iterator before the first viewed multi-index.
  const_reverse_iterator crend() const noexcept { return rend(); }

 private:
  const T* data_ = nullptr;  ///< First entry of the first viewed multi-index.
  size_t nDim_ = 0;          ///< Dimensionality of every viewed multi-index.
  size_t nMI_ = 0;           ///< Number of viewed multi-indices.
  /**
   * @brief Position of the first viewed multi-index within the storage the
   * view was created from. Only the iterators need it (see @ref storage());
   * element access goes through @ref data_ directly.
   */
  size_t offset_ = 0;

  /// @brief Constructs the view of a sub-range; @p offset is relative to the underlying storage.
  MIVecView(const T* const data, const size_t nDim, const size_t nMI, const size_t offset) noexcept
      : data_(data), nDim_(nDim), nMI_(nMI), offset_(offset) {}

  /**
   * @brief Start of the storage the view was created from, i.e. the base
   * pointer of the iterators. Using it (instead of @ref data_) keeps the
   * index-based iterators of all views of one @c MIVec comparable with each
   * other and with the iterators of the @c MIVec itself.
   */
  const T* storage() const noexcept { return data_ - offset_ * nDim_; }
};

/*************************************************************
Compile-time checks of the view and range properties.
*************************************************************/
static_assert(std::is_trivially_copyable<MIVecView<unsigned int>>::value,
              "MIVecView is passed by value and must stay trivially copyable");

#if defined(__cpp_lib_ranges)
static_assert(std::ranges::view<MIVecView<unsigned int>>);
static_assert(std::ranges::borrowed_range<MIVecView<unsigned int>>);
static_assert(std::ranges::random_access_range<MIVecView<unsigned int>>);
static_assert(std::ranges::sized_range<MIVecView<unsigned int>>);
static_assert(std::ranges::common_range<MIVecView<unsigned int>>);
static_assert(std::same_as<std::ranges::range_reference_t<MIVecView<unsigned int>>,
                           MIView<unsigned int>>);
static_assert(
    std::same_as<std::ranges::range_value_t<MIVecView<unsigned int>>, MI<unsigned int>>);
#endif

}  // namespace combigrid
}  // namespace sgpp
