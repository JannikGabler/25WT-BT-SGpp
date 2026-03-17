#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_scratch.hpp>

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid);

/******************
Internal operations
******************/
namespace quadrature_operator {

double quadrature(const SourceFunc& sourceFunc, const TensorGrid& tg, const SGGenInstr& genInstr,
                  tools::QuadScratch& scratch);

void getWeights(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                tools::QuadScratch& scratch);

double getWeightForGP(const std::vector<GPCntType>& gpMI, const GPMI& gpCntPerDim,
                      const tools::QuadScratch& scratch);

void getQuadRules(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                  tools::QuadScratch& scratch);

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp