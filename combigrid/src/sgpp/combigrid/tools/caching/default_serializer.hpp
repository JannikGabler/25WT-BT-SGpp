#pragma once

#include <ostream>
#include <sgpp/combigrid/miscellaneous/caching/serializer.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace caching {

/*
Default binary serializer for trivially copyable types
*/
template <typename T>
typename std::enable_if<std::is_trivially_copyable<T>::value, CacheSerializer<T>>::type
getDefaultCacheSerializer() {
  return
      [](const T& v, std::ostream& os) { os.write(reinterpret_cast<const char*>(&v), sizeof(T)); };
}

/*
Default binary deserializer for trivially copyable types
*/
template <typename T>
typename std::enable_if<std::is_trivially_copyable<T>::value, CacheDeserializer<T>>::type
getDefaultCacheDeserializer() {
  return [](T& v, std::istream& is) -> bool {
    is.read(reinterpret_cast<char*>(&v), sizeof(T));
    return (bool)is;
  };
}

}  // namespace caching

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
