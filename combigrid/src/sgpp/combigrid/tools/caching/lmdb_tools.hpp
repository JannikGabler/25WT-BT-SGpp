// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#pragma once

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <string>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace lmdb {

/**
 * TODO: Document
 */
std::string lmdbMessage(const std::string& what, const std::string& path, int rc);

/**
 * TODO: Document
 */
size_t mapSize(size_t storageSpaceLimit, const std::string& directory, size_t existingBytes);

/**
 * TODO: Document
 */
std::string directoryOf(const std::string& path);

}  // namespace lmdb
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE