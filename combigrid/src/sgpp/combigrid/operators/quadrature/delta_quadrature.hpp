/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>

using sgpp::combigrid::misc::DeltaQuadratureContext;
using sgpp::combigrid::misc::FlatNestedVec;

namespace sgpp {
namespace combigrid {

/**
 * TODO: Document
 * More specialized version of quadrature.
 * Requires nested nodes
 */
double quadratureDelta(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid);

/******************
Internal operations
******************/

/**
 * TODO: Document
 */
namespace delta_quadrature_operator {

/**
 * TODO: Document
 */
size_t comp1DWeightsSize(const SparseGrid& sparseGrid);

/**
 * TODO: Document
 */
FlatNestedVec<double, 3> comp1DWeights(const SparseGrid& sparseGrid);

/**
 * TODO: Document
 */
double processDeltaTensorGrid(const DeltaQuadratureContext& context,
                              const TensorGridCTData& deltaTGCTData);

/**
 * TODO: Document
 */
double compGlobalWeightForGP(const DeltaQuadratureContext& context,
                             const TensorGridCTData& deltaTGCTData, const GPMI& gpMIInDeltaTG);

/**
 * TODO: Document
 */
double compLocalWeightForGP(const DeltaQuadratureContext& context,
                            const TensorGridCTData& deltaTGCTData, const GPMI& gpMIInDeltaTG,
                            const LvlMI& targetTGMI, const size_t targetTGIdx);

}  // namespace delta_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp