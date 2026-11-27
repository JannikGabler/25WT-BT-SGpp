// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file linear_interpolation_refinement_target.hpp
 * @brief The multilinear interpolation at given points as the target of a dimension-adaptive
 * refinement.
 */
#pragma once

#include <cstddef>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace misc {
struct LinInterpScratch;
}  // namespace misc

/**
 * @brief Refines for the multilinear interpolation of a source function at given points (indicator
 * points): the values of a tensor grid are its interpolants at the P points (P · m values for a
 * @c VectorSourceFunc with m outputs, point-major), computed by the kernel of @c interpolateLinear;
 * the refinement's estimate equals @c interpolateLinear at these points on the refined grid.
 *
 * The kernel evaluates a tensor grid once per point, so a tensor grid costs P times its grid
 * points in evaluations; a @c CachedSourceFunc evaluates every grid point once. The source
 * function is referenced (it must outlive the target).
 */
class LinearInterpolationRefinementTarget : public RefinementTarget {
 public:
  /**
   * @brief Interpolation of the scalar function @p sourceFunc at the rows of @p points (on the
   * domain; one point per row).
   */
  LinearInterpolationRefinementTarget(const SourceFunc& sourceFunc, const base::DataMatrix& points);
  /// @brief As above for the vector-valued function @p sourceFunc.
  LinearInterpolationRefinementTarget(const VectorSourceFunc& sourceFunc,
                                      const base::DataMatrix& points);
  ~LinearInterpolationRefinementTarget() override;

  size_t nValues() const override;
  void beginRefinementCall(const SparseGridSetup& setup, const NodeTable& unitNodeTable,
                           size_t nWorkers) override;
  void estimate(const TensorGridView& domainTG, const TensorGridView& unitTG, size_t worker,
                std::span<double> out) override;
  void endRefinementCall() override;

 private:
  const SourceFunc* scalarFunc_ = nullptr;         ///< Scalar function, or nullptr.
  const VectorSourceFunc* vectorFunc_ = nullptr;   ///< Vector-valued function, or nullptr.
  std::vector<base::DataVector> points_;           ///< The points on the domain.
  std::vector<base::DataVector> normPoints_;       ///< The points on the unit cube (per call).
  SparseGridSetup setup_;                          ///< Setup of the grid of the call.
  std::vector<misc::LinInterpScratch> scratches_;  ///< Per worker.
};

}  // namespace combigrid
}  // namespace sgpp
