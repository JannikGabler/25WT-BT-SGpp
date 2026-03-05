#ifndef COMBIGRID_TOOLS_MATH_POWER_HPP
#define COMBIGRID_TOOLS_MATH_POWER_HPP

#include <type_traits>
namespace sgpp {
namespace combigrid {
namespace tools {

inline int pow(int base, int exponent) {
  int result = 1;

  while (exponent > 0) {
    if (exponent & 1) {
      result *= base;
    }

    base *= base;
    exponent >>= 1;
  }

  return result;
}

inline unsigned int pow(unsigned int base, unsigned int exponent) {
  unsigned int result = 1;

  while (exponent > 0) {
    if (exponent & 1) {
      result *= base;
    }

    base *= base;
    exponent >>= 1;
  }

  return result;
}

inline unsigned long pow(unsigned long base, unsigned long exponent) {
  unsigned long result = 1;

  while (exponent > 0) {
    if (exponent & 1) {
      result *= base;
    }

    base *= base;
    exponent >>= 1;
  }

  return result;
}

/*
Does not include 0.
*/
template <typename T>
inline typename std::enable_if<std::is_integral<T>::value, bool>::type isPowerOfTwo(const T n) {
  return n && ((n & (n - 1)) == 0);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
