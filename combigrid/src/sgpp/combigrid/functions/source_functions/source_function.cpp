#include <cassert>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>

namespace sgpp {
namespace combigrid {

SourceFunc::SourceFunc(const std::function<double(const base::DataVector&)>& func) : func(func) {}

SourceFunc::SourceFunc(std::function<double(const base::DataVector&)>&& func)
    : func(std::move(func)) {}

double SourceFunc::evaluate(const base::DataVector& point) const { return func(point); }

double SourceFunc::evaluateNormalizedInPlace(base::DataVector& point,
                                             const HyperCubeArea& area) const {
  assert(point.size() == area.size());

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