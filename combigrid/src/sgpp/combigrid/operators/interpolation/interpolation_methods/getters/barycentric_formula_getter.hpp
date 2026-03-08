#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation_methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

InterpolationMethod* getBarycentricFormula(const NodeGenFunc* const nodeGenFunc);

}  // namespace combigrid
}  // namespace sgpp