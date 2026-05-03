/**
 * @file file_system.hpp
 * @brief Small file-system helpers used by the persistent cache.
 */
#pragma once

#include <sys/stat.h>
#include <string>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace caching {

/**
 * @brief Ensures that the directory at @p path exists.
 *
 * Creates the directory with mode @c 0755 if it does not already exist.
 * Linux-only: returns @c false unconditionally on Windows since the
 * persistent cache is documented to be Linux-only.
 *
 * @param path Filesystem path of the directory.
 * @return @c true if the directory exists (or was just created),
 *         @c false otherwise.
 */
inline bool ensureDirExists(const std::string& path) {
#ifdef _WIN32
  (void)path;
  return false;  // we only guarantee Linux per spec for persistent caches
#else
  struct stat st;
  if (stat(path.c_str(), &st) == 0) return S_ISDIR(st.st_mode);
  int r = mkdir(path.c_str(), 0755);
  return r == 0 || errno == EEXIST;
#endif
}

}  // namespace caching

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
