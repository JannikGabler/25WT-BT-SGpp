// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#pragma once

#include <cassert>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
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
 * Maps the 1D nodes of @p tg onto the domain once and iterates over the grid points row by row.
 * The row dimension (innermost) is the first dimension with more than one node; dimensions with
 * one node are not iterated (constant coordinate and weight factor). A step to the next row
 * updates only the coordinates and the partial weight products of the dimensions that changed.
 * Each row is summed with the 1D weights of the row dimension and multiplied once by the product
 * of the weights of the other dimensions. The source function is evaluated at the grid points in
 * domain coordinates.
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
 * TODO: Document
 */
void combineTensorGrids(const misc::QuadratureContext& context, const VectorSourceFunc& sourceFunc,
                        const std::span<double> result);

/**
 * TODO: Document
 */
void prepareTensorGrid(const misc::QuadratureContext& context, const TensorGrid& tg,
                       misc::QuadratureScratch& scratch);

/**
 * TODO: Document (update)
 * @brief Populates @p scratch.weights with the 1D quadrature weights for
 * every dimension of @p tg, concatenated dimension by dimension.
 *
 * @param tg            Tensor grid.
 * @param nodeGenFuncs  Per-dimension node-generation functions (each one
 *                      provides the matching quadrature rule).
 * @param scratch       Scratch space; @c weights and @c quadRules are filled in.
 */
void getWeights(const misc::QuadratureContext& context, const TensorGrid& tg,
                misc::QuadratureScratch& scratch);

/**
 * TODO: Document
 */
void mapNodesOntoDomain(const misc::QuadratureContext& context, const TensorGrid& tg,
                        misc::QuadratureScratch& scratch);

/**
 * TODO: Document
 */
void setUpIterDims(const TensorGrid& tg, misc::QuadratureScratch& scratch);

/**
 * TODO: Document
 */
size_t findRowDim(const GPMI& nGPPerDim);

/**
 * TODO: Document
 */
void updateWeightProducts(const double* const* const weightsOfIterDim, const GPCntType* const gpMI,
                          double* const weightProducts, size_t j);

template <typename ValueType>
typename ValueType::Acc sumOverRows(const ValueType values, misc::QuadratureScratch& scratch);

/**
 * TODO: Document
 */
std::vector<QuadRule*> getQuadRulesForTensorGrid(const TensorGrid& tg,
                                                 const std::vector<NodeGenFunc*>& nodeGenFuncs);

/**
TODO: Document
*/
void getQuadRulesForTensorGrid(const TensorGrid& tg, const std::vector<NodeGenFunc*>& nodeGenFuncs,
                               std::vector<QuadRule*>& out);

/**
 * TODO: Document
 */
std::vector<QuadRule*> getQuadRulesForAllDims(std::span<const GPCntType> gpCntPerDim,
                                              const std::vector<NodeGenFunc*>& nodeGenFuncs);

/**
 * TODO: Document
 */
void getQuadRulesForAllDims(std::span<const GPCntType> gpCntPerDim,
                            const std::vector<NodeGenFunc*>& nodeGenFuncs,
                            std::vector<QuadRule*>& out);

}  // namespace quadrature
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp