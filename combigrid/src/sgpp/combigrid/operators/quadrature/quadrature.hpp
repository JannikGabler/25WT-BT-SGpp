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
 * (Clenshaw-Curtis, Simpson, trapezoidal, Fejér's first rule, etc.) whose
 * 1D rule is selected per dimension and level by the node-generation function
 * (@c NodeGenFunc::getQuadRule with the level's boundary nodes). The combination
 * coefficients yield the final sparse-grid integral approximation.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_options.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief Approximates @f$\int_\Omega f(\vec{x})\,\mathrm{d}\vec{x}@f$ on
 * the domain attached to @p sparseGrid using its combination-technique
 * structure.
 *
 * Internally, every tensor grid in @p sparseGrid is integrated with a
 * tensor-product quadrature rule (parallelized via OpenMP). The resulting
 * estimates are combined via the stored coefficients and finally scaled
 * by the volume of the domain.
 *
 * The estimates of the tensor grids are combined serially in the order of the tensor grids with
 * compensated summation: the result is bitwise independent of the number of threads, the same in
 * repeated calls, and the round-off of the combination is about one rounding of the result even
 * though the coefficients are large integers of alternating sign.
 *
 * @note Data kept across calls (@c QuadratureOptions::useCrossCallOperatorDataCache, default
 * off): the 1D quadrature weight table of the grid (one double per node of every (dimension,
 * level) row of the unit node table; e.g. 32 MiB for 2 dimensions with Clenshaw-Curtis nodes of
 * level 20), shared with the vector-valued overloads. Without it, every call computes the
 * table. The results do not depend on the option.
 *
 * @param sourceFunc Source function to integrate.
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @param options    Options (see @c QuadratureOptions).
 * @return Approximated integral value.
 */
double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                  const QuadratureOptions& options = QuadratureOptions());

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
 * @param options    Options (see @c QuadratureOptions).
 * @throws std::invalid_argument if the size of @p result is not m.
 */
void quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                std::span<double> result, const QuadratureOptions& options = QuadratureOptions());

/**
 * @brief Variant of @ref quadrature(const VectorSourceFunc&, const SparseGrid&, std::span<double>)
 * that returns the m approximated integrals.
 *
 * @param sourceFunc Vector-valued source function to integrate.
 * @param sparseGrid Sparse grid with attached generation instruction.
 * @param options    Options (see @c QuadratureOptions).
 * @return The m approximated integrals.
 */
base::DataVector quadrature(const VectorSourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                            const QuadratureOptions& options = QuadratureOptions());

}  // namespace combigrid
}  // namespace sgpp
