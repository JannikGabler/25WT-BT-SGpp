#include <omp.h>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using LinInterpScratch = sgpp::combigrid::tools::LinInterpScratch;

namespace sgpp {
namespace combigrid {

double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const SparseGrid& sparseGrid) {
  assert(point.size() == sparseGrid.nDim());

  const size_t nDim = sparseGrid.nDim();
  const size_t maxThreads = static_cast<size_t>(omp_get_max_threads());

  const std::shared_ptr<const SGGenInstr> genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = tools::normalizeDataVector(point, genInstr->getDomain());
  std::vector<LinInterpScratch> scratch(maxThreads, LinInterpScratch(nDim));
  double result = 0;

#pragma omp parallel for reduction(+ : result) schedule(static)
  for (const TensorGridCTData& tgData : sparseGrid) {
    const size_t threadId = static_cast<size_t>(omp_get_thread_num());
    const double v = linear_interpolation::interpolate(sourceFunc, normPoint, tgData, *genInstr,
                                                       scratch[threadId]);
    result += tgData.coefficient * v;
  }

  return result;
}

namespace linear_interpolation {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                   LinInterpScratch& scratch) {
  const size_t nDim = tgData.tensorGrid.nDim();
  const size_t nNeighbors = tools::pow(static_cast<size_t>(2), nDim);
  const HyperCubeArea& domain = genInstr.getDomain();

  if (nDim == 0) {
    return 0;
  }

  tgData.tensorGrid.getNeighborsForLinInterpolation(point, scratch.neighborsPerDim);
  double result = 0.0;

  for (size_t i = 0; i < nNeighbors; i++) {
    const double weight =
        computeNeighborGPWithWeight(i, point, nDim, scratch.neighborsPerDim, scratch.curGridPoint);
    const double value = sourceFunc.evaluateNormalizedInPlace(scratch.curGridPoint, domain);
    result += weight * value;
  }

  return result;
}

double computeNeighborGPWithWeight(size_t idx, const base::DataVector& point, const size_t nDim,
                                   const std::vector<std::pair<double, double>>& neighborsPerDim,
                                   base::DataVector& gpOut) {
  double weight = 1.0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const double length = neighborsPerDim[dim].second - neighborsPerDim[dim].first;

    if (length == 0.0) {
      weight *= 0.5;
      gpOut[dim] = neighborsPerDim[dim].first;
    } else {
      if (idx & 1) {  // Odd
        gpOut[dim] = neighborsPerDim[dim].second;
        weight *= (point[dim] - neighborsPerDim[dim].first) / length;
      } else {  // Even
        gpOut[dim] = neighborsPerDim[dim].first;
        weight *= (neighborsPerDim[dim].second - point[dim]) / length;
      }
    }

    idx /= 2;
  }

  return weight;
}

}  // namespace linear_interpolation
}  // namespace combigrid
}  // namespace sgpp