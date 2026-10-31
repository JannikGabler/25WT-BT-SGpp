// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
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
 * TODO: Document
 * More specialized version of quadrature.
 * Requires nested nodes
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
