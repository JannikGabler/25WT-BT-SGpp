#ifndef COMBIGRID_TOOLS_MATH_POWER_HPP
#define COMBIGRID_TOOLS_MATH_POWER_HPP

namespace sgpp {
namespace combigrid {
namespace tools {

int pow(int base, int exponent) {
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

unsigned int pow(unsigned int base, unsigned int exponent) {
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

unsigned long pow(unsigned long base, unsigned long exponent) {
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

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
