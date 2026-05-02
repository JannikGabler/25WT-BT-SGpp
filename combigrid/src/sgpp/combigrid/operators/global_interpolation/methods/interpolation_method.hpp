#pragma once

#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class InterpolationMethod {
 public:
  InterpolationMethod(uint64_t id);

  virtual ~InterpolationMethod() = default;

  virtual double interpolate(double pos, const std::vector<double>& nodes,
                             const std::vector<double>& values) const = 0;

  uint64_t id() const;

 protected:
  bool hasTrivialResult(double pos, const std::vector<double>& nodes,
                        const std::vector<double>& values, double& out) const;

 private:
  const uint64_t id_;
};

}  // namespace combigrid
}  // namespace sgpp
