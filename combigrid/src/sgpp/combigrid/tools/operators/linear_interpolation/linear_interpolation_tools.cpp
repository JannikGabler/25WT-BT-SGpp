// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
 */

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <vector>

using sgpp::combigrid::misc::LinInterpScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace lin_interpolation {

double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                         LinInterpScratch& scratch) {
  return sumOverCorners(misc::ScalarValues(sourceFunc), point, tgData, genInstr, scratch);
}

void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                       LinInterpScratch& scratch, const std::span<double> out) {
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.valueBuffer.size() >=
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), LinInterpScratch::N_ACCS));

  const misc::VectorValues values(sourceFunc, scratch.valueBuffer);
  values.store(sumOverCorners(values, point, tgData, genInstr, scratch), out.data());
}

/*
 * Corner loop, written once for both value types (misc::ScalarValues, misc::VectorValues): the
 * weight of a corner is computed once and serves all outputs. Inlined into the exported
 * functions, so that the scalar one keeps the structure it had before the vector overload.
 */
template <typename ValueType>
typename ValueType::Acc sumOverCorners(const ValueType values, const base::DataVector& point,
                                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                       LinInterpScratch& scratch) {
  const size_t nDim = tgData.tensorGrid.nDim();
  const size_t nNeighbors = tools::pow(static_cast<size_t>(2), nDim);
  const HyperCubeArea& domain = genInstr.getDomain();
  typename ValueType::Acc result = values.zeroAcc(LinInterpScratch::RESULT_ACC);

  if (nDim == 0) {
    return result;
  }

  tgData.tensorGrid.getNeighborsForLinInterpolation(point, scratch.neighborsPerDim);

  for (size_t i = 0; i < nNeighbors; i++) {
    const double weight =
        computeNeighborGPWithWeight(i, point, nDim, scratch.neighborsPerDim, scratch.curGridPoint);
    const auto value = values.evaluateNormalizedInPlace(scratch.curGridPoint, domain);
    values.addScaled(result, weight, value);
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

}  // namespace lin_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
