// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file dependencies_defines.hpp
 * @brief The one place that detects the optional libraries of the @c combigrid module.
 *
 * Every file whose code depends on an optional library includes this header and tests the macro
 * it defines; no other file checks for the libraries. The detection runs in the preprocessor
 * (@c __has_include and the libraries' version macros), so the build system needs no
 * configuration for it. Only for source files and internal headers: it includes @c xxhash.h.
 *
 * Point table of the source-function cache (@c misc::BoostPointValueTable): Boost.Unordered
 * (@c boost::unordered_flat_set, Boost >= 1.81) and xxHash (XXH3, >= 0.8), both header-only.
 * - @c SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE: defined if both are found, unless
 *   @c SGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE is defined (e.g.
 *   @c CPPFLAGS=-DSGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE);
 * - @c SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION: otherwise, the reason (a string
 *   literal for the message of the @c base::dependency_exception thrown when a cache is created).
 */
#pragma once

// Preprocessor only: no namespace (xxhash.h must not be included inside one).
#ifndef SGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE
#if __has_include(<boost/unordered/unordered_flat_set.hpp>)
#if __has_include(<xxhash.h>)
// unordered_flat_set.hpp exists since Boost 1.81; XXH3 is stable since xxHash 0.8.0.
#include <xxhash.h>
#if XXH_VERSION_NUMBER >= 800
#define SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE 1
#else
#define SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION \
  "Library xxHash doesn't meet minimum version requirements of 0.8.0."
#endif
#else
#define SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION "Library xxHash is not available."
#endif
#else
#define SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION \
  "Include <boost/unordered/unordered_flat_set.hpp> is not available."
#endif
#else
#define SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE_EXCEPTION \
  "Flag 'SGPP_COMBIGRID_DISABLE_BOOST_POINT_TABLE' is set."
#endif
