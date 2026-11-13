// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file weighted_sum_tools.hpp
 * @brief Weighted sum over the grid points of a tensor grid: the per-tensor-grid kernel shared by
 * the quadrature and the global interpolation on a @c SparseGrid.
 *
 * On a tensor grid, both operators compute @f$\sum_p \prod_k w_{k,p_k} f(p)@f$: the quadrature
 * with the 1D quadrature weights as @f$w_{k,i}@f$ (@c tools::quadrature::processTensorGrid), the
 * global interpolation with the 1D basis values at the query point
 * (@c tools::global_interpolation::processTensorGrid). The caller writes the 1D weights to
 * @c misc::WeightedSumScratch::weights1D, then prepares the row loop (@ref mapNodesOntoDomain,
 * @ref setUpIterDims) and runs it (@ref sumOverRows).
 */
#pragma once

#include <cstddef>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/weighted_sum_scratch.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace weighted_sum {

/**
 * @brief Maps the nodes of @p tg onto @p domain (@c tools::mapToDomain, once per tensor grid)
 * into @c scratch.nodesPerDim and moves @c scratch.gp to the first grid point.
 *
 * @param domain  Domain of the source function.
 * @param tg      Tensor grid.
 * @param scratch Scratch space (@c nodesPerDim large enough for the nodes of @p tg).
 */
void mapNodesOntoDomain(const HyperCubeArea& domain, const TensorGrid& tg,
                        misc::WeightedSumScratch& scratch);

/**
 * @brief Sets up the iterated dimensions of the row loop (@ref sumOverRows) over @p tg, using
 * the nodes of @ref mapNodesOntoDomain and the weights in @c scratch.weights1D (one per node,
 * per-dimension blocks back-to-back).
 *
 * @param tg      Tensor grid (at least one dimension).
 * @param scratch Scratch space with the nodes and weights of @p tg.
 */
void setUpIterDims(const TensorGrid& tg, misc::WeightedSumScratch& scratch);

/**
 * @brief Returns the row dimension of the row loop: the first dimension with more than one node
 * (or the last dimension if there is none).
 *
 * @param nGPPerDim Number of nodes per dimension of the tensor grid (at least one dimension).
 */
size_t findRowDim(const GPMI& nGPPerDim);

/**
 * @brief Recomputes the weight products of the iterated dimensions @p j, ..., 1 (see
 * @c misc::WeightedSumScratch::weightProducts) after their node indices changed
 * (@c weightProducts[j + 1] must be up to date).
 *
 * @param weightsOfIterDim Start of the 1D weights of every iterated dimension.
 * @param gpMI             Node indices of the current grid point in the iterated dimensions.
 * @param weightProducts   Weight products; the entries @p j, ..., 1 are overwritten.
 * @param j                Highest iterated dimension to update.
 */
void updateWeightProducts(const double* const* const weightsOfIterDim, const GPCntType* const gpMI,
                          double* const weightProducts, size_t j);

/**
 * @brief Sums the weight times the source function over all grid points of a tensor grid
 * prepared by @ref mapNodesOntoDomain and @ref setUpIterDims; the weight of a grid point is the
 * product of the 1D weights of its nodes.
 *
 * Instantiated for @c misc::ScalarValues and @c misc::VectorValues (in the @c .cpp).
 *
 * @tparam ValueType @c misc::ScalarValues or @c misc::VectorValues.
 * @param values  View of the source function.
 * @param scratch Prepared scratch space.
 * @return Accumulator of the sums (slot @c misc::WeightedSumScratch::TG_SUM_ACC).
 */
template <typename ValueType>
typename ValueType::Acc sumOverRows(const ValueType values, misc::WeightedSumScratch& scratch);

}  // namespace weighted_sum
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
