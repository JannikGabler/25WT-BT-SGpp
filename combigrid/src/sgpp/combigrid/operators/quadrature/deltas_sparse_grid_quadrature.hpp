// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_quadrature.hpp
 * @brief Quadrature operator on a deltas sparse grid: the combination technique quadrature of
 * @c quadrature on a @c SparseGrid, evaluating every grid point once.
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
 * @brief Approximates the integral of @p sourceFunc over the domain of the grid's generation
 * instruction on a deltas sparse grid.
 *
 * Computes the same combination @f$\sum_{\vec{\ell}} c_{\vec{\ell}}\, Q_{\vec{\ell}} f@f$ as
 * @c quadrature on the @c SparseGrid of the same generation instruction (equal up to round-off),
 * but evaluates @p sourceFunc exactly once at every grid point of the sparse grid and weights it
 * with its global weight (the sum over all tensor grids containing the point of the coefficient
 * times the product of the 1D quadrature weights). Requires nested nodes (guaranteed by the
 * construction of the @c DeltasSparseGrid).
 *
 * @param sourceFunc       Black-box source function to integrate.
 * @param deltasSparseGrid Deltas sparse grid with attached generation instruction.
 * @return Approximated integral.
 */
double quadrature(const SourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid);

/**
 * @brief Vector-valued counterpart of
 * @ref quadrature(const SourceFunc&, const DeltasSparseGrid&): approximates the integrals of the
 * m outputs of @p sourceFunc, evaluating it once per grid point (and computing the global weight
 * of a grid point once for all outputs).
 *
 * Output k equals the scalar quadrature of output k (as a @c SourceFunc) up to round-off.
 *
 * @param sourceFunc       Vector-valued source function to integrate (m = @c
 *                         sourceFunc.nOutputs()).
 * @param deltasSparseGrid Deltas sparse grid with attached generation instruction.
 * @param result           Output: the m approximated integrals.
 * @throws std::invalid_argument if the size of @p result is not m.
 */
void quadrature(const VectorSourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid,
                std::span<double> result);

/**
 * @brief Variant of
 * @ref quadrature(const VectorSourceFunc&, const DeltasSparseGrid&, std::span<double>) that
 * returns the m approximated integrals.
 *
 * @param sourceFunc       Vector-valued source function to integrate.
 * @param deltasSparseGrid Deltas sparse grid with attached generation instruction.
 * @return The m approximated integrals.
 */
base::DataVector quadrature(const VectorSourceFunc& sourceFunc,
                            const DeltasSparseGrid& deltasSparseGrid);

}  // namespace combigrid
}  // namespace sgpp
