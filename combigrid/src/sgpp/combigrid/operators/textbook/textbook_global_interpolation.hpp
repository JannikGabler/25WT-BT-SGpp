// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file textbook_global_interpolation.hpp
 * @brief Textbook variant of the global sparse-grid interpolation: a plain loop over the tensor
 * grids of a @c SparseGrid, each interpolated on its own with tensor-product Lagrange
 * interpolation.
 *
 * Written to be read, not to be fast: no parallelism, no data kept across calls, no shared
 * kernels. For production use, see @c interpolate (@c global_interpolation.hpp), which computes
 * the same value up to round-off.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief Textbook combination-technique global interpolation: evaluates
 * @f[
 *   I f(\vec{x}) = \sum_{\vec{\ell}} c_{\vec{\ell}}\, I_{\vec{\ell}} f(\vec{x}),
 *   \qquad
 *   I_{\vec{\ell}} f(\vec{x}) = \sum_{\vec{i}} \Big(\prod_{j=1}^{d} L^{(j)}_{\ell_j, i_j}(x_j)\Big)
 *                               f(\vec{x}_{\vec{i}}),
 * @f]
 * where @f$\vec{\ell}@f$ runs over the tensor grids of @p sparseGrid with their combination
 * coefficients @f$c_{\vec{\ell}}@f$, @f$\vec{x}_{\vec{i}}@f$ over the grid points of tensor grid
 * @f$\vec{\ell}@f$ and
 * @f$L_i(x) = \prod_{k \neq i} \frac{x - x_k}{x_i - x_k}@f$ is the Lagrange basis polynomial of
 * the 1D node @f$x_i@f$ (written out as this product). Point and nodes are mapped to
 * @f$[0,1]@f$ for the basis; the source function is evaluated at the grid points in domain
 * coordinates.
 *
 * Every tensor grid is interpolated independently: the 1D Lagrange basis of each dimension is
 * computed at the point, then the source function is evaluated at each grid point and multiplied
 * by the product of its 1D basis values. Every grid point enters, also with a basis value of 0
 * (as in @c interpolate).
 *
 * Complexity: @f$O\big(\sum_{\vec{\ell}} (N_{\vec{\ell}}\,(d + C_f) + \sum_j n_{\ell_j}^2)\big)@f$
 * for @f$N_{\vec{\ell}}@f$ points per tensor grid, @f$n_{\ell_j}@f$ nodes per dimension and
 * evaluation cost @f$C_f@f$; serial.
 *
 * The result equals
 * @ref interpolate(const SourceFunc&, const base::DataVector&, const SparseGrid&) up to round-off
 * for the built-in node families (all interpolate with the Lagrange polynomial; that operator uses
 * the barycentric formula of the node family) and uses the same evaluation points (bitwise).
 * That operator is the one to use in production: it is parallel and shares work between the grid
 * points of a tensor grid.
 *
 * @note Numerics (the formula is chosen to be read, not to be robust): for Clenshaw-Curtis and
 * Chebyshev nodes, the product formula agrees with the barycentric formula of @c interpolate to
 * about @f$10^{-14}@f$ up to 513 nodes per dimension (level 8 with the doubling growth); from
 * about 1000 nodes per dimension on (level @f$\ge@f$ 9), its partial products over- or underflow
 * and the result is wrong. Use @c interpolate there. Lagrange interpolation in equidistant nodes
 * is ill-conditioned from about 20 nodes on (Runge phenomenon) for every formula; the two
 * operators then differ by more than round-off (33 nodes: up to about @f$10^{-8}@f$ relative).
 *
 * @param sourceFunc Source function to interpolate.
 * @param point      Evaluation point (domain coordinates); @c size() must equal
 *                   @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Interpolated value.
 * @throws std::invalid_argument if the size of @p point is not the dimensionality of the grid.
 */
double textbookInterpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                           const SparseGrid& sparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref textbookInterpolate(const SourceFunc&, const base::DataVector&, const SparseGrid&):
 * interpolates the m outputs of @p sourceFunc at @p point, evaluating it once per grid point of
 * every tensor grid.
 *
 * Equals the vector-valued @c interpolate on a @c SparseGrid up to round-off. Both the scalar and
 * the vector-valued variant run this computation.
 *
 * @param sourceFunc Vector-valued source function (m = @c sourceFunc.nOutputs()).
 * @param point      Evaluation point; @c size() must equal @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @param result     Output: the m interpolated values.
 * @throws std::invalid_argument if the size of @p result is not m or the size of @p point is not
 *         the dimensionality of the grid.
 */
void textbookInterpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
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
base::DataVector textbookInterpolate(const VectorSourceFunc& sourceFunc,
                                     const base::DataVector& point, const SparseGrid& sparseGrid);

}  // namespace combigrid
}  // namespace sgpp
