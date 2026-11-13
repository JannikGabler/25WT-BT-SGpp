// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_global_interpolation.hpp
 * @brief Global interpolation operator on a deltas sparse grid: the combination technique
 * interpolant of @c interpolate on a @c SparseGrid, evaluating every grid point once and weighting
 * it with its global weight.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief Evaluates the global tensor-product sparse-grid interpolant at @p point on a deltas
 * sparse grid.
 *
 * Computes the same combination @f$\sum_{\vec{\ell}} c_{\vec{\ell}}\,
 * I^{\mathrm{glob}}_{\vec{\ell}} f(\vec{x})@f$ as @c interpolate on the @c SparseGrid of the same
 * generation instruction (with the same 1D interpolation methods of the node-generation
 * functions), but evaluates @p sourceFunc exactly once at every grid point of the sparse grid
 * (the points of all tensor grids, each distinct point once; point with a global weight of 0 are
 * also evaluated) and weights it with its global weight @f$W_p(\vec{x}) = \sum_{\vec{\ell} \ni p}
 * c_{\vec{\ell}} \prod_k L_{k,\ell_k,p_k}(x_k)@f$, where @f$L_{k,\ell,i}(x)@f$ is the weight of
 * node i in the 1D interpolant of level @f$\ell@f$ (@c InterpolationMethod::basisValues; the
 * built-in barycentric formulas compute them in one pass over the nodes, other methods by one
 * interpolation per node). With nested nodes, most points are shared by many tensor grids (complete
 * grids with boundary level offset 0 and Clenshaw-Curtis nodes: 4.2 times fewer evaluations at d =
 * 10, level 2; 7.8 times at d = 5, level 6; 2.75 times at d = 2, level 12), and no 1D interpolation
 * per tensor grid is needed.
 *
 * Per call, the operator computes the basis values of every (dimension, level) and, per delta
 * tensor grid, the projected sub-trie of the deltas sparse grid quadrature (memory: one double
 * per node of every level, plus the per-thread scratch of the quadrature). On its first call on a
 * grid, it builds and keeps an index of the grid (the trie of the combination coefficients and
 * the nodes of every level; thread-safe). Grids with fewer than
 * @c constants::global_interpolation::DSG_PARALLEL_MIN_GRID_POINTS grid points are processed on
 * the calling thread.
 *
 * @note As on a @c SparseGrid, a non-finite value (inf, NaN) of @p sourceFunc at any grid point
 * makes the result non-finite (points with a global weight of 0 enter as well), also if
 * @p point lies on a node.
 *
 * @param sourceFunc       Black-box source function to interpolate.
 * @param point            Evaluation point (in the domain of the grid's generation instruction);
 *                         must have @c size() == deltasSparseGrid.nDim().
 * @param deltasSparseGrid Deltas sparse grid (nested nodes) to interpolate on.
 * @return Interpolated value (0 for a grid without dimensions).
 */
double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref interpolate(const SourceFunc&, const base::DataVector&, const DeltasSparseGrid&):
 * evaluates the global interpolants of the m outputs of @p sourceFunc at @p point, evaluating it
 * once per grid point (the global weight of a grid point serves all outputs).
 *
 * Output k equals the scalar interpolation of output k (as a @c SourceFunc) up to round-off.
 *
 * @param sourceFunc       Vector-valued source function (m = @c sourceFunc.nOutputs()).
 * @param point            Evaluation point; must have @c size() == deltasSparseGrid.nDim().
 * @param deltasSparseGrid Deltas sparse grid (nested nodes) to interpolate on.
 * @param result           Output: the m interpolated values.
 * @throws std::invalid_argument if the size of @p result is not m.
 */
void interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                 const DeltasSparseGrid& deltasSparseGrid, std::span<double> result);

/**
 * @brief Variant of the @c std::span overload for a @c VectorSourceFunc that returns the m
 * interpolated values.
 *
 * @param sourceFunc       Vector-valued source function.
 * @param point            Evaluation point; must have @c size() == deltasSparseGrid.nDim().
 * @param deltasSparseGrid Deltas sparse grid (nested nodes) to interpolate on.
 * @return The m interpolated values.
 */
base::DataVector interpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                             const DeltasSparseGrid& deltasSparseGrid);

}  // namespace combigrid
}  // namespace sgpp
