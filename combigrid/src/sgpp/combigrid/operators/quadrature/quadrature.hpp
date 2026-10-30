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

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <span>

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

/**
 * @brief Vector-valued counterpart of @ref quadrature(const SourceFunc&, const SparseGrid&):
 * approximates the integrals of the m outputs of @p sourceFunc, evaluating it once per grid
 * point.
 *
 * Output k equals the scalar quadrature of output k (as a @c SourceFunc) up to round-off.
 *
 * @param sourceFunc Vector-valued source function to integrate (m = @c sourceFunc.nOutputs()).
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @param result     Output: the m approximated integrals.
 * @throws std::invalid_argument if the size of @p result is not m.
 */
void quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                std::span<double> result);

/**
 * @brief Variant of @ref quadrature(const VectorSourceFunc&, const SparseGrid&, std::span<double>)
 * that returns the m approximated integrals.
 *
 * @param sourceFunc Vector-valued source function to integrate.
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @return The m approximated integrals.
 */
base::DataVector quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid);

}  // namespace combigrid
}  // namespace sgpp
