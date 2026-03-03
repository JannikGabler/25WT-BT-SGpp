#pragma once

namespace sgpp {
namespace combigrid {
namespace tools {

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