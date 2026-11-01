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
#include <sgpp/combigrid/tools/concepts/strict_subclass_of.hpp>
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
 *
 * Like @c SourceFunc, it may carry an end-of-operator hook (@c CachedVectorSourceFunc).
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

  /// @name Copy, move and destruction
  /// @{

#ifndef SWIG
  VectorSourceFunc(const VectorSourceFunc&) = default;
  VectorSourceFunc(VectorSourceFunc&&) = default;
  VectorSourceFunc& operator=(const VectorSourceFunc&) = default;
  VectorSourceFunc& operator=(VectorSourceFunc&&) = default;

  /**
   * @brief Copying or moving an object of a subclass (e.g. @c CachedVectorSourceFunc) into a
   * @c VectorSourceFunc is not allowed: the copy would keep the subclass's callable but lose its
   * overrides, e.g. @c finishOperatorCall (slicing). Pass such objects by reference. Not
   * explicit, so that it also catches copy-initialization (@c VectorSourceFunc f = cached;).
   */
  template <tools::StrictSubclassOf<VectorSourceFunc> Subclass>
  VectorSourceFunc(Subclass&& other) = delete;  // NOLINT(runtime/explicit)

  /// @brief Assigning an object of a subclass is not allowed either (slicing, see above).
  template <tools::StrictSubclassOf<VectorSourceFunc> Subclass>
  VectorSourceFunc& operator=(Subclass&& other) = delete;

  // Hidden from SWIG as well: the bindings replace the destructor by an %extend destructor
  // (SourceFunc.i / VectorSourceFunc.i), which a declared destructor would shadow (warning 302).
  virtual ~VectorSourceFunc() = default;
#endif

  /// @}

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
  /// Wrapped user callable.
  std::function<void(const base::DataVector&, std::span<double>)> func;
  size_t nOutputs_;  ///< Number of outputs.
};

}  // namespace combigrid
}  // namespace sgpp
