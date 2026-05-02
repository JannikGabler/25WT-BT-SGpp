#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class QuadRule {
 public:
  QuadRule(const uint64_t id) : id_(id) {}

  virtual ~QuadRule() = default;

  virtual base::DataVector getWeights(GPCntType nNodes) const = 0;

  virtual void genWeightsInplace(GPCntType nNodes, base::DataVector& out,
                                 size_t startIdx = 0) const = 0;

  uint64_t id() const { return id_; }

 private:
  const uint64_t id_;
};

}  // namespace combigrid
}  // namespace sgpp
