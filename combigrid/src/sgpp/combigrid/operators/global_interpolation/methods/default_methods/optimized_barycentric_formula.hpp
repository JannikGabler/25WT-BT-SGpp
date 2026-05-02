#pragma once

#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

namespace global_interpolation {

/*
Barycentric formula of the second kind optimized for Clenshaw Curtis nodes.
*/
class OptBarycentricFormula : public InterpolationMethod {
 public:
  OptBarycentricFormula();

  double interpolate(const double pos, const std::vector<double>& nodes,
                     const std::vector<double>& values) const override;

 private:
  double barycentricFormula(const double pos, const std::vector<double>& nodes,
                            const std::vector<double>& values) const;
};

}  // namespace global_interpolation

}  // namespace combigrid
}  // namespace sgpp