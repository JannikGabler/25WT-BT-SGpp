#include <algorithm>
#include <cassert>
#include <limits>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const SparseGrid& sparseGrid) {
  assert(point.size() == sparseGrid.nDim());

  const std::shared_ptr<const SGGenInstr> genInstr = sparseGrid.getGenInstr();
  const base::DataVector normPoint = tools::normalizeDataVector(point, genInstr->getDomain());
  double result = 0;

#pragma omp parallel for reduction(+ : result) schedule(guided)  // TODO: Scheduling
  for (const TensorGridCTData& tgData : sparseGrid) {
    const double v = global_interpolation::interpolate(sourceFunc, normPoint, tgData, *genInstr);
    result += tgData.coefficient * v;
  }

  return result;
}

namespace global_interpolation {

double interpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                   const TensorGridCTData& tgData, const SGGenInstr& genInstr) {
  const size_t nDim = tgData.tensorGrid.nDim();

  if (nDim == 0) {
    return 0;
  }

  const std::vector<size_t> interpolationCntPerDim = getInterpolationCntPerDim(tgData.tensorGrid);

  std::vector<double> interpolationResults =
      interpolateFirstDim(sourceFunc, point[0], tgData, genInstr, interpolationCntPerDim[0]);

  for (size_t dim = 1; dim < nDim; dim++) {
    interpolateLaterDim(dim, point[dim], tgData, genInstr, interpolationCntPerDim[dim],
                        interpolationResults);
  }

  assert(interpolationResults.size() == 1);
  return interpolationResults[0];
}

std::vector<double> interpolateFirstDim(const SourceFunc& sourceFunc, const double point,
                                        const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                        const size_t nInterpolations) {
  const HyperCubeArea& bounds = genInstr.getDomain();
  const GPCntType nNodes = tgData.tensorGrid.getGPCntPerDim()[0];
  const NodeGenFunc* nodeGenFunc = genInstr.getNodeGenFuncForDim(0);
  const bool addBoundary = tgData.mi[0] >= genInstr.getBoundaryLevelOffset();
  const std::vector<double> nodes = nodeGenFunc->genNodes(nNodes, addBoundary);
  const InterpolationMethod* method = nodeGenFunc->getInterpolationMethod(nNodes);

  base::DataVector gp(tgData.tensorGrid.nDim());
  std::vector<double> results(nInterpolations);
  std::vector<double> funcValues(nNodes);

  for (size_t i = 0; i < nInterpolations; i++) {
    const size_t start = i * nNodes;
    for (size_t j = 0; j < nNodes; j++) {
      tgData.tensorGrid.getGridPoint(start + j, gp);
      funcValues[j] = sourceFunc.evaluateNormalizedInPlace(gp, bounds);
    }

    results[i] = method->interpolate(point, nodes, funcValues);
  }

  return results;
}

/*
Inplace
Used for dim >= 1
*/
void interpolateLaterDim(const size_t dim, const double point, const TensorGridCTData& tgData,
                         const SGGenInstr& genInstr, const size_t nInterpolations,
                         std::vector<double>& interpolationResults) {
  const GPCntType nNodes = tgData.tensorGrid.getGPCntPerDim()[dim];
  const NodeGenFunc* nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
  const bool addBoundary = tgData.mi[dim] >= genInstr.getBoundaryLevelOffset();
  const std::vector<double> nodes = nodeGenFunc->genNodes(nNodes, addBoundary);
  const InterpolationMethod* method = nodeGenFunc->getInterpolationMethod(nNodes);
  std::vector<double> inputValues(nNodes);

  for (size_t i = 0; i < nInterpolations; i++) {
    std::copy_n(interpolationResults.begin() + i * nNodes, nNodes, inputValues.begin());

    interpolationResults[i] = method->interpolate(point, nodes, inputValues);
  }
  interpolationResults.resize(nInterpolations);
}

std::vector<size_t> getInterpolationCntPerDim(const TensorGrid& tensorGrid) {
  const size_t nDim = tensorGrid.nDim();
  const GPMI gpCntPerDim = tensorGrid.getGPCntPerDim();
  std::vector<size_t> interpolationCntPerDim(nDim, 0);

  size_t curCnt = 1;
  for (size_t dim = nDim - 1; dim != std::numeric_limits<size_t>::max(); dim--) {
    interpolationCntPerDim[dim] = curCnt;
    curCnt *= gpCntPerDim[dim];
  }

  return interpolationCntPerDim;
}

}  // namespace global_interpolation
}  // namespace combigrid
}  // namespace sgpp