#pragma once

#include <typeindex>

namespace sgpp {
namespace combigrid {
namespace misc {

struct BaseCache {
  virtual ~BaseCache() {}
  virtual std::type_index key_type() const = 0;
  virtual std::type_index value_type() const = 0;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
