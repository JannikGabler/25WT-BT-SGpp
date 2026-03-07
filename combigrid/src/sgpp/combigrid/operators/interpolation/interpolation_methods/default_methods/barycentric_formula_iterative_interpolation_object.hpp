#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/iterative_interpolation_object.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace interpolation_methods {

class BarycentricFormulaIterativeInterpolationObj : public IterativeInterpolationObj {
 public:
  BarycentricFormulaIterativeInterpolationObj(double pos, const std::vector<double>& nodes);

  void addValue(double value) override;

  double getResult() override;

 private:
  const std::vector<double> weights;
  double curNumerator;
  double curDenominator;
};

}  // namespace interpolation_methods

}  // namespace combigrid
}  // namespace sgpp
