// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <bit>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <tuple>
#include <utility>
#include <vector>

using sgpp::combigrid::misc::LinInterpScratch;

namespace sgpp {
namespace combigrid {

namespace tools {
namespace lin_interpolation {

namespace {

/**
 * Neighbors of the normalized coordinate @p x among the sorted unit @p nodes of one dimension (as
 * TensorGrid::getNeighborsForLinInterpolation): the first node >= x equals x up to 1e-12: both
 * are that node; below all nodes: (0, first node); above all nodes: (last node, 1).
 *
 * The first node >= x is found by a branchless binary search (the same index as std::lower_bound):
 * the halving steps select with a conditional move instead of a data-dependent branch. Compared
 * with std::lower_bound on the linear interpolation of a 2D sparse grid of level 10-16 (one
 * query, 1 thread), the branchy loop ran at a 20 % lower IPC depending on its code placement.
 */
std::pair<double, double> findNeighbors(const double x, const std::span<const double> nodes) {
  assert(!nodes.empty());

  const double* first = nodes.data();

  for (size_t len = nodes.size(); len > 1;) {
    const size_t half = len / 2;
    first = (first[half - 1] < x) ? first + half : first;
    len -= half;
  }

  const double* const it = first + (*first < x ? 1 : 0);
  const double* const end = nodes.data() + nodes.size();

  if (it == end) {  // No node >= x
    return {nodes.back(), 1.0};
  } else if (std::abs(*it - x) <= 1E-12) {  // There is a node equal to x
    return {*it, *it};
  } else if (it == nodes.data()) {  // No node <= x
    return {0.0, *it};
  }

  return {*(it - 1), *it};
}

/**
 * Calls @p process(tgIdx, scratch) for every tensor grid of @p sparseGrid in parallel, with a
 * scratch per thread for m = @p nOutputs (0 for a scalar source function).
 *
 * Dynamic scheduling to balance out source functions with position-dependent cost.
 * Every tensor grid has the same number of corners (2^d), so no largest-first order.
 * The price is cache locality in repeated calls on the same grid.
 */
template <typename Process>
void forEachTensorGrid(const SparseGrid& sparseGrid, const size_t nOutputs, Process&& process) {
  const size_t nTG = sparseGrid.nTG();

#pragma omp parallel
  {
    LinInterpScratch scratch(sparseGrid.nDim(), nOutputs);

#pragma omp for schedule(dynamic) nowait
    for (size_t tgIdx = 0; tgIdx < nTG; tgIdx++) {
      process(tgIdx, scratch);
    }
  }
}

}  // namespace

std::vector<double> estimateTensorGrids(const SourceFunc& sourceFunc,
                                        const base::DataVector& normPoint,
                                        const SparseGrid& sparseGrid) {
  const SparseGridSetup& setup = sparseGrid.getSetup();

  std::vector<double> values(sparseGrid.nTG());

  forEachTensorGrid(sparseGrid, 0, [&](const size_t tgIdx, LinInterpScratch& scratch) {
    values[tgIdx] = processTensorGrid(
        sourceFunc, normPoint, sparseGrid.getUnitTensorGrid(tgIdx).tensorGrid, setup, scratch);
  });

  return values;
}

misc::FlatNestedVec<double, 2> estimateTensorGrids(const VectorSourceFunc& sourceFunc,
                                                   const base::DataVector& normPoint,
                                                   const SparseGrid& sparseGrid) {
  const size_t nOutputs = sourceFunc.nOutputs();
  const SparseGridSetup& setup = sparseGrid.getSetup();

  misc::FlatNestedVec<double, 2> values(sparseGrid.nTG(), nOutputs);

  forEachTensorGrid(sparseGrid, nOutputs, [&](const size_t tgIdx, LinInterpScratch& scratch) {
    processTensorGrid(sourceFunc, normPoint, sparseGrid.getUnitTensorGrid(tgIdx).tensorGrid, setup,
                      scratch, values[tgIdx]);
  });

  return values;
}

double processTensorGrid(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const TensorGridView& unitTG, const SparseGridSetup& setup,
                         LinInterpScratch& scratch) {
  return sumOverCorners(misc::ScalarValues(sourceFunc), point, unitTG, setup, scratch);
}

void processTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const TensorGridView& unitTG, const SparseGridSetup& setup,
                       LinInterpScratch& scratch, const std::span<double> out) {
  assert(out.size() == sourceFunc.nOutputs());
  assert(scratch.valueBuffer.size() >=
         misc::VectorValues::bufferSize(sourceFunc.nOutputs(), LinInterpScratch::N_ACCS));

  const misc::VectorValues values(sourceFunc, scratch.valueBuffer);
  values.store(sumOverCorners(values, point, unitTG, setup, scratch), out.data());
}

std::pair<double, double> weights1D(const double x, const double lower, const double upper) {
  const double length = upper - lower;

  if (length == 0.0) {
    return {0.5, 0.5};
  }

  return {(upper - x) / length, (x - lower) / length};
}

void prepareCorners(const base::DataVector& point, const TensorGridView& unitTG,
                    const HyperCubeArea& domain, LinInterpScratch& scratch) {
  const size_t nDim = unitTG.nDim();
  double* const bracket = scratch.bracket();
  double* const factors = scratch.factors();

  for (size_t k = 0; k < nDim; k++) {
    const std::pair<double, double> neighbors = findNeighbors(point[k], unitTG.nodeSpanInDim(k));
    const double width = domain[k].second - domain[k].first;

    bracket[2 * k] = tools::mapToDomain(neighbors.first, domain[k].first, width);
    bracket[2 * k + 1] = tools::mapToDomain(neighbors.second, domain[k].first, width);

    std::tie(factors[2 * k], factors[2 * k + 1]) =
        weights1D(point[k], neighbors.first, neighbors.second);

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
                                       const TensorGridView& unitTG, const SparseGridSetup& setup,
                                       LinInterpScratch& scratch) {
  const size_t nDim = unitTG.nDim();
  typename ValueType::Acc result = values.zeroAcc(LinInterpScratch::RESULT_ACC);
  if (nDim == 0) {
    return result;
  }

  prepareCorners(point, unitTG, setup.getDomain(), scratch);
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
