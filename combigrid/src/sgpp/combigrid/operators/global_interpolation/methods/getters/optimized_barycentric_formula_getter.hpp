#pragma once

#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

InterpolationMethod* getOptBarycentricFormula();

}
}  // namespace sgpp