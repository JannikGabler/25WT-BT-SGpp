#include <cassert>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>

namespace sgpp {
namespace combigrid {

SourceFunc::SourceFunc(const std::function<double(const base::DataVector&)>& func)
    : func(func), cache() {}

SourceFunc::SourceFunc(std::function<double(const base::DataVector&)>&& func)
    : func(std::move(func)) {}

double SourceFunc::evaluate(const base::DataVector& point) const { return func(point); }

double SourceFunc::evaluateNormalizedInPlace(base::DataVector& point,
                                             const HyperCubeArea& area) const {
  assert(point.size() == area.size());

  // double result = 0;
  // if (constants::source_func::USE_CACHE) {
  //   if (cache.find(point, result)) {
  //     return result;
  //   }
  // }
  tools::denormalizeDataVector(point, area);

  return evaluate(point);
}

double SourceFunc::evaluateNormalizedOutOfPlace(const base::DataVector& point,
                                                const HyperCubeArea& area) const {
  assert(point.size() == area.size());

  return evaluate(tools::denormalizeDataVector(point, area));
}

}  // namespace combigrid

}  // namespace sgpp