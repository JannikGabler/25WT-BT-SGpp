/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

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
double processDeltaTensorGrid(const SourceFunc& sourceFunc, const TensorGrid& deltaTG,
                              const SGGenInstr& genInstr, tools::QuadScratch& scratch);

/**
 * TODO: Document
 */
double compGlobalWeightForGP(const SparseGrid& sparseGrid, const GPMI& gpMIInDeltaTG,
                             const LvlMI& deltaTGMI,
                             const std::vector<std::vector<base::DataVector>>& weights1D,
                             const LvlMI& targetTGMI, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                             const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs);

/**
 * TODO: Document
 */
double compLocalWeightForGP(const GPMI& gpMIInDeltaTG, const LvlMI& deltaTGMI,
                            const LvlMI& targetTGMI, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                            const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs,
                            const std::vector<base::DataVector>& weights1D);

/**
 * TODO: Document
 */
void getQuadRules(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs);

}  // namespace delta_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp