#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

namespace sgpp {
namespace combigrid {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const SparseGrid& sparseGrid);

/******************
Internal operations
******************/
namespace interpolation {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const TensorGridCTData& tgData, const SGGenInstr& genInstr);

std::vector<double> interpolateFirstDim(const SourceFunc& sourceFunc, double point,
                                        const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                        size_t nInterpolations);

void interpolateLaterDim(size_t dim, double point, const TensorGridCTData& tgData,
                         const SGGenInstr& genInstr, size_t nInterpolations,
                         std::vector<double>& interpolationResults);

std::vector<size_t> getInterpolationCntPerDim(const TensorGrid& tensorGrid);

}  // namespace interpolation
}  // namespace combigrid
}  // namespace sgpp