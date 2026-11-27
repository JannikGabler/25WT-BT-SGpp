#include <sgpp/combigrid/operators/global_interpolation/methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

InterpolationMethod* getBarycentricFormula() {
  static global_interpolation::BarycentricFormula instance;
  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp