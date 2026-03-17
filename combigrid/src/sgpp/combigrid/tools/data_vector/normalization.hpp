#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

/*
Normalizes a point in the area to [0,1]^d inplace.
*/
void normalizeDataVector(base::DataVector& point, const HyperCubeArea& area);

/*
Normalizes a point in the area to [0,1]^d.
*/
base::DataVector normalizeDataVector(const base::DataVector& point, const HyperCubeArea& area);

/*
Denormalizes a point from [0,1]^d to the area inplace.
*/
void denormalizeDataVector(base::DataVector& point, const HyperCubeArea& area);

/*
Denormalizes a point from [0,1]^d to the area.
*/
base::DataVector denormalizeDataVector(const base::DataVector& point, const HyperCubeArea& area);

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp