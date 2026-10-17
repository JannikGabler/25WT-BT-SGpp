/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>

namespace sgpp {
namespace combigrid {

/**
 * TODO: Document
 * More specialized version of quadrature.
 * Requires nested nodes
 */
double quadratureDelta(const SourceFunc& sourceFunc, const DeltasSparseGrid& deltasSparseGrid);

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
std::pair<misc::FlatNestedVec<GPCntType, 2>, GPCntType> compNodeCnts(
    const DeltasSparseGrid& deltasSparseGrid);

/**
 * TODO: Document
 */
misc::FlatNestedVec<double, 3> comp1DWeights(const DeltasSparseGrid& deltasSparseGrid);

/**
 * TODO: Document
 */
std::vector<NodeIdxLvlMapFunc> compNodeIdxLvlMapFuncs(const DeltasSparseGrid& deltasSparseGrid);

/**
 * TODO: Document
 */
double processDeltaTensorGrid(const misc::DeltaQuadratureContext& context,
                              const TensorGridCTData& deltaTGCTData);

/**
 * TODO: Document
 */
double compGlobalWeightForGP(const misc::DeltaQuadratureContext& context,
                             const TensorGridCTData& deltaTGCTData, const GPMI& gpMIInDeltaTG);

/**
 * TODO: Document
 */
double compLocalWeightForGP(const misc::DeltaQuadratureContext& context,
                            const TensorGridCTData& deltaTGCTData, const GPMI& gpMIInDeltaTG,
                            const LvlMI& targetTGMI, const size_t targetTGIdx);

/**
 * TODO: Document
 */
GPCntType mapNodeIdx(const misc::DeltaQuadratureContext& context, const GPCntType nNodesInDeltaTG,
                     const GPCntType nNodesInTargetTG, const LvlType deltaTGLvl,
                     const LvlType targetTGLvl, const GPCntType gpIdxInDeltaTG,
                     const NodeIdxLvlMapFunc nodeIdxLvlMapFunc);

}  // namespace delta_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp