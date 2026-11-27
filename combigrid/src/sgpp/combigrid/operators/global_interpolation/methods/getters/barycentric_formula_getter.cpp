#include <memory>
#include <mutex>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <unordered_map>
#include <utility>

namespace sgpp {
namespace combigrid {

InterpolationMethod* getBarycentricFormula(const NodeFamily* const nodeFamily) {
  static std::mutex m;
  static std::unordered_map<const NodeFamily*, std::unique_ptr<InterpolationMethod>> instances;

  {
    std::lock_guard<std::mutex> lock(m);
    const auto it = instances.find(nodeFamily);

    if (it != instances.end()) {
      return it->second.get();
    }

    std::unique_ptr<InterpolationMethod> instance = std::unique_ptr<InterpolationMethod>(
        new global_interpolation::BarycentricFormula(nodeFamily));
    InterpolationMethod* result = instance.get();

    instances.emplace(nodeFamily, std::move(instance));

    return result;
  }
}

}  // namespace combigrid
}  // namespace sgpp