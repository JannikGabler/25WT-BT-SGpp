#pragma once

#include <streambuf>

namespace sgpp {
namespace combigrid {
namespace tools {

// Memory stream buffer for zero-copy istream
class MemoryStreamBuf : public std::streambuf {
 public:
  MemoryStreamBuf(const char* data, size_t len) {
    char* p = const_cast<char*>(data);  // streambuf API verwendet non-const pointers
    setg(p, p, p + static_cast<std::ptrdiff_t>(len));
  }
};

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp