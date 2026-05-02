#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_scratch.hpp>

namespace sgpp {
namespace combigrid {

double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const SparseGrid& sparseGrid);

/******************
Internal operations
******************/
namespace linear_interpolation {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                   tools::LinInterpScratch& scratch);

double computeNeighborGPWithWeight(size_t idx, const base::DataVector& point, const size_t nDim,
                                   const std::vector<std::pair<double, double>>& neighborsPerDim,
                                   base::DataVector& gpOut);

}  // namespace linear_interpolation
}  // namespace combigrid
}  // namespace sgpp