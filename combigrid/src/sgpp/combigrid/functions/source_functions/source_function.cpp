#include <cassert>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include "sgpp/combigrid/constants.hpp"

namespace sgpp {
namespace combigrid {

SourceFunc::SourceFunc(const std::function<double(const base::DataVector&)>& func)
    : func(func), cache() {}

SourceFunc::SourceFunc(std::function<double(const base::DataVector&)>&& func)
    : func(std::move(func)) {}

double SourceFunc::evaluate(const base::DataVector& point) const { return func(point); }

double SourceFunc::evaluateNormalized(base::DataVector point, const HyperCubeArea& area) const {
  assert(point.size() == area.size());

  // double result = 0;
  // if (constants::source_func::USE_CACHE) {
  //   if (cache.find(point, result)) {
  //     return result;
  //   }
  // }

  for (size_t dim = 0; dim < point.size(); dim++) {  // Transform on to the area
    assert(point[dim] >= 0 && point[dim] <= 1);

    point[dim] *= area[dim].second - area[dim].first;
    point[dim] += area[dim].first;
  }

  return evaluate(point);
}

}  // namespace combigrid

}  // namespace sgpp