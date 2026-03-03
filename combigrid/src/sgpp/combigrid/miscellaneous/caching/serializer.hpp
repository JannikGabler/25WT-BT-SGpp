#pragma once

#include <functional>
#include <istream>

namespace sgpp {
namespace combigrid {

template <typename T>
using CacheSerializer = std::function<void(const T&, std::ostream&)>;

template <typename T>
using CacheDeserializer = std::function<bool(T&, std::istream&)>;

}  // namespace combigrid
}  // namespace sgpp