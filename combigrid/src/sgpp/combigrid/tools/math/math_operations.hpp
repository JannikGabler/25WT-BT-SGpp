// #ifndef COMBIGRID_TOOLS_MATH_OPERATIONS_HPP
// #define COMBIGRID_TOOLS_MATH_OPERATIONS_HPP

// #include <limits>
// #include <stdexcept>

// namespace sgpp {
// namespace combigrid {
// namespace tools {

// template <typename T>
// T binomial(const T n, T k) {
//   if (k < 0 || n < 0 || n < k) {
//     return 0;
//   } else if (k == 0 || n == k) {
//     return 1;
//   } else if (k > n - k) {
//     k = n - k;
//   }

//   __uint128_t res = 1;

//   for (T i = 1; i <= k; i++) {
//     // Do not simplify to res *= (__uint128_t)(n - k + i) / (__uint128_t)i, because this changes
//     the
//     // semantics
//     res = res * (__uint128_t)(n - k + i) / (__uint128_t)i;

//     if (res > (__uint128_t)std::numeric_limits<T>::max()) {
//       throw std::overflow_error("An overflow occured while computing a binomial!");
//     }
//   }
//   return (T)res;
// }

// int ceil(const int a, const int b);

// unsigned int ceil(const unsigned int a, const unsigned int b);

// unsigned long ceil(const unsigned long a, const unsigned long b);

// }  // namespace tools
// }  // namespace combigrid
// }  // namespace sgpp

// #endif
