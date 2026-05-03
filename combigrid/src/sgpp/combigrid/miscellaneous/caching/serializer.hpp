/**
 * @file serializer.hpp
 * @brief Type aliases for the serialize / deserialize callbacks used by
 * the cache infrastructure.
 */
// #pragma once

// #include <functional>
// #include <istream>

// namespace sgpp {
// namespace combigrid {

// /**
//  * @brief Callable that writes a @c T value to an output stream.
//  * @tparam T Cached value type.
//  */
// template <typename T>
// using CacheSerializer = std::function<void(const T&, std::ostream&)>;

// /**
//  * @brief Callable that reads a @c T value from an input stream and
//  * returns @c true on success.
//  * @tparam T Cached value type.
//  */
// template <typename T>
// using CacheDeserializer = std::function<bool(T&, std::istream&)>;

// }  // namespace combigrid
// }  // namespace sgpp
