/**
 * @file os.hpp
 * @brief Compile-time host-OS detection helpers.
 */
#pragma once

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Returns @c true iff this translation unit was compiled on Linux.
 *
 * Used by features (e.g. the persistent cache, mmap) that are documented
 * to work only on Linux to query their availability at runtime without
 * preprocessor noise at the call site.
 */
inline bool isLinux() {
#ifdef __linux__
  return true;
#else
  return false;
#endif
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
