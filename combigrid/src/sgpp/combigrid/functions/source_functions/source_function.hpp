#pragma once

#include <functional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class SourceFunc {
 public:
  SourceFunc(const std::function<double(const base::DataVector&)>& func);

  SourceFunc(std::function<double(const base::DataVector&)>&& func);

  /*
  This operation does not use caching!
  */
  double evaluate(const base::DataVector& point) const;

  /*
  Evaluates the functions by transforming the given normalized point on to the area.
  Warning: This changed the given point in place in order to avoid memory allocations!
  */
  double evaluateNormalizedInPlace(base::DataVector& point, const HyperCubeArea& area) const;

 private:
  std::function<double(const base::DataVector&)> func;
  mutable misc::SourceFunctionCache cache;  // Caches normalized points
};

}  // namespace combigrid
}  // namespace sgpp