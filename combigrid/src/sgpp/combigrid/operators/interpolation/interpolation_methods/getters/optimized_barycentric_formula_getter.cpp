#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/default_methods/optimized_barycentric_formula.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

InterpolationMethod* getOptBarycentricFormula() {
  static interpolation::OptBarycentricFormula instance;
  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp