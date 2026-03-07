#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class IterativeInterpolationObj {
 public:
  IterativeInterpolationObj(double pos, const std::vector<double>& nodes);

  virtual ~IterativeInterpolationObj() = default;

  virtual void addValue(double value) = 0;

  virtual double getResult() = 0;

 protected:
  const double pos;
  const std::vector<double>& nodes;
  size_t curIdx;
};

}  // namespace combigrid
}  // namespace sgpp
