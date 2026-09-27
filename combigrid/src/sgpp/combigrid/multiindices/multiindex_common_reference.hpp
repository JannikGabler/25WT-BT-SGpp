// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_common_reference.hpp
 * @brief Specializations of @c std::basic_common_reference for the
 * multi-index types @c MI, @c MIView and @c MIRef (C++20 and later).
 *
 * They make the @c common_reference of any two different multi-index types
 * the non-owning @c MIView (instead of an allocating @c MI), as required for
 * the proxy iterators of @c MIVec to model @c std::indirectly_readable.
 *
 * Included by @c multiindex_view.hpp, so that the specializations are
 * visible wherever a view type is: they must be declared before the first
 * use of @c std::common_reference with these types.
 */
#pragma once

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIView;  ///< Forward declaration to avoid circular includes.
template <typename T>
class MIRef;  ///< Forward declaration to avoid circular includes.

}  // namespace combigrid
}  // namespace sgpp

#if __cplusplus >= 202002L
namespace std {

template <typename T, template <class> class TQual, template <class> class UQual>
struct basic_common_reference<sgpp::combigrid::MIRef<T>, sgpp::combigrid::MI<T>, TQual, UQual> {
  using type = sgpp::combigrid::MIView<T>;
};
template <typename T, template <class> class TQual, template <class> class UQual>
struct basic_common_reference<sgpp::combigrid::MI<T>, sgpp::combigrid::MIRef<T>, TQual, UQual> {
  using type = sgpp::combigrid::MIView<T>;
};
template <typename T, template <class> class TQual, template <class> class UQual>
struct basic_common_reference<sgpp::combigrid::MIView<T>, sgpp::combigrid::MI<T>, TQual, UQual> {
  using type = sgpp::combigrid::MIView<T>;
};
template <typename T, template <class> class TQual, template <class> class UQual>
struct basic_common_reference<sgpp::combigrid::MI<T>, sgpp::combigrid::MIView<T>, TQual, UQual> {
  using type = sgpp::combigrid::MIView<T>;
};
template <typename T, template <class> class TQual, template <class> class UQual>
struct basic_common_reference<sgpp::combigrid::MIRef<T>, sgpp::combigrid::MIView<T>, TQual,
                              UQual> {
  using type = sgpp::combigrid::MIView<T>;
};
template <typename T, template <class> class TQual, template <class> class UQual>
struct basic_common_reference<sgpp::combigrid::MIView<T>, sgpp::combigrid::MIRef<T>, TQual,
                              UQual> {
  using type = sgpp::combigrid::MIView<T>;
};

}  // namespace std
#endif
