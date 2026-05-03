/**
 * @file interpolation_method.hpp
 * @brief Abstract interface for 1D interpolation methods used by the global
 * tensor-product interpolation operator.
 */
#pragma once

#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Abstract base class for 1D interpolation methods (e.g.
 * Lagrange, barycentric Lagrange).
 *
 * A concrete subclass implements @ref interpolate, which evaluates the
 * Lagrange (or equivalent) interpolant defined by @c (nodes, values) at
 * the requested position. A unique numeric id identifies equivalent
 * methods so that the higher-level cache structures can deduplicate.
 */
class InterpolationMethod {
 public:
  /**
   * @brief Constructs the method with the given identifier.
   * @param id Unique numeric id for this method.
   */
  InterpolationMethod(uint64_t id);

  virtual ~InterpolationMethod() = default;

  /**
   * @brief Evaluates the 1D interpolant at @p pos.
   *
   * @param pos    Evaluation position.
   * @param nodes  1D node positions (size @c n).
   * @param values Values at the corresponding nodes (size @c n).
   * @return Interpolated value at @p pos.
   */
  virtual double interpolate(double pos, const std::vector<double>& nodes,
                             const std::vector<double>& values) const = 0;

  /// @brief Returns the unique identifier of this method.
  uint64_t id() const;

 protected:
  /**
   * @brief Detects trivial cases where the result follows immediately from
   * the inputs.
   *
   * Currently handles two cases:
   * - @p nodes contains a single entry (the result is that entry's value);
   * - @p pos coincides (up to floating-point tolerance) with one of the
   *   @p nodes (the result is the corresponding value).
   *
   * @param pos    Evaluation position.
   * @param nodes  1D node positions.
   * @param values Values at the nodes.
   * @param out    Out parameter receiving the trivial result, if any.
   * @return @c true if a trivial result was written into @p out.
   */
  bool hasTrivialResult(double pos, const std::vector<double>& nodes,
                        const std::vector<double>& values, double& out) const;

 private:
  const uint64_t id_;  ///< Unique identifier.
};

}  // namespace combigrid
}  // namespace sgpp
