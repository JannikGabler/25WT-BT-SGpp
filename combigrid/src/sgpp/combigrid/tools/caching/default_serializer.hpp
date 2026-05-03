/**
 * @file default_serializer.hpp
 * @brief Default binary serialize / deserialize callbacks for the
 * persistent cache.
 */
#pragma once

#include <ostream>
#include <sgpp/combigrid/miscellaneous/caching/serializer.hpp>
#include <type_traits>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace caching {

/**
 * @brief Default binary @c CacheSerializer for trivially copyable types.
 *
 * Writes the raw bytes of the value to the output stream. Only available
 * for types satisfying @c std::is_trivially_copyable; non-trivial types
 * must supply a custom serializer.
 *
 * @tparam T Cached value type.
 */
template <typename T>
typename std::enable_if<std::is_trivially_copyable<T>::value, CacheSerializer<T>>::type
getDefaultCacheSerializer() {
  return
      [](const T& v, std::ostream& os) { os.write(reinterpret_cast<const char*>(&v), sizeof(T)); };
}

/**
 * @brief Default binary @c CacheDeserializer for trivially copyable types.
 *
 * Reads @c sizeof(T) raw bytes from the input stream into @c v.
 *
 * @tparam T Cached value type.
 * @return Deserializer that returns @c true on success.
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
