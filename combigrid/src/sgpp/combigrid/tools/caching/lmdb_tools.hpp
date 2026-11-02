// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file lmdb_tools.hpp
 * @brief Helpers of the disk store of the source-function cache (@c misc::LMDBPointValueStore):
 * messages, map size, directories (only with @c SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE).
 */
#pragma once

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <cstddef>
#include <string>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace lmdb {

/**
 * @brief Message of an LMDB error of the source-function cache.
 * @param what Failed action (e.g. "cannot open").
 * @param path File of the store.
 * @param rc   LMDB return code.
 * @return <tt>"SourceFuncCache: <what> '<path>': <mdb_strerror(rc)>"</tt>.
 */
std::string lmdbMessage(const std::string& what, const std::string& path, int rc);

/**
 * @brief Map size of a store (the maximal size of its file): the storage space limit, bounded
 * by the free space of the file system plus the size of the existing file, rounded down to whole
 * pages, and at least @c constants::source_func_cache::MIN_STORE_PAGES pages.
 * @param storageSpaceLimit Maximal size of the file in bytes.
 * @param directory         Directory of the file (for the free space; ignored if it cannot be
 *                          queried).
 * @param existingBytes     Size of the existing file (0 for a new one).
 * @return Map size in bytes.
 */
size_t mapSize(size_t storageSpaceLimit, const std::string& directory, size_t existingBytes);

/**
 * @brief Directory of a file path.
 * @param path Path of a file.
 * @return Its parent directory, or "." if it has none.
 */
std::string directoryOf(const std::string& path);

}  // namespace lmdb
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE