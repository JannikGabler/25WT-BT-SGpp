#pragma once

#include <sys/stat.h>
#include <string>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace caching {

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
