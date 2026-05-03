/**
 * @file ceil.hpp
 * @brief Integer ceiling division: @f$\lceil a/b \rceil@f$.
 */
#ifndef COMBIGRID_TOOLS_MATH_CEIL_HPP
#define COMBIGRID_TOOLS_MATH_CEIL_HPP

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Ceiling division for signed @c int.
 * @param a Numerator.
 * @param b Denominator (assumed non-zero).
 * @return @f$\lceil a/b \rceil@f$.
 */
inline int ceil(const int a, const int b) { return a / b + (a % b != 0); }

/**
 * @brief Ceiling division for @c unsigned @c int.
 * @param a Numerator.
 * @param b Denominator (assumed non-zero).
 * @return @f$\lceil a/b \rceil@f$.
 */
inline unsigned int ceil(const unsigned int a, const unsigned int b) {
  return a / b + (a % b != 0);
}

/**
 * @brief Ceiling division for @c unsigned @c long.
 * @param a Numerator.
 * @param b Denominator (assumed non-zero).
 * @return @f$\lceil a/b \rceil@f$.
 */
inline unsigned long ceil(const unsigned long a, const unsigned long b) {
  return a / b + (a % b != 0);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
