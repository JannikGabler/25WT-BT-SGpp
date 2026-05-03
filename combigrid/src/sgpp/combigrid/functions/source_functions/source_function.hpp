/**
 * @file source_function.hpp
 * @brief Wrapper for user-supplied black-box functions
 * @f$f: \mathbb{R}^d \to \mathbb{R}@f$.
 */
#pragma once

#include <functional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Adapter around a user-supplied function
 * @f$f : \mathbb{R}^d \to \mathbb{R}@f$ used by all sparse-grid operators.
 *
 * Beyond plain forwarding, the wrapper can transform a normalized
 * @f$[0,1]^d@f$ point into the actual problem domain before evaluating
 * (and may cache results, see @c constants::source_func::USE_CACHE).
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
   * @note This overload bypasses the cache.
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

 private:
  std::function<double(const base::DataVector&)> func;  ///< Wrapped user callable.
  /// @brief Cache for normalized-point evaluations (only used if enabled).
  mutable misc::SourceFunctionCache cache;
};

}  // namespace combigrid
}  // namespace sgpp
