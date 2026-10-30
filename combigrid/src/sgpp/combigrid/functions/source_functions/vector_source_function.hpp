// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file vector_source_function.hpp
 * @brief Wrapper for user-supplied vector-valued black-box functions
 * @f$f: \mathbb{R}^d \to \mathbb{R}^m@f$.
 */
#pragma once

#include <functional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>

namespace sgpp {
namespace combigrid {

/**
 * @brief Adapter around a user-supplied function @f$f : \mathbb{R}^d \to \mathbb{R}^m@f$ with a
 * number of outputs @f$m@f$ fixed at construction; the vector-valued counterpart of
 * @c SourceFunc.
 *
 * The operators have overloads for @c VectorSourceFunc that evaluate the function once per grid
 * point for all @f$m@f$ outputs. Output @f$k@f$ of such an operator equals the scalar operator
 * applied to output @f$k@f$ (as a @c SourceFunc) up to round-off.
 */
class VectorSourceFunc {
 public:
  /**
   * @brief Wraps the provided callable (copy).
   * @param func     Callable writing the @p nOutputs values at a point in problem coordinates to
   *                 its second argument (of size @p nOutputs).
   * @param nOutputs Number of outputs @f$m@f$.
   * @throws base::data_exception if @p nOutputs is 0.
   */
  VectorSourceFunc(const std::function<void(const base::DataVector&, std::span<double>)>& func,
                   size_t nOutputs);

  /**
   * @brief Wraps the provided callable (move).
   * @param func     Callable writing the @p nOutputs values at a point in problem coordinates to
   *                 its second argument (of size @p nOutputs).
   * @param nOutputs Number of outputs @f$m@f$.
   * @throws base::data_exception if @p nOutputs is 0.
   */
  VectorSourceFunc(std::function<void(const base::DataVector&, std::span<double>)>&& func,
                   size_t nOutputs);

  /**
   * @brief Returns the number of outputs @f$m@f$.
   * @return Number of outputs.
   */
  size_t nOutputs() const;

  /**
   * @brief Evaluates the source function at @p point in problem coordinates.
   *
   * @note The size of @p out is not checked in optimized builds (no check per evaluation; an
   * assertion in debug builds).
   *
   * @param point Evaluation point in problem coordinates.
   * @param out   Function values (size @ref nOutputs).
   */
  void evaluate(const base::DataVector& point, std::span<double> out) const;

  /**
   * @brief Evaluates the source function on a normalized point in @f$[0,1]^d@f$,
   * mapping it onto @p area before forwarding to the user callable.
   *
   * @warning To avoid memory allocations, @p point is overwritten in place
   * with its image in problem coordinates.
   *
   * @param point Normalized point in @f$[0,1]^d@f$ (modified in place).
   * @param area  Hyper-rectangular target domain to map onto.
   * @param out   Function values at the mapped point (size @ref nOutputs).
   */
  void evaluateNormalizedInPlace(base::DataVector& point, const HyperCubeArea& area,
                                 std::span<double> out) const;

 private:
  /// Wrapped user callable.
  std::function<void(const base::DataVector&, std::span<double>)> func;
  size_t nOutputs_;  ///< Number of outputs.
};

}  // namespace combigrid
}  // namespace sgpp
