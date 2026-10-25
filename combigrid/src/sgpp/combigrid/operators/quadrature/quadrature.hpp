// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature.hpp
 * @brief Tensor-product quadrature operator on a combination-technique
 * sparse grid.
 *
 * Each contained tensor grid contributes a tensor-product quadrature
 * (Clenshaw-Curtis, Simpson, trapezoidal, etc.) whose 1D rule is selected
 * per dimension by the node-generation function. The combination
 * coefficients yield the final sparse-grid integral approximation.
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>

namespace sgpp {
namespace combigrid {

/**
 * TODO: Document (update)
 * @brief Approximates @f$\int_\Omega f(\vec{x})\,\mathrm{d}\vec{x}@f$ on
 * the domain attached to @p sparseGrid using its combination-technique
 * structure.
 *
 * Internally, every tensor grid in @p sparseGrid is integrated with a
 * tensor-product quadrature rule (parallelized via OpenMP). The resulting
 * estimates are combined via the stored coefficients and finally scaled
 * by the volume of the domain.
 *
 * @param sourceFunc Source function to integrate.
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return Approximated integral value.
 */
double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid);

/******************
Internal operations
******************/

/**
 * @brief Implementation details of the sparse-grid quadrature operator.
 */
namespace quadrature_operator {

/*
 * TODO: Add internal helpers from quadrature.cpp with documentation to the header (was skipped to
 * the inlining issues)
 */

/**
 * @brief Tensor-product quadrature on a single tensor grid.
 *
 * Maps the 1D nodes of @p tg onto the domain once and iterates over the grid points row by row.
 * The row dimension (innermost) is the first dimension with more than one node; dimensions with
 * one node are not iterated (constant coordinate and weight factor). A step to the next row
 * updates only the coordinates and the partial weight products of the dimensions that changed.
 * Each row is summed with the 1D weights of the row dimension and multiplied once by the product
 * of the weights of the other dimensions. The source function is evaluated at the grid points in
 * domain coordinates.
 *
 * @param context Read-only data of the quadrature call (source function, domain and
 *                per-dimension node-generation functions, which provide the quadrature rules).
 * @param tg      Tensor grid (at least one dimension).
 * @param scratch Per-thread scratch space; reused across calls.
 * @return Quadrature estimate on the unit hypercube
 *         @f$[0,1]^d@f$ (the domain volume is multiplied in by the caller).
 */
double processTensorGrid(const sgpp::combigrid::misc::QuadratureContext& context,
                         const TensorGrid& tg, sgpp::combigrid::misc::QuadratureScratch& scratch);

/**
 * TODO: Document (update)
 * @brief Populates @p scratch.weights with the 1D quadrature weights for
 * every dimension of @p tg, concatenated dimension by dimension.
 *
 * @param tg            Tensor grid.
 * @param nodeGenFuncs  Per-dimension node-generation functions (each one
 *                      provides the matching quadrature rule).
 * @param scratch       Scratch space; @c weights and @c quadRules are filled in.
 */
void getWeights(const sgpp::combigrid::misc::QuadratureContext& context, const TensorGrid& tg,
                sgpp::combigrid::misc::QuadratureScratch& scratch);

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp
