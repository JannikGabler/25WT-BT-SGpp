// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <limits>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace global_interpolation {

double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const TensorGridCTData& tgData, const SGGenInstr& genInstr) {
  double result;
  processTensorGridInternal(misc::ScalarValues(sourceFunc), point, tgData, genInstr, &result);
  return result;
}

void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                       const std::span<double> out) {
  assert(out.size() == sourceFunc.nOutputs());

  // The kernel writes the values to its own vectors (the overloads of evaluateNormalizedInPlace
  // and interpolate with a slot) and never uses the buffer of the view, so out serves as that
  // buffer.
  processTensorGridInternal(misc::VectorValues(sourceFunc, out), point, tgData, genInstr,
                            out.data());
}

template <typename ValueType>
void processTensorGridInternal(const ValueType values, const base::DataVector& point,
                               const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                               double* const out) {
  const size_t nDim = tgData.tensorGrid.nDim();

  if (nDim == 0) {
    std::fill_n(out, values.nOutputs(), 0.0);
    return;
  }

  const std::vector<size_t> interpolationCntPerDim = getInterpolationCntPerDim(tgData.tensorGrid);

  std::vector<double> interpolationResults =
      interpolateFirstDim(values, point[0], tgData, genInstr, interpolationCntPerDim[0]);

  for (size_t dim = 1; dim < nDim; dim++) {
    interpolateLaterDim(values, dim, point[dim], tgData, genInstr, interpolationCntPerDim[dim],
                        interpolationResults);
  }

  assert(interpolationResults.size() == values.nOutputs());
  std::copy_n(interpolationResults.data(), values.nOutputs(), out);
}

/*
 * The kernels are written once for both value types (misc::ScalarValues, misc::VectorValues):
 * an intermediate result holds the values of all outputs (node-major), and the 1D rule
 * interpolates all outputs at once (ValueType::interpolate). The two collapse steps are
 * separate functions for readability only: they carry no inlining attributes, and in the
 * optimized build (LTO) both are inlined into processTensorGridInternal, which stays one
 * function per value type called by the exported overloads (WP-29c: no measurable difference
 * to the former structure with noinline collapse steps).
 */

template <typename ValueType>
std::vector<double> interpolateFirstDim(const ValueType values, const double point,
                                        const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                        const size_t nInterpolations) {
  const size_t nOutputs = values.nOutputs();
  const HyperCubeArea& bounds = genInstr.getDomain();
  const GPCntType nNodes = tgData.tensorGrid.getGPCntPerDim()[0];
  const NodeGenFunc* nodeGenFunc = genInstr.getNodeGenFuncForDim(0);
  const bool addBoundary = tgData.mi[0] >= genInstr.getBoundaryLevelOffset();
  const std::vector<double> nodes = nodeGenFunc->genNodes(nNodes, addBoundary);
  const InterpolationMethod* method = nodeGenFunc->getInterpolationMethod(nNodes);

  base::DataVector gp(tgData.tensorGrid.nDim());
  std::vector<double> results(nInterpolations * nOutputs);
  std::vector<double> funcValues(nNodes * nOutputs);

  for (size_t i = 0; i < nInterpolations; i++) {
    const size_t start = i * nNodes;
    for (size_t j = 0; j < nNodes; j++) {
      tgData.tensorGrid.getGridPoint(start + j, gp);
      values.evaluateNormalizedInPlace(gp, bounds, funcValues, j);
    }

    values.interpolate(*method, point, nodes, funcValues, results, i);
  }

  return results;
}

template <typename ValueType>
void interpolateLaterDim(const ValueType values, const size_t dim, const double point,
                         const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                         const size_t nInterpolations, std::vector<double>& interpolationResults) {
  const size_t nOutputs = values.nOutputs();
  const GPCntType nNodes = tgData.tensorGrid.getGPCntPerDim()[dim];
  const NodeGenFunc* nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);
  const bool addBoundary = tgData.mi[dim] >= genInstr.getBoundaryLevelOffset();
  const std::vector<double> nodes = nodeGenFunc->genNodes(nNodes, addBoundary);
  const InterpolationMethod* method = nodeGenFunc->getInterpolationMethod(nNodes);
  std::vector<double> inputValues(nNodes * nOutputs);

  for (size_t i = 0; i < nInterpolations; i++) {
    std::copy_n(interpolationResults.data() + i * nNodes * nOutputs, nNodes * nOutputs,
                inputValues.begin());

    values.interpolate(*method, point, nodes, inputValues, interpolationResults, i);
  }
  interpolationResults.resize(nInterpolations * nOutputs);
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
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
