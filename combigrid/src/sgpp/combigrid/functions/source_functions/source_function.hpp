// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file source_function.hpp
 * @brief Wrapper for user-supplied black-box functions
 * @f$f: \mathbb{R}^d \to \mathbb{R}@f$.
 */
#pragma once

#include <functional>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function_hook.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Adapter around a user-supplied function
 * @f$f : \mathbb{R}^d \to \mathbb{R}@f$ used by all sparse-grid operators.
 *
 * Beyond plain forwarding, the wrapper can transform a normalized
 * @f$[0,1]^d@f$ point into the actual problem domain before evaluating.
 *
 * A source function may carry a hook (@c SourceFuncHook) that every operator notifies once at
 * the end of a call; a @c CachedSourceFunc attaches its cache this way. A plain source function
 * has none and evaluates the wrapped callable directly.
 */
class SourceFunc {
 public:
  /**
   * @brief Wraps the provided callable (copy).
   * @param func Callable mapping a point in problem coordinates to a scalar.
   */
  SourceFunc(const std::function<double(const base::DataVector&)>& func);

  /**
   * @brief Wraps the provided callable (move).
   * @param func Callable mapping a point in problem coordinates to a scalar.
   */
  SourceFunc(std::function<double(const base::DataVector&)>&& func);

  /**
   * @brief Evaluates the source function at @p point in problem coordinates.
   * @param point Evaluation point in problem coordinates.
   * @return Function value.
   */
  double evaluate(const base::DataVector& point) const;

  /**
   * @brief Evaluates the source function on a normalized point in @f$[0,1]^d@f$,
   * mapping it onto @p area before forwarding to the user callable.
   *
   * @warning To avoid memory allocations, @p point is overwritten in place
   * with its image in problem coordinates.
   *
   * @param point Normalized point in @f$[0,1]^d@f$ (modified in place).
   * @param area  Hyper-rectangular target domain to map onto.
   * @return Function value at the mapped point.
   */
  double evaluateNormalizedInPlace(base::DataVector& point, const HyperCubeArea& area) const;

  /**
   * @brief Out-of-place variant of @ref evaluateNormalizedInPlace.
   *
   * Allocates a temporary buffer instead of mutating @p point.
   *
   * @param point Normalized point in @f$[0,1]^d@f$.
   * @param area  Hyper-rectangular target domain.
   * @return Function value at the mapped point.
   * @note Allocates memory; avoid in hot loops.
   */
  double evaluateNormalizedOutOfPlace(const base::DataVector& point,
                                      const HyperCubeArea& area) const;

  /**
   * @brief Notifies the hook of the source function, if any, that an operator call has
   * finished. Every operator calls it once at its end.
   */
  void finishOperatorCall() const;

 protected:
  /**
   * @brief Wraps the provided callable and attaches @p hook (e.g. @c CachedSourceFunc).
   * @param func Callable mapping a point in problem coordinates to a scalar.
   * @param hook Hook notified at the end of every operator call.
   */
  SourceFunc(std::function<double(const base::DataVector&)> func,
             std::shared_ptr<SourceFuncHook> hook);

  /**
   * @brief Returns the hook (nullptr for a plain source function).
   * @return Hook.
   */
  const std::shared_ptr<SourceFuncHook>& hook() const;

 private:
  std::function<double(const base::DataVector&)> func;  ///< Wrapped user callable.
  std::shared_ptr<SourceFuncHook> hook_;                ///< End-of-operator hook (or nullptr).
};

}  // namespace combigrid
}  // namespace sgpp
