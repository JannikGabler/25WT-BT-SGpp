// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file global_interpolation.hpp
 * @brief Global tensor-product interpolation operator on a
 * combination-technique sparse grid.
 *
 * Unlike the multilinear operator in @c linear_interpolation.hpp, the
 * global operator uses a (potentially high-degree) 1D interpolation rule
 * per dimension (e.g. a barycentric formula), assembled into a tensor
 * product: on a tensor grid, the interpolant is the sum over its grid points of the function
 * value times the product of the 1D basis values (@c InterpolationMethod::basisValues).
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Evaluates the global tensor-product sparse-grid interpolant at @p point.
 *
 * Loops over the contained tensor grids (in parallel via OpenMP),
 * performs a tensor-product 1D interpolation on each, and returns the
 * weighted sum @f$\sum_{\vec{\ell}} c_{\vec{\ell}}\,
 * I^{\mathrm{glob}}_{\vec{\ell}} f(\vec{x})@f$. The 1D interpolation rule
 * used in each dimension is the one returned by the dimension's
 * node-generation function (e.g. a barycentric Lagrange formula for
 * Chebyshev nodes).
 *
 * @note Every grid point of every tensor grid enters with its weight, also with a weight of 0
 * (e.g. if a coordinate of @p point is a node, the other nodes of that dimension have the
 * weight 0). So a non-finite value (inf, NaN) of @p sourceFunc at any grid point makes the
 * result non-finite, also if @p point lies on a node.
 *
 * @param sourceFunc Black-box source function.
 * @param point      Evaluation point; @c size() must equal @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Interpolated value.
 */
double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const SparseGrid& sparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref interpolate(const SourceFunc&, const base::DataVector&, const SparseGrid&): evaluates the
 * global interpolants of the m outputs of @p sourceFunc at @p point, evaluating it once per grid
 * point.
 *
 * Output k equals the scalar interpolation of output k (as a @c SourceFunc) up to round-off; as
 * there, a non-finite value of output k at any grid point makes output k non-finite.
 *
 * @param sourceFunc Vector-valued source function (m = @c sourceFunc.nOutputs()).
 * @param point      Evaluation point; @c size() must equal @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @param result     Output: the m interpolated values.
 * @throws std::invalid_argument if the size of @p result is not m.
 */
void interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                 const SparseGrid& sparseGrid, std::span<double> result);

/**
 * @brief Variant of the @c std::span overload for a @c VectorSourceFunc that returns the m
 * interpolated values.
 *
 * @param sourceFunc Vector-valued source function.
 * @param point      Evaluation point; @c size() must equal @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return The m interpolated values.
 */
base::DataVector interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                             const SparseGrid& sparseGrid);

}  // namespace combigrid
}  // namespace sgpp
