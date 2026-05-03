/**
 * @file binomial.hpp
 * @brief Overflow-checked computation of binomial coefficients.
 */
#ifndef COMBIGRID_TOOLS_MATH_BINOMIAL_HPP
#define COMBIGRID_TOOLS_MATH_BINOMIAL_HPP

#include <limits>
#include <stdexcept>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Computes the binomial coefficient @f$\binom{n}{k}@f$.
 *
 * Uses the symmetry @f$\binom{n}{k} = \binom{n}{n-k}@f$ to minimize the
 * number of multiplications. Intermediate products are accumulated in
 * @c __uint128_t and the result is checked for overflow against
 * @c std::numeric_limits<T>::max().
 *
 * @tparam T Integral return / argument type.
 * @param n  Upper argument.
 * @param k  Lower argument.
 * @return @f$\binom{n}{k}@f$, or @c 0 if @p n or @p k is negative or
 *         @p k @c > @p n.
 * @throws std::overflow_error if the value does not fit into @c T.
 */
template <typename T>
T binomial(const T n, T k) {
  if (k < 0 || n < 0 || n < k) {
    return 0;
  } else if (k == 0 || n == k) {
    return 1;
  } else if (k > n - k) {
    k = n - k;
  }

  __uint128_t res = 1;

  for (T i = 1; i <= k; i++) {
    // Do not simplify to res *= (__uint128_t)(n - k + i) / (__uint128_t)i, because this changes the
    // semantics
    res = res * (__uint128_t)(n - k + i) / (__uint128_t)i;

    if (res > (__uint128_t)std::numeric_limits<T>::max()) {
      throw std::overflow_error("An overflow occured while computing a binomial!");
    }
  }
  return (T)res;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
