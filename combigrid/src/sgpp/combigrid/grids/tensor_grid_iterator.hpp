// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tensor_grid_iterator.hpp
 * @brief Input iterators that enumerate the grid points of a @c TensorGrid
 * together with their multi-indices, without materializing the grid.
 *
 * A tensor grid stores only its per-dimension node lists, so there is no
 * stored element an iterator could refer to. Both iterators therefore own one
 * @c TensorGridPoint, dereferencing returns a reference to it, and
 * incrementing overwrites it with the next grid point ("stashing" iterators).
 * The grid points are visited in the order of the linear grid-point index
 * (first dimension fastest), see
 * @c TensorGrid::getGridPoint(size_t, base::DataVector&) const.
 *
 * Common properties of both iterators:
 * - Constructing an iterator allocates the storage of its element once.
 *   Incrementing and dereferencing do not allocate.
 * - The end of the grid points is marked by @c std::default_sentinel; there
 *   are no past-the-end iterator objects. Range-based for loops, the
 *   @c std::ranges algorithms and views work directly. Algorithms that need a
 *   pair of iterators of the same type work on <tt>range | std::views::common</tt>.
 * - They are input iterators: single-pass algorithms only. References
 *   obtained by dereferencing are invalidated by incrementing, assigning to,
 *   moving or destroying the iterator.
 * - Copying an iterator copies its element (and allocates); the copy advances
 *   independently.
 * - The iterated tensor grid must outlive the iterator. Several iterators,
 *   also in different threads, can traverse the same tensor grid at once.
 *
 * Implementation note: the constructors of @c base::DataVector are not
 * inline. If one of them constructed the coordinates in place, it would
 * receive the address of a member of the iterator, and the compiler would
 * from then on have to assume that every store through a pointer and every
 * non-inlined call may modify the iterator. The coordinates are therefore
 * constructed as a temporary and moved in (the move constructor is inline).
 * Together with the sentinel, which avoids a second iterator object, this
 * lets the compiler keep the complete state of the iterator in registers
 * whenever the loop body is inlined.
 */
#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

/**
 * @brief One grid point of a tensor grid, as yielded by iterating a
 * @c TensorGrid.
 *
 * Supports structured bindings:
 * <tt>for (const auto& [mi, gp] : tensorGrid)</tt>.
 */
struct TensorGridPoint {
  GPMI mi;              ///< Per-dimension node indices of the grid point.
  base::DataVector gp;  ///< Coordinates of the grid point.
};

/**
 * @brief Read-only input iterator over the grid points of a @c TensorGrid.
 *
 * Incrementing updates only the entries of the element that differ from the
 * previous grid point. In all but one of @f$n_1@f$ steps (@f$n_1@f$ being the
 * number of nodes in the first dimension) this is a single node index and a
 * single coordinate.
 *
 * See the file description for the properties shared with
 * @c TensorGridMutableIterator.
 */
class TensorGridConstIterator {
 public:
  using value_type = TensorGridPoint;
  using reference = const TensorGridPoint&;  ///< Refers to the element owned by the iterator.
  using pointer = const TensorGridPoint*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;
  using iterator_concept = std::input_iterator_tag;

  /**
   * @brief Constructs an iterator to the first grid point (equal to
   * @c std::default_sentinel if the tensor grid has no grid points).
   *
   * @param nGPPerDim   Per-dimension grid-point counts of the tensor grid.
   * @param nodesPerDim Concatenated per-dimension node lists of the tensor grid.
   * @param nGP         Total number of grid points of the tensor grid.
   */
  TensorGridConstIterator(const GPMI& nGPPerDim, const base::DataVector& nodesPerDim,
                          const size_t nGP)
      : TensorGridConstIterator(nGPPerDim, nodesPerDim, nGP,
                                base::DataVector(nGPPerDim.size())) {}

  /// @brief Returns the current grid point; valid until the iterator is incremented.
  reference operator*() const noexcept {
    assert(remaining_ > 0);
    return current_;
  }
  /// @brief Member access to the current grid point.
  pointer operator->() const noexcept {
    assert(remaining_ > 0);
    return &current_;
  }

  /// @brief Advances to the next grid point.
  TensorGridConstIterator& operator++() noexcept {
    assert(remaining_ > 0);
    --remaining_;

    const GPCntType idx = idx0_ + 1;
    if (idx < nGP0_) {
      // By far the most frequent case: only the node index of dimension 0 changes.
      idx0_ = idx;
      current_.mi[0] = idx;
      current_.gp[0] = nodesPerDim_[idx];
    } else {
      carry();
    }
    return *this;
  }
  /// @brief Advances to the next grid point. Returns nothing, as the old element is gone.
  void operator++(int) noexcept { ++*this; }

  /// @brief Returns @c true iff @p it is past the last grid point.
  friend bool operator==(const TensorGridConstIterator& it, std::default_sentinel_t) noexcept {
    return it.remaining_ == 0;
  }

 private:
  const GPCntType* nGPPerDim_;  ///< Per-dimension grid-point counts of the tensor grid.
  const double* nodesPerDim_;   ///< Concatenated per-dimension node lists of the tensor grid.
  size_t nDim_;                 ///< Dimensionality of the tensor grid.
  size_t remaining_;            ///< Number of grid points not yet passed, including the current.

  /// @brief Copy of @c current_.mi[0]. Kept outside of the container so that the hot path of
  /// the increment does not have to read the index back from the heap.
  GPCntType idx0_;
  GPCntType nGP0_;  ///< Grid-point count of dimension 0 (@c 0 for a zero-dimensional grid).

  TensorGridPoint current_;  ///< The element; @c current_.mi doubles as the position.

  /// @brief Does the work of the public constructor. @p gp provides the storage of the
  /// coordinates (@c nDim entries); see the implementation note in the file description.
  TensorGridConstIterator(const GPMI& nGPPerDim, const base::DataVector& nodesPerDim,
                          const size_t nGP, base::DataVector gp)
      : nGPPerDim_(nGPPerDim.data()),
        nodesPerDim_(nodesPerDim.data()),
        nDim_(nGPPerDim.size()),
        remaining_(nGP),
        idx0_(0),
        nGP0_(nGPPerDim.empty() ? 0 : nGPPerDim[0]),
        current_{GPMI(nDim_), std::move(gp)} {
    assert(nGP == nGPPerDim.productofElems<size_t>());
    assert(nodesPerDim.size() == nGPPerDim.sumOfElems<size_t>());

    if (remaining_ > 0) {
      // First grid point: all node indices are 0 (set above).
      const double* nodes = nodesPerDim_;
      for (size_t dim = 0; dim < nDim_; dim++) {
        current_.gp[dim] = nodes[0];
        nodes += nGPPerDim_[dim];
      }
    }
  }

  /**
   * @brief Wraps dimension 0 around and carries over into the higher
   * dimensions, updating exactly the entries of the element that change.
   *
   * Incrementing past the last grid point wraps around to the first one.
   */
  void carry() noexcept {
    const GPCntType* const nGPPerDim = nGPPerDim_;
    const size_t nDim = nDim_;
    GPCntType* const mi = current_.mi.data();
    double* const gp = current_.gp.data();
    const double* nodes = nodesPerDim_;

    idx0_ = 0;
    mi[0] = 0;
    gp[0] = nodes[0];
    nodes += nGPPerDim[0];
    for (size_t dim = 1; dim < nDim; dim++) {
      const GPCntType nGP = nGPPerDim[dim];
      const GPCntType idx = mi[dim] + 1;
      if (idx < nGP) {
        mi[dim] = idx;
        gp[dim] = nodes[idx];
        return;
      }
      mi[dim] = 0;
      gp[dim] = nodes[0];
      nodes += nGP;
    }
  }
};

/**
 * @brief Input iterator over the grid points of a @c TensorGrid whose element
 * may be modified by the loop body.
 *
 * The element is a working copy: the loop body may assign any values to the
 * entries of the multi-index and of the coordinates, e.g. to transform the
 * coordinates in place. The tensor grid itself is never modified.
 *
 * The iterator assumes that every entry was altered. It keeps its position
 * in private storage and each increment rewrites @em all entries of the
 * multi-index and of the coordinates.
 *
 * @warning The loop body must not change the @em number of entries of
 * @c mi or @c gp (no resizing, clearing or moving from them); this is
 * checked by an assertion only. Exchanging a container for another one with
 * the same number of entries (e.g. by @c swap) is fine.
 *
 * See the file description for the properties shared with
 * @c TensorGridConstIterator.
 */
class TensorGridMutableIterator {
 public:
  using value_type = TensorGridPoint;
  using reference = TensorGridPoint&;  ///< Refers to the element owned by the iterator.
  using pointer = TensorGridPoint*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;
  using iterator_concept = std::input_iterator_tag;

  /**
   * @brief Constructs an iterator to the first grid point (equal to
   * @c std::default_sentinel if the tensor grid has no grid points).
   *
   * @param nGPPerDim   Per-dimension grid-point counts of the tensor grid.
   * @param nodesPerDim Concatenated per-dimension node lists of the tensor grid.
   * @param nGP         Total number of grid points of the tensor grid.
   */
  TensorGridMutableIterator(const GPMI& nGPPerDim, const base::DataVector& nodesPerDim,
                            const size_t nGP)
      : TensorGridMutableIterator(nGPPerDim, nodesPerDim, nGP,
                                  base::DataVector(nGPPerDim.size())) {}

  /// @brief Returns the working copy of the current grid point; valid until the iterator
  /// is incremented.
  reference operator*() const noexcept {
    assert(remaining_ > 0);
    return current_;
  }
  /// @brief Member access to the working copy of the current grid point.
  pointer operator->() const noexcept {
    assert(remaining_ > 0);
    return &current_;
  }

  /// @brief Advances to the next grid point and rewrites the whole element.
  TensorGridMutableIterator& operator++() noexcept {
    assert(remaining_ > 0);
    // The loop body may have altered every entry, but not the number of entries.
    assert(current_.mi.size() == nDim_ && current_.gp.size() == nDim_);
    --remaining_;

    const size_t nDim = nDim_;
    GPCntType idx0 = idx0_ + 1;
    if (idx0 >= nGP0_) {
      idx0 = 0;
      carry();
    }
    idx0_ = idx0;

    // Write all entries, regardless of what the element contains.
    const GPCntType* const nGPPerDim = nGPPerDim_;
    const GPCntType* const position = position_.data();
    GPCntType* const mi = current_.mi.data();
    double* const gp = current_.gp.data();
    const double* nodes = nodesPerDim_;

    mi[0] = idx0;
    gp[0] = nodes[idx0];
    nodes += nGPPerDim[0];
    for (size_t dim = 1; dim < nDim; dim++) {
      const GPCntType idx = position[dim];
      mi[dim] = idx;
      gp[dim] = nodes[idx];
      nodes += nGPPerDim[dim];
    }
    return *this;
  }
  /// @brief Advances to the next grid point. Returns nothing, as the old element is gone.
  void operator++(int) noexcept { ++*this; }

  /// @brief Returns @c true iff @p it is past the last grid point.
  friend bool operator==(const TensorGridMutableIterator& it, std::default_sentinel_t) noexcept {
    return it.remaining_ == 0;
  }

 private:
  const GPCntType* nGPPerDim_;  ///< Per-dimension grid-point counts of the tensor grid.
  const double* nodesPerDim_;   ///< Concatenated per-dimension node lists of the tensor grid.
  size_t nDim_;                 ///< Dimensionality of the tensor grid.
  size_t remaining_;            ///< Number of grid points not yet passed, including the current.

  GPCntType idx0_;  ///< Node index of the current grid point in dimension 0.
  GPCntType nGP0_;  ///< Grid-point count of dimension 0 (@c 0 for a zero-dimensional grid).
  /// @brief Node indices of the current grid point in the dimensions @c >=1 (entry 0 is
  /// unused, see @ref idx0_). Never handed out, so the loop body cannot alter the position.
  GPMI position_;

  /// @brief The element. @c mutable because dereferencing is a const operation that hands
  /// out a non-const reference to it.
  mutable TensorGridPoint current_;

  /// @brief Does the work of the public constructor. @p gp provides the storage of the
  /// coordinates (@c nDim entries); see the implementation note in the file description.
  TensorGridMutableIterator(const GPMI& nGPPerDim, const base::DataVector& nodesPerDim,
                            const size_t nGP, base::DataVector gp)
      : nGPPerDim_(nGPPerDim.data()),
        nodesPerDim_(nodesPerDim.data()),
        nDim_(nGPPerDim.size()),
        remaining_(nGP),
        idx0_(0),
        nGP0_(nGPPerDim.empty() ? 0 : nGPPerDim[0]),
        position_(nDim_),
        current_{GPMI(nDim_), std::move(gp)} {
    assert(nGP == nGPPerDim.productofElems<size_t>());
    assert(nodesPerDim.size() == nGPPerDim.sumOfElems<size_t>());

    if (remaining_ > 0) {
      // First grid point: all node indices are 0 (set above).
      const double* nodes = nodesPerDim_;
      for (size_t dim = 0; dim < nDim_; dim++) {
        current_.gp[dim] = nodes[0];
        nodes += nGPPerDim_[dim];
      }
    }
  }

  /// @brief Carries over into the dimensions @c >=1 after dimension 0 wrapped around.
  /// Incrementing past the last grid point wraps around to the first one.
  void carry() noexcept {
    const GPCntType* const nGPPerDim = nGPPerDim_;
    const size_t nDim = nDim_;
    GPCntType* const position = position_.data();

    for (size_t dim = 1; dim < nDim; dim++) {
      const GPCntType idx = position[dim] + 1;
      if (idx < nGPPerDim[dim]) {
        position[dim] = idx;
        return;
      }
      position[dim] = 0;
    }
  }
};

/**
 * @brief Range over the grid points of a tensor grid that yields modifiable
 * working copies; returned by @c TensorGrid::mutableGridPoints().
 *
 * The range and its iterators refer to the tensor grid, which must outlive
 * them. The iterators do not refer to the range object.
 */
class TensorGridMutableRange {
 public:
  using iterator = TensorGridMutableIterator;

  /**
   * @param nGPPerDim   Per-dimension grid-point counts of the tensor grid.
   * @param nodesPerDim Concatenated per-dimension node lists of the tensor grid.
   * @param nGP         Total number of grid points of the tensor grid.
   */
  TensorGridMutableRange(const GPMI& nGPPerDim, const base::DataVector& nodesPerDim,
                         const size_t nGP) noexcept
      : nGPPerDim_(&nGPPerDim), nodesPerDim_(&nodesPerDim), nGP_(nGP) {}

  /// @brief Returns an iterator to the first grid point. Allocates the element once.
  iterator begin() const { return iterator(*nGPPerDim_, *nodesPerDim_, nGP_); }
  /// @brief Returns the sentinel that marks the end of the grid points.
  std::default_sentinel_t end() const noexcept { return std::default_sentinel; }

 private:
  const GPMI* nGPPerDim_;                ///< Per-dimension grid-point counts.
  const base::DataVector* nodesPerDim_;  ///< Concatenated per-dimension node lists.
  size_t nGP_;                           ///< Total number of grid points.
};

/*************************************************************
Compile-time checks of the iterator properties, so that
regressions fail here instead of inside the standard library.
*************************************************************/
static_assert(std::input_iterator<TensorGridConstIterator>);
static_assert(std::input_iterator<TensorGridMutableIterator>);
static_assert(std::sentinel_for<std::default_sentinel_t, TensorGridConstIterator>);
static_assert(std::sentinel_for<std::default_sentinel_t, TensorGridMutableIterator>);
static_assert(std::same_as<std::iter_reference_t<TensorGridConstIterator>, const TensorGridPoint&>,
              "Iterating a tensor grid must not hand out write access by default");
static_assert(std::same_as<std::iter_reference_t<TensorGridMutableIterator>, TensorGridPoint&>,
              "The mutable iterator must hand out write access to its working copy");
// Stashing iterators cannot be forward iterators: a reference dies with the next increment.
static_assert(!std::forward_iterator<TensorGridConstIterator>);
static_assert(!std::forward_iterator<TensorGridMutableIterator>);

}  // namespace combigrid
}  // namespace sgpp

/// @brief The iterators of a @c TensorGridMutableRange stay valid when the range object is
/// destroyed, so algorithms may return them for temporary ranges.
template <>
inline constexpr bool
    std::ranges::enable_borrowed_range<sgpp::combigrid::TensorGridMutableRange> = true;
