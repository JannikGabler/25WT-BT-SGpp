// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_refinement_target.hpp
 * @brief The quadrature as the target of a dimension-adaptive refinement.
 */
#pragma once

#include <cstddef>
#include <memory>
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

// Forward declaration
namespace misc {
class QuadWeightTable;
struct QuadratureScratch;
}  // namespace misc

/**
 * @brief Refines for the quadrature of a source function: the values of a tensor grid are its
 * quadrature estimate (m values for a @c VectorSourceFunc with m outputs), computed by the kernel
 * of @c quadrature; the finished combination is multiplied by the volume of the domain, so the
 * refinement's estimate equals @c quadrature on the refined grid.
 *
 * The source function is referenced (it must outlive the target) and may be cached
 * (@c CachedSourceFunc reuses the values of the points tensor grids share).
 */
class QuadratureRefinementTarget : public RefinementTarget {
 public:
  /// @name Constructors
  /// @{

  /// @brief Quadrature of the scalar function @p sourceFunc (1 value per tensor grid).
  explicit QuadratureRefinementTarget(const SourceFunc& sourceFunc);

  /// @brief Quadrature of the vector-valued function @p sourceFunc (m values per tensor grid).
  explicit QuadratureRefinementTarget(const VectorSourceFunc& sourceFunc);

  /// @}

  ~QuadratureRefinementTarget() override;

  size_t nValues() const override;

  void beginRefinementCall(const SparseGridSetup& setup, const NodeTable& unitNodeTable,
                           size_t nWorkers) override;

  void estimate(const TensorGridView& domainTG, const TensorGridView& unitTG, size_t worker,
                std::span<double> out) override;

  /// @brief Multiplies by the volume of the domain.
  void finishCombination(std::span<double> values) const override;

  void endRefinementCall() override;

 private:
  const SourceFunc* scalarFunc_ = nullptr;              ///< Scalar function, or nullptr.
  const VectorSourceFunc* vectorFunc_ = nullptr;        ///< Vector-valued function, or nullptr.
  double volume_ = 1.0;                                 ///< Volume of the domain of the call.
  std::unique_ptr<misc::QuadWeightTable> weightTable_;  ///< 1D weights of the call.
  std::vector<misc::QuadratureScratch> scratches_;      ///< Per worker.
};

}  // namespace combigrid
}  // namespace sgpp
