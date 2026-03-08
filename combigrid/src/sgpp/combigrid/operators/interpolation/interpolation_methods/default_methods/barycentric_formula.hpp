#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace interpolation {

/*
Barycentric formula of the second kind for all kind of nodes.
*/
class BarycentricFormula : public InterpolationMethod {
 public:
  BarycentricFormula(const NodeGenFunc* nodeGenFunc);

  double interpolate(const double pos, const std::vector<double>& nodes,
                     const std::vector<double>& values) const override;

 private:
  // TODO: Optimize (caching) (double long could be used in pre-computation)
  double evaluateFormula(const double pos, const std::vector<double>& nodes,
                         const std::vector<double>& values) const;
};

}  // namespace interpolation

}  // namespace combigrid
}  // namespace sgpp