// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file normalization.hpp
 * @brief Affine mapping between @f$[0,1]^d@f$ and an axis-aligned domain.
 */
#pragma once

#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Maps the coordinate @p unit from @f$[0,1]@f$ onto
 * @f$[\mathrm{lower}, \mathrm{lower} + \mathrm{width}]@f$: @c std::fma(unit, width, lower)
 * (one rounding).
 *
 * The one mapping of all operators onto the domain: quadrature maps the nodes of a tensor grid
 * once, interpolation every grid point (@ref denormalizeDataVector), so all pass bitwise
 * identical points to the source function, which a source-function cache relies on for hits
 * across operators. Pinned to the fused operation, so that the result does not depend on whether
 * the compiler contracts a multiplication and an addition (without hardware FMA, @c std::fma is
 * a correctly rounded library call).
 *
 * @param unit  Coordinate in @f$[0,1]@f$.
 * @param lower Lower bound of the domain in this dimension.
 * @param width Width of the domain in this dimension (upper - lower).
 * @return Coordinate in the domain.
 */
inline double mapToDomain(const double unit, const double lower, const double width) {
  return std::fma(unit, width, lower);
}

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
 * @c area[k].first + point[k] * (area[k].second - area[k].first), computed by
 * @ref mapToDomain.
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
