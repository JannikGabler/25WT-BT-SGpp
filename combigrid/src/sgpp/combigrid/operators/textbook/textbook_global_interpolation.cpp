// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file textbook_global_interpolation.cpp
 * @brief Textbook combination-technique global interpolation (see
 * @c textbook_global_interpolation.hpp). The file reads top to bottom: 1D Lagrange basis, one
 * tensor grid, the combination of all tensor grids, the public overloads.
 */

#include <algorithm>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <span>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace {

/**
 * Returns the Lagrange basis polynomials of the 1D @p nodes at @p x:
 * L_i(x) = product over k != i of (x - x_k) / (x_i - x_k). One node: L_0 = 1.
 */
base::DataVector lagrangeBasis(const double x, const std::span<const double> nodes) {
  base::DataVector basis(nodes.size(), 1.0);
  for (size_t i = 0; i < nodes.size(); i++) {
    for (size_t k = 0; k < nodes.size(); k++) {
      if (k != i) {
        basis[i] *= (x - nodes[k]) / (nodes[i] - nodes[k]);
      }
    }
  }
  return basis;
}

/**
 * Returns the product of the 1D basis values of the grid point @p idx of a tensor grid. Its 1D
 * indices follow from @p idx as in @c TensorGridView::getGridPoint: dimension 0 runs fastest.
 */
double tensorProductBasis(const std::vector<base::DataVector>& basis1D, size_t idx) {
  double basisValue = 1.0;
  for (const base::DataVector& basisOfDim : basis1D) {
    basisValue *= basisOfDim[idx % basisOfDim.size()];
    idx /= basisOfDim.size();
  }
  return basisValue;
}

/**
 * Sets @p tgResult to the tensor-product Lagrange interpolant of the m outputs of @p sourceFunc
 * on one tensor grid at the point @p unitPoint (on [0, 1]^d): sum over the grid points of
 * (product of the 1D basis values) * f(grid point). The basis uses the nodes on [0, 1]
 * (@p unitTG), the source function the grid points in domain coordinates (@p domainTG).
 */
void interpolateTensorGrid(const VectorSourceFunc& sourceFunc, const base::DataVector& unitPoint,
                           const TensorGridView& domainTG, const TensorGridView& unitTG,
                           const std::span<double> tgResult) {
  std::vector<base::DataVector> basis1D;
  for (size_t dim = 0; dim < unitTG.nDim(); dim++) {
    basis1D.push_back(lagrangeBasis(unitPoint[dim], unitTG.nodeSpanInDim(dim)));
  }
  base::DataVector gridPoint(domainTG.nDim());
  std::vector<double> values(sourceFunc.nOutputs());

  std::ranges::fill(tgResult, 0.0);
  for (size_t idx = 0; idx < domainTG.nGP(); idx++) {
    domainTG.getGridPoint(idx, gridPoint);
    sourceFunc.evaluate(gridPoint, values);
    const double basisValue = tensorProductBasis(basis1D, idx);
    for (size_t k = 0; k < values.size(); k++) {
      tgResult[k] += basisValue * values[k];
    }
  }
}

/**
 * Sets @p result to the combination-technique interpolant at @p point: the sum over the tensor
 * grids of their coefficient times their interpolant.
 *
 * This is the combination technique: the sparse-grid approximation is a linear combination of
 * approximations on small, anisotropic full tensor grids, each computed on its own with a standard
 * method. The integer coefficients c_l (stored in the grid) cancel the overlap of the tensor
 * grids; for functions with bounded mixed derivatives, the sum is nearly as accurate as the full
 * tensor grid of the highest level (up to logarithmic factors) at a fraction of its grid points.
 */
void combineTensorGrids(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                        const SparseGrid& sparseGrid, const std::span<double> result) {
  if (point.size() != sparseGrid.nDim()) {
    throw std::invalid_argument(
        "textbookInterpolate: The size of the point is not the dimensionality of the grid");
  }
  const base::DataVector unitPoint =
      tools::normalizeDataVector(point, sparseGrid.getGenInstr().getDomain());
  std::vector<double> tgResult(result.size());

  std::ranges::fill(result, 0.0);
  for (size_t tgIdx = 0; tgIdx < sparseGrid.nTG(); tgIdx++) {
    const TensorGridCTDataView domainTG = sparseGrid.getDomainTensorGrid(tgIdx);
    const TensorGridCTDataView unitTG = sparseGrid.getUnitTensorGrid(tgIdx);
    interpolateTensorGrid(sourceFunc, unitPoint, domainTG.tensorGrid, unitTG.tensorGrid, tgResult);
    for (size_t k = 0; k < result.size(); k++) {
      result[k] += static_cast<double>(domainTG.coefficient) * tgResult[k];
    }
  }
}

}  // namespace

double textbookInterpolate(const SourceFunc& sourceFunc, const base::DataVector& point,
                           const SparseGrid& sparseGrid) {
  // The scalar function as a function with one output: both variants run the same computation
  const VectorSourceFunc asVectorFunc(
      [&sourceFunc](const base::DataVector& x, const std::span<double> out) {
        out[0] = sourceFunc.evaluate(x);
      },
      1);
  double result = 0.0;
  combineTensorGrids(asVectorFunc, point, sparseGrid, std::span<double>(&result, 1));

  sourceFunc.finishOperatorCall();
  return result;
}

void textbookInterpolate(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                         const SparseGrid& sparseGrid, const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument(
        "textbookInterpolate: The size of the result is not the number of outputs");
  }

  combineTensorGrids(sourceFunc, point, sparseGrid, result);

  sourceFunc.finishOperatorCall();
}

base::DataVector textbookInterpolate(const VectorSourceFunc& sourceFunc,
                                     const base::DataVector& point, const SparseGrid& sparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  textbookInterpolate(sourceFunc, point, sparseGrid, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
