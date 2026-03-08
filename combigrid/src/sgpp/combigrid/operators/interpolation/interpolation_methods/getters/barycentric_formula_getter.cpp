#include <memory>
#include <mutex>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>
#include <unordered_map>
#include <utility>

namespace sgpp {
namespace combigrid {

InterpolationMethod* getBarycentricFormula(const NodeGenFunc* const nodeGenFunc) {
  static std::mutex m;
  static std::unordered_map<const NodeGenFunc*, std::unique_ptr<InterpolationMethod>> instances;

  {
    std::lock_guard<std::mutex> lock(m);
    const auto it = instances.find(nodeGenFunc);

    if (it != instances.end()) {
      return it->second.get();
    }

    std::unique_ptr<InterpolationMethod> instance =
        std::unique_ptr<InterpolationMethod>(new interpolation::BarycentricFormula(nodeGenFunc));
    InterpolationMethod* result = instance.get();

    instances.emplace(nodeGenFunc, std::move(instance));

    return result;
  }
}

}  // namespace combigrid
}  // namespace sgpp