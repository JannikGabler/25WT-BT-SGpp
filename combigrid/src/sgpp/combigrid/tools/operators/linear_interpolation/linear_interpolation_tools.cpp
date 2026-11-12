// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <bit>
#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <tuple>
#include <utility>

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

std::pair<double, double> weights1D(const double x, const double lower, const double upper) {
  const double length = upper - lower;
  if (length == 0.0) {
    return {0.5, 0.5};
  }
  return {(upper - x) / length, (x - lower) / length};
}

void prepareCorners(const base::DataVector& point, const TensorGrid& tg,
                    const HyperCubeArea& domain, LinInterpScratch& scratch) {
  const size_t nDim = tg.nDim();
  tg.getNeighborsForLinInterpolation(point, scratch.neighborsPerDim);
  double* const bracket = scratch.bracket();
  double* const factors = scratch.factors();

  for (size_t k = 0; k < nDim; k++) {
    const auto [lower, upper] = scratch.neighborsPerDim[k];
    const double width = domain[k].second - domain[k].first;
    bracket[2 * k] = tools::mapToDomain(lower, domain[k].first, width);
    bracket[2 * k + 1] = tools::mapToDomain(upper, domain[k].first, width);
    std::tie(factors[2 * k], factors[2 * k + 1]) = weights1D(point[k], lower, upper);
    scratch.curGridPoint[k] = bracket[2 * k];
  }

  fillWeightTable(factors, scratch.tableDim(), scratch.weightTable());
}

void fillWeightTable(const double* const factors, const size_t tableDim, double* const table) {
  // Doubling: entry j + 2^k = entry j times the upper weight of dimension k, entry j times the
  // lower one. Every entry is the product of its 1D weights in the order of the dimensions,
  // starting from 1.0 (bitwise the weights of the definition).
  table[0] = 1.0;
  for (size_t k = 0; k < tableDim; k++) {
    const size_t half = size_t{1} << k;
    for (size_t j = 0; j < half; j++) {
      table[j + half] = table[j] * factors[2 * k + 1];
      table[j] *= factors[2 * k];
    }
  }
}

void moveToCorner(const size_t corner, const double* const bracket, double* const gp) {
  const auto t = static_cast<size_t>(std::countr_zero(corner));
  gp[t] = bracket[2 * t + 1];
  for (size_t k = 0; k < t; k++) {
    gp[k] = bracket[2 * k];
  }
}

/*
 * Corner loop, written once for both value types (misc::ScalarValues, misc::VectorValues): the
 * weight of a corner is computed once and serves all outputs. The corners run in blocks of
 * 2^tableDim (the table covers the low bits of the corner index); the 1D weights of the higher
 * dimensions are fixed per block and multiplied per corner, after the table entry, which keeps
 * the order of the definition. The scratch pointers are copied into locals: the source function
 * is an opaque call.
 */
template <typename ValueType>
typename ValueType::Acc sumOverCorners(const ValueType values, const base::DataVector& point,
                                       const TensorGridCTData& tgData, const SGGenInstr& genInstr,
                                       LinInterpScratch& scratch) {
  const size_t nDim = tgData.tensorGrid.nDim();
  typename ValueType::Acc result = values.zeroAcc(LinInterpScratch::RESULT_ACC);
  if (nDim == 0) {
    return result;
  }

  prepareCorners(point, tgData.tensorGrid, genInstr.getDomain(), scratch);
  const size_t tableDim = scratch.tableDim();
  const size_t nHigh = nDim - tableDim;
  const size_t blockSize = size_t{1} << tableDim;
  const size_t nCorners = size_t{1} << nDim;
  const double* const table = scratch.weightTable();
  const double* const bracket = scratch.bracket();
  const double* const factors = scratch.factors();
  double* const high = scratch.highFactors();
  const base::DataVector& gp = scratch.curGridPoint;
  double* const gpData = scratch.curGridPoint.data();

  for (size_t block = 0; block < nCorners; block += blockSize) {
    for (size_t k = 0; k < nHigh; k++) {
      high[k] = factors[2 * (tableDim + k) + ((block >> (tableDim + k)) & 1)];
    }
    for (size_t j = 0; j < blockSize; j++) {
      double weight = table[j];
      for (size_t k = 0; k < nHigh; k++) {
        weight *= high[k];
      }
      values.addScaled(result, weight, values.evaluate(gp));
      if (block + j + 1 < nCorners) {
        moveToCorner(block + j + 1, bracket, gpData);
      }
    }
  }

  return result;
}

}  // namespace lin_interpolation
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
