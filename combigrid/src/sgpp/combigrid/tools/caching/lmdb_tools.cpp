// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include <sgpp/combigrid/tools/caching/lmdb_tools.hpp>
#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <lmdb.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <sgpp/combigrid/constants.hpp>
#include <string>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace lmdb {

std::string lmdbMessage(const std::string& what, const std::string& path, const int rc) {
  return "SourceFuncCache: " + what + " '" + path + "': " + mdb_strerror(rc);
}

size_t mapSize(const size_t storageSpaceLimit, const std::string& directory,
               const size_t existingBytes) {
  const size_t pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
  size_t size = storageSpaceLimit;
  struct statvfs fs;
  if (statvfs(directory.c_str(), &fs) == 0) {
    const size_t available = static_cast<size_t>(fs.f_bavail) * static_cast<size_t>(fs.f_frsize);
    size = std::min(size, existingBytes + available);
  }
  size -= size % pageSize;
  return std::max(size, constants::source_func_cache::MIN_STORE_PAGES * pageSize);
}

std::string directoryOf(const std::string& path) {
  const std::filesystem::path parent = std::filesystem::path(path).parent_path();
  return parent.empty() ? std::string(".") : parent.string();
}

}  // namespace lmdb
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE