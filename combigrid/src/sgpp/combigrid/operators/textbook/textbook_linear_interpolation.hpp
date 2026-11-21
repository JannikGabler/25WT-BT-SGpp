// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file textbook_linear_interpolation.hpp
 * @brief Textbook variant of the multilinear sparse-grid interpolation: a plain loop over the
 * tensor grids of a @c SparseGrid, each interpolated on its own in the cell that contains the
 * point.
 *
 * Written to be read, not to be fast: no parallelism, no data kept across calls, no shared
 * kernels. For production use, see @c interpolateLinear (@c linear_interpolation.hpp), which
 * computes the same value up to round-off.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Textbook combination-technique multilinear interpolation: evaluates
 * @f[
 *   I f(\vec{x}) = \sum_{\vec{\ell}} c_{\vec{\ell}}\, I_{\vec{\ell}} f(\vec{x}),
 *   \qquad
 *   I_{\vec{\ell}} f(\vec{x}) = \sum_{\vec{s} \in \{0,1\}^d}
 *     \Big(\prod_{j=1}^{d} \lambda^{(j)}_{s_j}\Big) f(\vec{v}_{\vec{s}}),
 * @f]
 * where @f$\vec{\ell}@f$ runs over the tensor grids of @p sparseGrid with their combination
 * coefficients @f$c_{\vec{\ell}}@f$. In dimension @f$j@f$, @f$[a_j, b_j]@f$ is the cell of the
 * tensor grid's nodes that contains @f$x_j@f$, with the 1D weights
 * @f$\lambda^{(j)}_0 = \frac{b_j - x_j}{b_j - a_j}@f$ and
 * @f$\lambda^{(j)}_1 = \frac{x_j - a_j}{b_j - a_j}@f$; @f$\vec{v}_{\vec{s}}@f$ is the corner of
 * the cell with coordinate @f$a_j@f$ (@f$s_j = 0@f$) or @f$b_j@f$ (@f$s_j = 1@f$).
 *
 * The cells follow the conventions of @c interpolateLinear (cells computed on @f$[0,1]@f$, corners
 * mapped to the domain):
 * - a coordinate within @f$10^{-12}@f$ of the first node @f$\ge x_j@f$ lies on that node: the cell
 *   is that node (@f$a_j = b_j@f$, both ends with weight 1/2);
 * - below the first node, the cell reaches to the lower boundary of the domain, above the last
 *   node to the upper one, also if the tensor grid has no boundary nodes there (then the source
 *   function is evaluated on the boundary).
 *
 * Every tensor grid is interpolated independently: the cell and the 1D weights of each dimension
 * are found, then the source function is evaluated at each of the @f$2^d@f$ corners and multiplied
 * by the product of its 1D weights.
 *
 * Complexity: @f$O\big(n_{\mathrm{TG}}\,(d \log n + 2^d (d + C_f))\big)@f$ for
 * @f$n_{\mathrm{TG}}@f$ tensor grids with up to @f$n@f$ nodes per dimension and evaluation cost
 * @f$C_f@f$; serial.
 *
 * The result equals
 * @ref interpolateLinear(const SourceFunc&, const base::DataVector&, const SparseGrid&) up to
 * round-off and uses the same evaluation points (bitwise). That operator is the one to use in
 * production: it is parallel and computes the corner weights incrementally.
 *
 * @param sourceFunc Source function to interpolate.
 * @param point      Evaluation point (domain coordinates); @c size() must equal
 *                   @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Interpolated value.
 * @throws std::invalid_argument if the size of @p point is not the dimensionality of the grid.
 */
double textbookInterpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                                 const SparseGrid& sparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref textbookInterpolateLinear(const SourceFunc&, const base::DataVector&, const SparseGrid&):
 * interpolates the m outputs of @p sourceFunc at @p point, evaluating it once per corner of
 * every tensor grid.
 *
 * Equals the vector-valued @c interpolateLinear on a @c SparseGrid up to round-off. Both the
 * scalar and the vector-valued variant run this computation.
 *
 * @param sourceFunc Vector-valued source function.
 * @param point      Evaluation point; @c size() must equal @c sparseGrid.nDim().
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Interpolated values.
 * @throws std::invalid_argument if the size of @p point is not the dimensionality of the grid.
 */
base::DataVector textbookInterpolateLinear(const VectorSourceFunc& sourceFunc,
                                           const base::DataVector& point,
                                           const SparseGrid& sparseGrid);

}  // namespace combigrid
}  // namespace sgpp
