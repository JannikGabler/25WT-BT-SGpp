// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file optional_libraries.hpp
 * @brief The one place that detects the optional libraries of the @c combigrid module.
 *
 * Every file whose code depends on an optional library includes this header and tests the macro
 * it defines; no other file checks for the libraries. The detection runs in the preprocessor
 * (@c __has_include and the libraries' version macros), so the build system needs no
 * configuration for it.
 *
 * - Source-function cache (@c SourceFuncCache, @c CachedSourceFunc, @c CachedVectorSourceFunc):
 *   Boost.Unordered (@c boost::unordered_flat_set, Boost >= 1.81) and xxHash (XXH3,
 *   >= 0.8), both header-only. Defines @c SGPP_COMBIGRID_HAS_CACHE_LIBRARIES if both are found,
 *   unless @c SGPP_COMBIGRID_DISABLE_CACHE is defined (e.g. @c CPPFLAGS=-DSGPP_COMBIGRID_DISABLE_CACHE).
 *   The cache classes are compiled either way; without the libraries their constructors throw
 *   (@c SourceFuncCache::isAvailable()).
 *
 * @note Only for source files and internal headers: it includes @c xxhash.h, which the public
 * headers must not depend on.
 */
#pragma once

#if !defined(SGPP_COMBIGRID_DISABLE_CACHE) && \
    __has_include(<boost/unordered/unordered_flat_set.hpp>) && __has_include(<xxhash.h>)
// unordered_flat_set.hpp exists since Boost 1.81; XXH3 is stable since xxHash 0.8.0.
#include <xxhash.h>
#if XXH_VERSION_NUMBER >= 800
#define SGPP_COMBIGRID_HAS_CACHE_LIBRARIES 1
#endif
#endif

namespace sgpp {
namespace combigrid {

/**
 * @brief Whether the libraries of the source-function cache were found (compile-time constant
 * of @c SGPP_COMBIGRID_HAS_CACHE_LIBRARIES, for @c if @c constexpr).
 */
#ifdef SGPP_COMBIGRID_HAS_CACHE_LIBRARIES
inline constexpr bool HAS_CACHE_LIBRARIES = true;
#else
inline constexpr bool HAS_CACHE_LIBRARIES = false;
#endif

}  // namespace combigrid
}  // namespace sgpp
