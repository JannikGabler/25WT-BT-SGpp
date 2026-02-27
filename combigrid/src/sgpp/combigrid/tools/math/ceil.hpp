#ifndef COMBIGRID_TOOLS_MATH_CEIL_HPP
#define COMBIGRID_TOOLS_MATH_CEIL_HPP

namespace sgpp {
namespace combigrid {
namespace tools {

inline int ceil(const int a, const int b) { return a / b + (a % b != 0); }

inline unsigned int ceil(const unsigned int a, const unsigned int b) {
  return a / b + (a % b != 0);
}

inline unsigned long ceil(const unsigned long a, const unsigned long b) {
  return a / b + (a % b != 0);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif