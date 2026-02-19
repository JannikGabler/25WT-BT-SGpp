#ifndef COMBIGRID_TOOLS_MULTIINDEX_UTILITIES_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_UTILITIES_HPP

#include <limits>
#include <stdexcept>

namespace sgpp {
namespace combigrid {
namespace tools {

template <typename T>
T binomial(const T n, T k) {
  if (k < 0 || n < 0 || n < k) {
    return 0;
  } else if (k == 0 || n == k) {
    return 1;
  } else if (k > n - k) {
    k = n - k;
  }

  __int128 res = 1;

  for (T i = 1; i <= k; i++) {
    res *= (__int128)(n - k + i) / (__int128)i;

    if (res > (__int128)std::numeric_limits<T>::max()) {
      throw std::overflow_error("An overflow occured while computing a binomial!");
    }
  }
  return (T)res;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif
