// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_tools.hpp
 * @brief Per-tensor-grid kernel of the quadrature operator (@c quadrature) on a @c SparseGrid:
 * the 1D quadrature weights of a tensor grid and the weighted sum over its grid points
 * (@c tools::weighted_sum, shared with the global interpolation).
 */
#pragma once

#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace quadrature {

/**
 * @brief Tensor-product quadrature on a single tensor grid.
 *
 * Computes the 1D quadrature weights of @p tg (@ref getWeights) and sums the weight times the
 * source function over the grid points (@c tools::weighted_sum::sumOverRows): the nodes are mapped
 * onto the domain once, the grid points are visited row by row, the dimensions with one node are
 * not iterated (constant coordinate and weight factor), and a step to the next row updates only
 * the coordinates and the partial weight products of the dimensions that changed. The source
 * function is evaluated at the grid points in domain coordinates.
 *
 * @param context    Read-only data of the quadrature call (domain and per-dimension
 *                   node-generation functions, which provide the quadrature rules).
 * @param sourceFunc Source function.
 * @param tg         Tensor grid (at least one dimension).
 * @param scratch    Per-thread scratch space; reused across calls.
 * @return Quadrature estimate on the unit hypercube
 *         @f$[0,1]^d@f$ (the domain volume is multiplied in by the caller).
 */
double processTensorGrid(const misc::QuadratureContext& context, const SourceFunc& sourceFunc,
                         const TensorGrid& tg, misc::QuadratureScratch& scratch);

/**
 * @brief @ref processTensorGrid for a vector-valued source function: the same row loop with m
 * values per grid point.
 *
 * @param context    Read-only data of the quadrature call.
 * @param sourceFunc Vector-valued source function (m outputs).
 * @param tg         Tensor grid (at least one dimension).
 * @param scratch    Per-thread scratch space, constructed with m outputs; reused across calls.
 * @param out        Output: the m quadrature estimates on the unit hypercube (size m).
 */
void processTensorGrid(const misc::QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                       const TensorGrid& tg, misc::QuadratureScratch& scratch,
                       std::span<double> out);

/**
 * @brief Sets @p result to the sum over the tensor grids of @c context.sparseGrid of the
 * coefficient times the vector-valued quadrature estimate of the tensor grid (unit hypercube), in
 * parallel.
 *
 * @param context    Read-only data of the quadrature call.
 * @param sourceFunc Vector-valued source function (m outputs).
 * @param result     Output: the m combined estimates (size m).
 */
void combineTensorGrids(const misc::QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                        const std::span<double> result);

/**
 * @brief Prepares the row loop over @p tg: 1D quadrature weights (@ref getWeights), nodes mapped
 * onto the domain and iterated dimensions (@c tools::weighted_sum).
 *
 * @param context Read-only data of the quadrature call.
 * @param tg      Tensor grid (at least one dimension and one grid point).
 * @param scratch Per-thread scratch space.
 */
void prepareTensorGrid(const misc::QuadratureContext& context, const TensorGrid& tg,
                       misc::QuadratureScratch& scratch);

/**
 * @brief Writes the 1D quadrature weights of every dimension of @p tg to
 * @c scratch.rows.weights1D (per-dimension blocks back-to-back, in the order of the nodes).
 *
 * @param context Read-only data of the quadrature call (the node-generation function of every
 *                dimension provides the quadrature rule).
 * @param tg      Tensor grid.
 * @param scratch Scratch space; @c rows.weights1D and @c quadRules are filled in.
 */
void getWeights(const misc::QuadratureContext& context, const TensorGrid& tg,
                misc::QuadratureScratch& scratch);

/**
 * @brief Returns the quadrature rule of every dimension of @p tg (for its number of nodes).
 *
 * @param tg           Tensor grid.
 * @param nodeGenFuncs Node-generation function of every dimension.
 * @return The (non-owning) quadrature rule of every dimension.
 */
std::vector<QuadRule*> getQuadRulesForTensorGrid(const TensorGrid& tg,
                                                 const std::vector<NodeGenFunc*>& nodeGenFuncs);

/**
 * @brief @ref getQuadRulesForTensorGrid(const TensorGrid&, const std::vector<NodeGenFunc*>&)
 * writing into @p out (size: the dimension of @p tg).
 */
void getQuadRulesForTensorGrid(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                               std::vector<QuadRule*>& out);

/**
 * @brief Returns the quadrature rule of every dimension for @p gpCntPerDim nodes.
 *
 * @param gpCntPerDim  Number of nodes of every dimension.
 * @param nodeGenFuncs Node-generation function of every dimension.
 * @return The (non-owning) quadrature rule of every dimension.
 */
std::vector<QuadRule*> getQuadRulesForAllDims(std::span<const GPCntType> gpCntPerDim,
                                              const std::vector<NodeGenFunc*>& nodeGenFuncs);

/**
 * @brief @ref getQuadRulesForAllDims(std::span<const GPCntType>, const std::vector<NodeGenFunc*>&)
 * writing into @p out (size: the number of dimensions).
 */
void getQuadRulesForAllDims(std::span<const GPCntType> gpCntPerDim,
                            const std::vector<NodeGenFunc*>& nodeGenFuncs,
                            std::vector<QuadRule*>& out);

}  // namespace quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
