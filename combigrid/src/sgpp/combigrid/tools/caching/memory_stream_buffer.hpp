/**
 * @file memory_stream_buffer.hpp
 * @brief A zero-copy @c std::streambuf adapter over an in-memory byte range.
 */
#pragma once

#include <streambuf>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Adapts a contiguous byte range as a read-only @c std::streambuf.
 *
 * Used to wrap memory-mapped persistent-cache pages so that they can be
 * fed into a @c std::istream without an intermediate copy.
 *
 * @note The buffer does @em not take ownership of the underlying memory;
 * the caller must keep it alive for the lifetime of the @c MemoryStreamBuf.
 */
class MemoryStreamBuf : public std::streambuf {
 public:
  /**
   * @brief Constructs the streambuf over the byte range @c [data, data+len).
   * @param data Pointer to the first byte of the range.
   * @param len  Number of bytes available.
   */
  MemoryStreamBuf(const char* data, size_t len) {
    char* p = const_cast<char*>(data);  // streambuf API verwendet non-const pointers
    setg(p, p, p + static_cast<std::ptrdiff_t>(len));
  }
};

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
