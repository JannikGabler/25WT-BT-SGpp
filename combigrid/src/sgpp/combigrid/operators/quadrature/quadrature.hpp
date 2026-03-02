#pragma once

#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

double quadrature(const SparseGrid& sparseGrid, SourceFunc sourceFunc);

double quadrature(const TensorGrid& tg, const std::vector<NodeGenFunc>& nodeGenFuncs,
                  const SourceFunc sourceFunc);

namespace quadrature_operator {

std::vector<QuadRule> getQuadRules(const TensorGrid& tg,
                                   const std::vector<NodeGenFunc>& nodeGenFuncs);

double getWeightForGP(const std::vector<GPCntType>& gpMI,
                      const std::vector<base::DataVector>& weights);

std::vector<base::DataVector> getWeights(const TensorGrid& tg,
                                         const std::vector<NodeGenFunc>& nodeGenFuncs);

}  // namespace quadrature_operator

}  // namespace combigrid
}  // namespace sgpp