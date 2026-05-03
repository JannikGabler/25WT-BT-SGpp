/**
 * @file power.hpp
 * @brief Integer exponentiation by squaring and a power-of-two predicate.
 */
#ifndef COMBIGRID_TOOLS_MATH_POWER_HPP
#define COMBIGRID_TOOLS_MATH_POWER_HPP

#include <type_traits>
namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Integer exponentiation by squaring (signed @c int overload).
 * @param base     Base.
 * @param exponent Non-negative exponent.
 * @return @f$\mathrm{base}^{\mathrm{exponent}}@f$.
 */
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

/// @brief Integer exponentiation by squaring (@c unsigned @c int overload).
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

/// @brief Integer exponentiation by squaring (@c unsigned @c long overload).
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

/**
 * @brief Returns @c true iff @p n is a positive power of two.
 *
 * @c 0 is intentionally not considered a power of two by this predicate.
 *
 * @tparam T Integral type.
 * @param n  Value to test.
 * @return @c true iff @p n is one of @c 1,2,4,8,...
 */
template <typename T>
inline typename std::enable_if<std::is_integral<T>::value, bool>::type isPowerOfTwo(const T n) {
  return n && ((n & (n - 1)) == 0);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
