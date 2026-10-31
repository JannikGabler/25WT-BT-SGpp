// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file linear_interpolation.hpp
 * @brief Multilinear (piecewise-linear in every dimension) interpolation
 * operator on a combination-technique sparse grid.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief Evaluates the linear (multilinear) sparse-grid interpolant at @p point.
 *
 * Loops over the contained tensor grids, performs a multilinear
 * interpolation on each, and returns the combination-technique linear
 * combination @f$\sum_{\vec{\ell}} c_{\vec{\ell}}\,
 * I^{\mathrm{lin}}_{\vec{\ell}} f(\vec{x})@f$. The @p point coordinates
 * are interpreted in the domain attached to the sparse grid's generation
 * instruction and are normalized to @f$[0,1]^d@f$ internally before each
 * tensor-grid evaluation.
 *
 * @param sourceFunc Black-box source function to interpolate.
 * @param point      Evaluation point; must have @c size() == sparseGrid.nDim().
 * @param sparseGrid Sparse grid to interpolate on. Must have an attached
 *                   generation instruction.
 * @return Interpolated value.
 */
double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const SparseGrid& sparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref interpolateLinear(const SourceFunc&, const base::DataVector&, const SparseGrid&):
 * evaluates the linear interpolants of the m outputs of @p sourceFunc at @p point, evaluating it
 * once per corner (the corner weights serve all outputs).
 *
 * Output k equals the scalar interpolation of output k (as a @c SourceFunc) up to round-off.
 *
 * @param sourceFunc Vector-valued source function (m = @c sourceFunc.nOutputs()).
 * @param point      Evaluation point; must have @c size() == sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @param result     Output: the m interpolated values.
 * @throws std::invalid_argument if the size of @p result is not m.
 */
void interpolateLinear(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const SparseGrid& sparseGrid, std::span<double> result);

/**
 * @brief Variant of the @c std::span overload for a @c VectorSourceFunc that returns the m
 * interpolated values.
 *
 * @param sourceFunc Vector-valued source function.
 * @param point      Evaluation point; must have @c size() == sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return The m interpolated values.
 */
base::DataVector interpolateLinear(const VectorSourceFunc& sourceFunc,
                                   const base::DataVector& point, const SparseGrid& sparseGrid);

}  // namespace combigrid
}  // namespace sgpp
