// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file textbook_quadrature.cpp
 * @brief Textbook combination-technique quadrature (see @c textbook_quadrature.hpp). The file
 * reads top to bottom: 1D weights, one tensor grid, the combination of all tensor grids, the
 * public overloads.
 */

#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace {

/**
 * Returns, per dimension of @p tg, the weights on [0, 1] of the 1D quadrature rule that the node
 * family of the dimension defines for the number of nodes of the tensor grid in that dimension.
 */
std::vector<base::DataVector> quadratureWeights1D(const TensorGridView& tg,
                                                  const SGGenInstr& genInstr) {
  std::vector<base::DataVector> weights;

  for (size_t dim = 0; dim < tg.nDim(); dim++) {
    const GPCntType nNodes = tg.nGPInDim(dim);
    const QuadRule* const rule = genInstr.getNodeGenFuncForDim(dim)->getQuadRule(nNodes);
    weights.push_back(rule->genWeights(nNodes));
  }

  return weights;
}

/**
 * Returns the product of the 1D weights of the grid point @p idx of a tensor grid. Its 1D indices
 * follow from @p idx as in @c TensorGridView::getGridPoint: dimension 0 runs fastest.
 */
double tensorProductWeight(const std::vector<base::DataVector>& weights1D, size_t idx) {
  double weight = 1.0;

  for (const base::DataVector& weightsOfDim : weights1D) {
    weight *= weightsOfDim[idx % weightsOfDim.size()];
    idx /= weightsOfDim.size();
  }

  return weight;
}

/**
 * Returns the quadrature result of the m outputs of @p sourceFunc on the full
 * tensor grid @p tg (nodes in domain coordinates, weights on the unit cube):
 * sum over the grid points of (product of the 1D weights) * f(grid point).
 */
base::DataVector integrateTensorGrid(const VectorSourceFunc& sourceFunc, const TensorGridView& tg,
                                     const SGGenInstr& genInstr) {
  const std::vector<base::DataVector> weights1D = quadratureWeights1D(tg, genInstr);

  base::DataVector tgResult(sourceFunc.nOutputs());

  for (size_t idx = 0; idx < tg.nGP(); idx++) {
    const base::DataVector gridPoint = tg.getGridPoint(idx);
    const base::DataVector values = sourceFunc.evaluate(gridPoint);
    const double weight = tensorProductWeight(weights1D, idx);

    for (size_t k = 0; k < values.size(); k++) {
      tgResult[k] += weight * values[k];
    }
  }

  return tgResult;
}

/**
 * Returns the combination-technique quadrature: the sum over the tensor grids of their
 * coefficient times their quadrature, scaled from the unit cube to the volume of the domain.
 *
 * This is the combination technique: the sparse-grid approximation is a linear combination of
 * approximations on small, anisotropic full tensor grids, each computed on its own with a standard
 * method. The integer coefficients c_l (stored in the grid) cancel the overlap of the tensor
 * grids; for functions with bounded mixed derivatives, the sum is nearly as accurate as the full
 * tensor grid of the highest level (up to logarithmic factors) at a fraction of its grid points.
 */
base::DataVector combineTensorGrids(const VectorSourceFunc& sourceFunc,
                                    const SparseGrid& sparseGrid) {
  const SGGenInstr& genInstr = sparseGrid.getGenInstr();

  base::DataVector result(sourceFunc.nOutputs());

  for (const TensorGridCTDataView tgData : sparseGrid.domainTensorGrids()) {
    const base::DataVector tgResult = integrateTensorGrid(sourceFunc, tgData.tensorGrid, genInstr);

    for (size_t k = 0; k < result.size(); k++) {
      result[k] += static_cast<double>(tgData.coefficient) * tgResult[k];
    }
  }

  // The 1D weights sum to 1 (unit interval): the integral over the domain scales with its volume
  for (double& value : result) {
    value *= genInstr.getVolumeOfDomain();
  }

  return result;
}

}  // namespace

double textbookQuadrature(const SourceFunc& sourceFunc, const SparseGrid& sparseGrid) {
  // The scalar function as a function with one output: both variants run the same computation
  const VectorSourceFunc asVectorFunc(
      [&sourceFunc](const base::DataVector& x, const std::span<double> out) {
        out[0] = sourceFunc.evaluate(x);
      },
      1);

  const double result = combineTensorGrids(asVectorFunc, sparseGrid)[0];

  sourceFunc.finishOperatorCall();
  return result;
}

base::DataVector textbookQuadrature(const VectorSourceFunc& sourceFunc,
                                    const SparseGrid& sparseGrid) {
  const base::DataVector result = combineTensorGrids(sourceFunc, sparseGrid);

  sourceFunc.finishOperatorCall();

  return result;
}

}  // namespace combigrid
}  // namespace sgpp
