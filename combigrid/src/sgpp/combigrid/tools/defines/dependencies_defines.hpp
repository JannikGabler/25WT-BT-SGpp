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
 *
 * Disk store of the storage policies of the source-function cache
 * (@c misc::LMDBPointValueStore, @c CachePolicy::TemporaryStorage / @c PersistentStorage): LMDB
 * (>= 0.9.14, not header-only) and POSIX file I/O. SCons links LMDB if it finds the library and
 * its header (site_scons/SGppConfigure.py, @c detectLMDB) and then defines
 * @c SGPP_COMBIGRID_LMDB_LINKED.
 * - @c SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE: defined if LMDB is linked, @c lmdb.h and
 *   the POSIX headers are found, unless @c SGPP_COMBIGRID_DISABLE_LMDB_STORE is defined;
 * - @c SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE_EXCEPTION: otherwise, the reason.
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

#ifndef SGPP_COMBIGRID_DISABLE_LMDB_STORE
#ifdef SGPP_COMBIGRID_LMDB_LINKED
#if __has_include(<lmdb.h>) && __has_include(<unistd.h>) && __has_include(<sys/file.h>)
#include <lmdb.h>
#if MDB_VERSION_FULL >= MDB_VERINT(0, 9, 14)
#define SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE 1
#else
#define SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE_EXCEPTION \
  "Library LMDB doesn't meet minimum version requirements of 0.9.14."
#endif
#else
#define SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE_EXCEPTION \
  "Include <lmdb.h> or the POSIX file headers are not available."
#endif
#else
#define SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE_EXCEPTION \
  "Library LMDB was not found when SG++ was configured."
#endif
#else
#define SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE_EXCEPTION \
  "Flag 'SGPP_COMBIGRID_DISABLE_LMDB_STORE' is set."
#endif
