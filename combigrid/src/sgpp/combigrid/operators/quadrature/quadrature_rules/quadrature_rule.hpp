#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>

namespace sgpp {
namespace combigrid {

class QuadRule {
 public:
  QuadRule(const uint64_t id) : id_(id) {}

  virtual ~QuadRule() = default;

  virtual base::DataVector getWeights(size_t nNodes) const = 0;

  uint64_t id() const { return id_; }

 private:
  const uint64_t id_;
};

}  // namespace combigrid
}  // namespace sgpp
