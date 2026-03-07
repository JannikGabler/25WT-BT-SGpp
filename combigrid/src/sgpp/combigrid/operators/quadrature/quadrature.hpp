#pragma once

#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

double quadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid);

/******************
Internal operations
******************/
namespace quadrature_operator {

double quadrature(const SourceFunc& sourceFunc, const TensorGrid& tg, const SGGenInstr& genInstr);

std::vector<base::DataVector> getWeights(const TensorGrid& tg,
                                         const std::vector<NodeGenFunc*>& nodeGenFuncs);

double getWeightForGP(const std::vector<GPCntType>& gpMI,
                      const std::vector<base::DataVector>& weights);

std::vector<QuadRule*> getQuadRules(const TensorGrid& tg,
                                    const std::vector<NodeGenFunc*>& nodeGenFuncs);

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp