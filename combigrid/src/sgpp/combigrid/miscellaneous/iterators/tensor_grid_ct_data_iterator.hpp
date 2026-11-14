// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tensor_grid_ct_data_iterator.hpp
 * @brief Random-access iterator over the tensor grids of a sparse grid, yielding
 * @c TensorGridCTDataView proxies.
 */
#pragma once

#include <compare>
#include <cstddef>
#include <iterator>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Random-access proxy iterator over the tensor grids of a @c SparseGrid or
 * @c DeltasSparseGrid (see @c SparseGridBase::begin).
 *
 * Dereferencing returns a @c TensorGridCTDataView by value, which refers to the storage of the
 * grid: bind it by value or by const reference (<tt>for (const auto& tgData : grid)</tt>). The
 * iterator satisfies @c std::random_access_iterator (its reference type is the view itself, so it
 * is a C++17 input iterator only).
 */
class TensorGridCTDataIterator {
 public:
  using value_type = TensorGridCTDataView;                   ///< Element type.
  using reference = TensorGridCTDataView;                    ///< Proxy returned by operator*.
  using difference_type = std::ptrdiff_t;                    ///< Distance type.
  using iterator_concept = std::random_access_iterator_tag;  ///< C++20 iterator category.
  using iterator_category = std::input_iterator_tag;         ///< C++17 category (proxy reference).

  /// @brief Constructs a singular iterator.
  TensorGridCTDataIterator() noexcept = default;

  /**
   * @brief Constructs an iterator to the tensor grid @p idx.
   * @param lvls   Levels of all tensor grids (row-major, @c nDim per tensor grid).
   * @param coeffs Coefficients of all tensor grids.
   * @param table  Node table of the grid.
   * @param idx    Index of the tensor grid.
   */
  TensorGridCTDataIterator(const LvlType* lvls, const CTCoeffType* coeffs, const NodeTable* table,
                           const difference_type idx) noexcept
      : lvls_(lvls), coeffs_(coeffs), table_(table), idx_(idx) {}

  /// @brief Returns the view of the current tensor grid.
  reference operator*() const noexcept { return (*this)[0]; }

  /// @brief Member access through the view (<tt>it->coefficient</tt>): returns the view by value,
  /// whose @c operator-> points to itself for the rest of the expression.
  reference operator->() const noexcept { return **this; }

  /// @brief Returns the view of the tensor grid @p n positions further.
  reference operator[](const difference_type n) const noexcept {
    const size_t idx = static_cast<size_t>(idx_ + n);
    const LvlMIView mi(lvls_ + idx * table_->nDim(), table_->nDim());
    return TensorGridCTDataView{mi, coeffs_[idx], TensorGridView(mi, *table_)};
  }

  /// @name Increment, decrement, arithmetic
  /// @{
  TensorGridCTDataIterator& operator++() noexcept {
    ++idx_;
    return *this;
  }
  TensorGridCTDataIterator operator++(int) noexcept {
    TensorGridCTDataIterator copy = *this;
    ++idx_;
    return copy;
  }
  TensorGridCTDataIterator& operator--() noexcept {
    --idx_;
    return *this;
  }
  TensorGridCTDataIterator operator--(int) noexcept {
    TensorGridCTDataIterator copy = *this;
    --idx_;
    return copy;
  }
  TensorGridCTDataIterator& operator+=(const difference_type n) noexcept {
    idx_ += n;
    return *this;
  }
  TensorGridCTDataIterator& operator-=(const difference_type n) noexcept {
    idx_ -= n;
    return *this;
  }
  friend TensorGridCTDataIterator operator+(TensorGridCTDataIterator it,
                                            const difference_type n) noexcept {
    return it += n;
  }
  friend TensorGridCTDataIterator operator+(const difference_type n,
                                            TensorGridCTDataIterator it) noexcept {
    return it += n;
  }
  friend TensorGridCTDataIterator operator-(TensorGridCTDataIterator it,
                                            const difference_type n) noexcept {
    return it -= n;
  }
  friend difference_type operator-(const TensorGridCTDataIterator& a,
                                   const TensorGridCTDataIterator& b) noexcept {
    return a.idx_ - b.idx_;
  }
  /// @}

  /// @name Comparison (iterators of the same grid)
  /// @{
  friend bool operator==(const TensorGridCTDataIterator& a,
                         const TensorGridCTDataIterator& b) noexcept {
    return a.idx_ == b.idx_;
  }
  friend std::strong_ordering operator<=>(const TensorGridCTDataIterator& a,
                                          const TensorGridCTDataIterator& b) noexcept {
    return a.idx_ <=> b.idx_;
  }
  /// @}

  /// @brief Returns the index of the current tensor grid.
  size_t index() const noexcept { return static_cast<size_t>(idx_); }

 private:
  const LvlType* lvls_ = nullptr;        ///< Levels of all tensor grids.
  const CTCoeffType* coeffs_ = nullptr;  ///< Coefficients of all tensor grids.
  const NodeTable* table_ = nullptr;     ///< Node table of the grid.
  difference_type idx_ = 0;              ///< Index of the current tensor grid.
};

static_assert(std::random_access_iterator<TensorGridCTDataIterator>);

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
