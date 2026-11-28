// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file textbook_quadrature.hpp
 * @brief Textbook variant of the sparse-grid quadrature: a plain loop over the tensor grids of a
 * @c SparseGrid, each integrated on its own with a tensor-product quadrature rule.
 *
 * Written to be read, not to be fast: no parallelism, no data kept across calls, no shared
 * kernels. For production use, see @c quadrature (@c quadrature.hpp), which computes the same
 * value up to round-off.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Textbook combination-technique quadrature: approximates
 * @f$\int_\Omega f(\vec{x})\,\mathrm{d}\vec{x}@f$ as
 * @f[
 *   Q f = |\Omega| \sum_{\vec{\ell}} c_{\vec{\ell}}\, Q_{\vec{\ell}} f,
 *   \qquad
 *   Q_{\vec{\ell}} f = \sum_{\vec{i}} \Big(\prod_{j=1}^{d} w^{(j)}_{\ell_j, i_j}\Big)
 *                      f(\vec{x}_{\vec{i}}),
 * @f]
 * where @f$\vec{\ell}@f$ runs over the tensor grids of @p sparseGrid with their combination
 * coefficients @f$c_{\vec{\ell}}@f$, @f$\vec{x}_{\vec{i}}@f$ over the grid points of tensor grid
 * @f$\vec{\ell}@f$ (domain coordinates) and @f$w^{(j)}_{\ell_j, i}@f$ are the weights on
 * @f$[0,1]@f$ (summing to 1) of the 1D quadrature rule that the node family of
 * dimension @f$j@f$ defines for its @f$n_{\ell_j}@f$ nodes (@c NodeFamily::getQuadRule with
 * the boundary nodes iff @f$\ell_j@f$ is at least the boundary level offset: Clenshaw-Curtis for
 * Clenshaw-Curtis nodes, Simpson / trapezoidal rule for equidistant nodes, the interpolatory rule
 * for first-kind Chebyshev nodes, i.e. Fejér's first rule without boundary nodes).
 *
 * Every tensor grid is integrated independently: its 1D weights are computed, then the source
 * function is evaluated at each of its grid points and multiplied by the product of the 1D
 * weights. A grid point shared by several tensor grids is evaluated once per tensor grid, as in
 * @c quadrature.
 *
 * Complexity: @f$O\big(\sum_{\vec{\ell}} N_{\vec{\ell}}\,(d + C_f)\big)@f$ for
 * @f$N_{\vec{\ell}}@f$ points per tensor grid and evaluation cost @f$C_f@f$, plus the 1D
 * weights of every tensor grid; serial.
 *
 * The result equals @ref quadrature(const SourceFunc&, const SparseGrid&, const QuadratureOptions&)
 * up to round-off (the sums run in a different order) and uses the same evaluation points
 * (bitwise). That operator is the one to use in production: it is parallel and shares work
 * between the grid points of a tensor grid.
 *
 * @param sourceFunc Source function to integrate.
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Approximated integral.
 * @throws base::not_implemented_exception if a node family defines no quadrature rule, as @c
 * quadrature.
 */
double textbookQuadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref textbookQuadrature(const SourceFunc&, const SparseGrid&): approximates the integrals of
 * the m outputs of @p sourceFunc, evaluating it once per grid point of every tensor grid.
 *
 * Equals the vector-valued @c quadrature on a @c SparseGrid up to round-off. Both the scalar and
 * the vector-valued variant run this computation.
 *
 * @param sourceFunc Vector-valued source function.
 * @param sparseGrid Sparse grid with attached generation instruction.
 */
base::DataVector textbookQuadrature(const VectorSourceFunc& sourceFunc,
                                    const SparseGrid& sparseGrid);

}  // namespace combigrid
}  // namespace sgpp
