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
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

/**
 * TODO: Document
 * More specialized version of quadrature.
 * Requires nested nodes
 */
double quadrature(const SourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid);

}  // namespace combigrid
}  // namespace sgpp
