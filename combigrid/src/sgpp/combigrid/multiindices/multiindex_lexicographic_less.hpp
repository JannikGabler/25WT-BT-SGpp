// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_lexicographic_less.hpp
 * @brief Defines the lexicographic multi-index comparator @c MILexLess.
 */
#pragma once

#include <algorithm>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <sgpp/combigrid/tools/multiindex_view/multiindex_view_utilities.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {

/**
 * @brief Lexicographic strict weak ordering on multi-indices.
 *
 * The comparison operators of @c MI (and of the views) implement the
 * component-wise partial order, which is not a strict weak ordering. Ordering
 * algorithms (@c std::sort, @c std::lower_bound, @c std::binary_search,
 * @c std::set, ...) must therefore be given this comparator instead.
 *
 * Accepts any combination of @c MI, @c MIView and @c MIRef with the same
 * element type (transparent). Multi-indices of different sizes are compared
 * like @c std::lexicographical_compare does (a proper prefix is smaller).
 */
struct MILexLess {
  using is_transparent = void;  ///< Enables heterogeneous lookup in ordered containers.

  /// @brief Returns @c true iff @p a lexicographically precedes @p b.
  template <typename A, typename B,
            typename std::enable_if<tools::mi_view::areComparableMultiIndices<A, B>(),
                                    int>::type = 0>
  bool operator()(const A& a, const B& b) const noexcept {
    const MIView<typename A::value_type> viewA(a);
    const MIView<typename A::value_type> viewB(b);
    return std::lexicographical_compare(viewA.begin(), viewA.end(), viewB.begin(), viewB.end());
  }
};

}  // namespace combigrid
}  // namespace sgpp
