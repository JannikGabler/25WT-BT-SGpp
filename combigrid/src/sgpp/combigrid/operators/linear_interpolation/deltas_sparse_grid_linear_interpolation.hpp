// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_linear_interpolation.hpp
 * @brief Multilinear interpolation operator on a deltas sparse grid: the combination technique
 * interpolant of @c interpolateLinear on a @c SparseGrid, evaluating every distinct corner once.
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
 * @brief Evaluates the linear (multilinear) sparse-grid interpolant at @p point on a deltas sparse
 * grid.
 *
 * Computes the same combination @f$\sum_{\vec{\ell}} c_{\vec{\ell}}\,
 * I^{\mathrm{lin}}_{\vec{\ell}} f(\vec{x})@f$ as @c interpolateLinear on the @c SparseGrid of the
 * same generation instruction (equal up to round-off: the tensor grids are combined in another
 * order), but evaluates @p sourceFunc exactly once at every distinct corner of the cells of the
 * tensor grids that contain @p point, and nowhere else. With nested nodes, most corners are shared
 * by many tensor grids (complete grids with boundary level offset 1 and equidistant nodes: 8.3
 * times fewer evaluations at d = 10, level 5; 7.0 times at d = 8, level 5; 6.7 times at d = 5,
 * level 8; 1.9 times at d = 2, level 16). The evaluation order differs from the @c SparseGrid
 * operator: the corners are evaluated grouped by the levels at which their coordinates appear
 * first, in parallel, after all corners have been identified. The cost per call beyond the
 * evaluations is a few walks over the corners of all tensor grids; for very small problems
 * (2 dimensions) it exceeds the saved evaluations of cheap source functions.
 *
 * Per call, the operator allocates a store of m doubles and one byte per potential corner (for a
 * complete grid of level 5 in 10 dimensions about 370,000, i.e. 3.2 MiB for m = 1). On its first
 * call on a grid, it builds and keeps an index of the grid (its multi-indices and sorted delta
 * nodes; thread-safe). Small problems (few tensor grids times corners, see
 * @c constants::lin_interpolation::DSG_PARALLEL_MIN_CORNER_VISITS) run on the calling thread.
 *
 * @param sourceFunc       Black-box source function to interpolate.
 * @param point            Evaluation point (in the domain of the grid's generation instruction);
 *                         must have @c size() == deltasSparseGrid.nDim().
 * @param deltasSparseGrid Deltas sparse grid (nested nodes) to interpolate on.
 * @return Interpolated value (0 for a grid without dimensions).
 */
double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref interpolateLinear(const SourceFunc&, const base::DataVector&, const DeltasSparseGrid&):
 * evaluates the linear interpolants of the m outputs of @p sourceFunc at @p point, evaluating it
 * once per distinct corner (the corner weights serve all outputs; the store holds m values per
 * corner).
 *
 * Output k equals the scalar interpolation of output k (as a @c SourceFunc) up to round-off.
 *
 * @param sourceFunc       Vector-valued source function (m = @c sourceFunc.nOutputs()).
 * @param point            Evaluation point; must have @c size() == deltasSparseGrid.nDim().
 * @param deltasSparseGrid Deltas sparse grid (nested nodes) to interpolate on.
 * @param result           Output: the m interpolated values.
 * @throws std::invalid_argument if the size of @p result is not m.
 */
void interpolateLinear(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
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
base::DataVector interpolateLinear(const VectorSourceFunc& sourceFunc,
                                   const base::DataVector& point,
                                   const DeltasSparseGrid& deltasSparseGrid);

}  // namespace combigrid
}  // namespace sgpp
