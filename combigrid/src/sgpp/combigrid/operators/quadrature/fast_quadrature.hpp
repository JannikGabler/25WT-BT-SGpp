#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

/**
TODO: Document
More specialized version of quadrature.
Requires nested nodes
*/
double quadratureFast(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid);

/******************
Internal operations
******************/

/**
TODO: Document
*/
namespace fast_quadrature_operator {

/**
TODO: Document
*/
double processTensorGrid(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid,
                         const TensorGridCTData& tgData);

/**
TODO: Document
*/
double getLocalQuadratureWeightForGP();

/**
TODO: Document
*/
void getQuadRules(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs);

}  // namespace fast_quadrature_operator

}  // namespace combigrid
}  // namespace sgpp