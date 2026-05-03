/**
 * @file normalization.hpp
 * @brief Affine mapping between @f$[0,1]^d@f$ and an axis-aligned domain.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Maps @p point from @p area onto @f$[0,1]^d@f$ in place.
 *
 * Component @c k of the output is
 * @c (point[k] - area[k].first) / (area[k].second - area[k].first).
 *
 * @param point In/out: point in problem coordinates, overwritten with the
 *              normalized coordinates.
 * @param area  Hyper-rectangular source domain.
 */
void normalizeDataVector(base::DataVector& point, const HyperCubeArea& area);

/**
 * @brief Out-of-place variant of @ref normalizeDataVector(base::DataVector&, const HyperCubeArea&).
 * @param point Point in problem coordinates.
 * @param area  Hyper-rectangular source domain.
 * @return Newly allocated normalized point.
 */
base::DataVector normalizeDataVector(const base::DataVector& point, const HyperCubeArea& area);

/**
 * @brief Maps @p point from @f$[0,1]^d@f$ onto @p area in place.
 *
 * Component @c k of the output is
 * @c area[k].first + point[k] * (area[k].second - area[k].first).
 *
 * @param point In/out: normalized point, overwritten with problem-domain coordinates.
 * @param area  Hyper-rectangular target domain.
 */
void denormalizeDataVector(base::DataVector& point, const HyperCubeArea& area);

/**
 * @brief Out-of-place variant of @ref denormalizeDataVector(base::DataVector&, const HyperCubeArea&).
 * @param point Normalized point.
 * @param area  Hyper-rectangular target domain.
 * @return Newly allocated denormalized point.
 */
base::DataVector denormalizeDataVector(const base::DataVector& point, const HyperCubeArea& area);

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
