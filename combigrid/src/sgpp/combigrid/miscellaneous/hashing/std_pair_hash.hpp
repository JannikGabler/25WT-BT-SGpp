#ifndef COMBIGRID_MISC_STD_PAIR_HASH_HPP
#define COMBIGRID_MISC_STD_PAIR_HASH_HPP

#include <cstddef>
#include <functional>
#include <utility>

namespace sgpp {
namespace combigrid {
namespace misc {

template <class T1, class T2>
struct PairHash {
  std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
    std::size_t h1 = std::hash<T1>{}(p.first);
    std::size_t h2 = std::hash<T2>{}(p.second);

    return h1 * 1315423911u + h2;
  }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif