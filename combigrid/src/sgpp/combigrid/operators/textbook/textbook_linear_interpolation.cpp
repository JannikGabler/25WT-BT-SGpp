// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file textbook_linear_interpolation.cpp
 * @brief Textbook combination-technique multilinear interpolation (see
 * @c textbook_linear_interpolation.hpp). The file reads top to bottom: 1D cell and weights, one
 * tensor grid, the combination of all tensor grids, the public overloads.
 */

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace {

/**
 * Returns the cell [a, b] of the sorted 1D @p nodes (on [0, 1]) that contains @p x: (node, node)
 * if the first node >= x equals x up to 1e-12; (0, first node) below all nodes; (last node, 1)
 * above all nodes; else the two nodes around x.
 */
std::pair<double, double> enclosingCell(const double x, const base::DataVector& nodes) {
  const auto upper = std::ranges::lower_bound(nodes, x);  // The first node >= x

  if (upper == nodes.end()) {
    return {nodes.back(), 1.0};
  } else if (std::abs(*upper - x) <= 1e-12) {
    return {*upper, *upper};
  } else if (upper == nodes.begin()) {
    return {0.0, *upper};
  }

  return {*(upper - 1), *upper};
}

/**
 * Returns the weights of the ends a and b of the 1D cell @p cell at @p x. If x lies on a node
 * (a = b), both ends are that node and get half of the weight each.
 */
std::pair<double, double> linearWeights(const double x, const std::pair<double, double> cell) {
  const double a = cell.first;
  const double b = cell.second;

  if (a == b) {
    return {0.5, 0.5};
  }

  return {(b - x) / (b - a), (x - a) / (b - a)};
}

/**
 * Returns the multilinear interpolant of the m outputs of @p sourceFunc on one tensor
 * grid at the point @p unitPoint (on [0, 1]^d): sum over the 2^d corners of the cell of
 * (product of the 1D weights) * f(corner). Bit j of a corner's number selects the lower (0) or
 * upper (1) end of the cell in dimension j; the corners are mapped onto @p domain.
 */
base::DataVector interpolateTensorGrid(const VectorSourceFunc& sourceFunc,
                                       const base::DataVector& unitPoint,
                                       const TensorGridView& unitTG, const HyperCubeArea& domain) {
  const size_t nDim = unitTG.nDim();
  std::vector<std::pair<double, double>> cells, weights;

  for (size_t dim = 0; dim < nDim; dim++) {
    cells.push_back(enclosingCell(unitPoint[dim], unitTG.nodeVectorInDim(dim)));
    weights.push_back(linearWeights(unitPoint[dim], cells[dim]));
  }

  base::DataVector tgResult(sourceFunc.nOutputs());

  for (size_t cornerIdx = 0; cornerIdx < (size_t{1} << nDim); cornerIdx++) {
    base::DataVector corner(nDim);
    double weight = 1.0;

    for (size_t dim = 0; dim < nDim; dim++) {
      const bool isUpper = ((cornerIdx >> dim) & 1) == 1;
      const double unitCoord = isUpper ? cells[dim].second : cells[dim].first;
      const double width = domain[dim].second - domain[dim].first;
      corner[dim] = tools::mapToDomain(unitCoord, domain[dim].first, width);
      weight *= isUpper ? weights[dim].second : weights[dim].first;
    }

    const base::DataVector values = sourceFunc.evaluate(corner);

    for (size_t k = 0; k < values.size(); k++) {
      tgResult[k] += weight * values[k];
    }
  }

  return tgResult;
}

/**
 * Returns the combination-technique interpolant at @p point: the sum over the tensor
 * grids of their coefficient times their interpolant.
 *
 * This is the combination technique: the sparse-grid approximation is a linear combination of
 * approximations on small, anisotropic full tensor grids, each computed on its own with a standard
 * method. The integer coefficients c_l (stored in the grid) cancel the overlap of the tensor
 * grids; for functions with bounded mixed derivatives, the sum is nearly as accurate as the full
 * tensor grid of the highest level (up to logarithmic factors) at a fraction of its grid points.
 */
base::DataVector combineTensorGrids(const VectorSourceFunc& sourceFunc,
                                    const base::DataVector& point, const SparseGrid& sparseGrid) {
  if (point.size() != sparseGrid.nDim()) {
    throw std::invalid_argument("The size of the point is not the dimensionality of the grid");
  }

  const HyperCubeArea& domain = sparseGrid.getGenInstr().getDomain();
  const base::DataVector unitPoint = tools::normalizeDataVector(point, domain);

  base::DataVector result(sourceFunc.nOutputs());

  for (const TensorGridCTDataView tgData : sparseGrid.unitTensorGrids()) {
    const base::DataVector tgResult =
        interpolateTensorGrid(sourceFunc, unitPoint, tgData.tensorGrid, domain);

    for (size_t k = 0; k < result.size(); k++) {
      result[k] += static_cast<double>(tgData.coefficient) * tgResult[k];
    }
  }

  return result;
}

}  // namespace

double textbookInterpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                                 const SparseGrid& sparseGrid) {
  // The scalar function as a function with one output: both variants run the same computation
  const VectorSourceFunc asVectorFunc(
      [&sourceFunc](const base::DataVector& x, const std::span<double> out) {
        out[0] = sourceFunc.evaluate(x);
      },
      1);

  const double result = combineTensorGrids(asVectorFunc, point, sparseGrid)[0];

  sourceFunc.finishOperatorCall();

  return result;
}

base::DataVector textbookInterpolateLinear(const VectorSourceFunc& sourceFunc,
                                           const base::DataVector& point,
                                           const SparseGrid& sparseGrid) {
  const base::DataVector result = combineTensorGrids(sourceFunc, point, sparseGrid);

  sourceFunc.finishOperatorCall();

  return result;
}

}  // namespace combigrid
}  // namespace sgpp
