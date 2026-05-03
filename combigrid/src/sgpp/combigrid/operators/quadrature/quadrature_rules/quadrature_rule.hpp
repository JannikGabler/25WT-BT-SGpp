/**
 * @file quadrature_rule.hpp
 * @brief Abstract interface for 1D quadrature rules.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Abstract base class for 1D quadrature rules.
 *
 * Concrete subclasses (Clenshaw-Curtis, trapezoidal, Simpson, ...) compute
 * the weights for a given number of nodes. A unique numeric id is used
 * to deduplicate equivalent rules in the higher-level cache structures.
 */
class QuadRule {
 public:
  /**
   * @brief Constructs a rule with the given identifier.
   * @param id Unique numeric id for this rule.
   */
  QuadRule(const uint64_t id) : id_(id) {}

  virtual ~QuadRule() = default;

  /**
   * @brief Returns the quadrature weights for @p nNodes nodes.
   * @param nNodes Number of 1D nodes.
   * @return Newly allocated vector of @p nNodes weights.
   */
  virtual base::DataVector getWeights(GPCntType nNodes) const = 0;

  /**
   * @brief Writes the quadrature weights for @p nNodes nodes into @p out.
   *
   * The weights are written into @p out starting at @p startIdx; @p out
   * must already have @c size() @c >= startIdx + nNodes.
   *
   * @param nNodes   Number of 1D nodes.
   * @param out      Output vector.
   * @param startIdx Offset at which to start writing (default @c 0).
   */
  virtual void genWeightsInplace(GPCntType nNodes, base::DataVector& out,
                                 size_t startIdx = 0) const = 0;

  /// @brief Returns the unique identifier of this quadrature rule.
  uint64_t id() const { return id_; }

 private:
  const uint64_t id_;  ///< Unique identifier (used for deduplication).
};

}  // namespace combigrid
}  // namespace sgpp
