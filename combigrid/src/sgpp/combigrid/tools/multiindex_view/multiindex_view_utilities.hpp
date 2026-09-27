// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_view_utilities.hpp
 * @brief Helpers of the multi-index views @c MIView and @c MIRef: compile-time
 * classification of the multi-index types and the element-wise comparisons
 * behind the view comparison operators.
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <stdexcept>
#include <type_traits>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIView;  ///< Forward declaration to avoid circular includes.
template <typename T>
class MIRef;  ///< Forward declaration to avoid circular includes.

namespace tools {

/**
 * @brief Implementation details of the multi-index views.
 */
namespace mi_view {

/**
 * @brief Compile-time classification of the multi-index types.
 *
 * The primary template covers all types that are not multi-indices;
 * specializations exist for @c MI, @c MIView and @c MIRef.
 *
 * @tparam X Type to classify.
 */
template <typename X>
struct MITypeTraits {
  using IsMultiIndex = std::false_type;  ///< @c MI, @c MIView or @c MIRef.
  using IsView = std::false_type;        ///< @c MIView or @c MIRef.
  using ElementType = void;              ///< Type of the entries.
};

/// @brief Owning multi-index.
template <typename T>
struct MITypeTraits<MI<T>> {
  using IsMultiIndex = std::true_type;
  using IsView = std::false_type;
  using ElementType = T;
};

/// @brief Read-only view.
template <typename T>
struct MITypeTraits<MIView<T>> {
  using IsMultiIndex = std::true_type;
  using IsView = std::true_type;
  using ElementType = T;
};

/// @brief Mutable proxy reference.
template <typename T>
struct MITypeTraits<MIRef<T>> {
  using IsMultiIndex = std::true_type;
  using IsView = std::true_type;
  using ElementType = T;
};

/**
 * @brief Returns @c true iff @p A and @p B are multi-index types (@c MI,
 * @c MIView or @c MIRef) with the same element type.
 */
template <typename A, typename B>
constexpr bool areComparableMultiIndices() {
  return MITypeTraits<A>::IsMultiIndex::value && MITypeTraits<B>::IsMultiIndex::value &&
         std::is_same<typename MITypeTraits<A>::ElementType,
                      typename MITypeTraits<B>::ElementType>::value;
}

/**
 * @brief Returns @c true iff @p A and @p B are comparable multi-index types
 * and at least one of them is a view.
 *
 * Selects the comparison operators of the views; @c MI with @c MI is left to
 * the operators of @c MI.
 */
template <typename A, typename B>
constexpr bool areViewComparable() {
  return areComparableMultiIndices<A, B>() &&
         (MITypeTraits<A>::IsView::value || MITypeTraits<B>::IsView::value);
}

/**
 * @brief Throws @c std::logic_error if two multi-indices have different sizes.
 *
 * Same exception type and message as the comparison operators of @c MI.
 */
inline void checkSameSize(const size_t sizeA, const size_t sizeB) {
  if (sizeA != sizeB) {
    throw std::logic_error("Sizes of MI do not match!");
  }
}

/// @brief Returns @c true iff @p a and @p b agree in every dimension.
template <typename T>
bool equal(const MIView<T> a, const MIView<T> b) {
  checkSameSize(a.size(), b.size());
  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a.data()[dim] != b.data()[dim]) return false;
  }
  return true;
}

/// @brief Returns @c true iff @p a is strictly less than @p b in every dimension.
template <typename T>
bool lessInEveryDim(const MIView<T> a, const MIView<T> b) {
  checkSameSize(a.size(), b.size());
  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a.data()[dim] >= b.data()[dim]) return false;
  }
  return true;
}

/// @brief Returns @c true iff @p a is less than or equal to @p b in every dimension.
template <typename T>
bool lessEqualInEveryDim(const MIView<T> a, const MIView<T> b) {
  checkSameSize(a.size(), b.size());
  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a.data()[dim] > b.data()[dim]) return false;
  }
  return true;
}

}  // namespace mi_view
}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
