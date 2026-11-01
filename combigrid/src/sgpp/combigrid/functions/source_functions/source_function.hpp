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
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/tools/concepts/strict_subclass_of.hpp>
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
 * Every operator calls @ref finishOperatorCall once at its end; a @c CachedSourceFunc overrides
 * it to notify its cache. A plain source function evaluates the wrapped callable directly.
 */
class SourceFunc {
 public:
  /// @name Constructors
  /// @{

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

  /// @}

  /// @name Copy, move and destruction
  /// @{

#ifndef SWIG
  SourceFunc(const SourceFunc&) = default;
  SourceFunc(SourceFunc&&) = default;
  SourceFunc& operator=(const SourceFunc&) = default;
  SourceFunc& operator=(SourceFunc&&) = default;

  /**
   * @brief Copying or moving an object of a subclass (e.g. @c CachedSourceFunc) into a
   * @c SourceFunc is not allowed: the copy would keep the subclass's callable but lose its
   * overrides, e.g. @c finishOperatorCall (slicing). Pass such objects by reference. Not
   * explicit, so that it also catches copy-initialization (@c SourceFunc f = cached;).
   */
  template <tools::StrictSubclassOf<SourceFunc> Subclass>
  SourceFunc(Subclass&& other) = delete;  // NOLINT(runtime/explicit)

  /// @brief Assigning an object of a subclass is not allowed either (slicing, see above).
  template <tools::StrictSubclassOf<SourceFunc> Subclass>
  SourceFunc& operator=(Subclass&& other) = delete;

  // Hidden from SWIG as well: the bindings replace the destructor by an %extend destructor
  // (SourceFunc.i / VectorSourceFunc.i), which a declared destructor would shadow (warning 302).
  virtual ~SourceFunc() = default;
#endif

  /// @}

  /// @name Evaluate operations
  /// @{

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

  /// @}

  /// @name Bookkeeping
  /// @{

  /**
   * @brief End-of-operator hook: every operator calls it once at its end (after its parallel
   * region). Does nothing for a plain source function; the cached functions override it to
   * notify their cache (one virtual call per operator call, never per evaluation).
   * @note The operators call it through a reference; copying a subclass object into the base
   * class (which would lose the override) does not compile.
   */
  virtual void finishOperatorCall() const;

  /// @}

 private:
  std::function<double(const base::DataVector&)> func;  ///< Wrapped user callable.
};

}  // namespace combigrid
}  // namespace sgpp
