/**
 * @file node_generation_function.hpp
 * @brief Abstract interface for 1D node-generation rules used to construct
 * tensor grids.
 */
#pragma once

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Abstract base class for 1D node-generation rules.
 *
 * Each subclass produces a sequence of @f$n@f$ nodes in the unit interval
 * @f$[0,1]@f$. Implementations differ both in the node distribution
 * (equidistant, Chebyshev, Clenshaw-Curtis, ...) and in their growth
 * behavior with respect to @f$n@f$ (nested vs. non-nested families).
 *
 * A node-generation function also exposes the matching 1D quadrature rule
 * and 1D interpolation method, so that downstream operators can integrate
 * or interpolate consistently.
 */
class NodeGenFunc {
 public:
  /**
   * @brief Constructs the node-generation function with a unique numeric id.
   * @param id Identifier used for caching and equality.
   */
  NodeGenFunc(const uint64_t id) : id_(id) {}

  virtual ~NodeGenFunc() = default;

  /**
   * @brief Generates @p nNodes nodes (with or without boundary nodes).
   *
   * Dispatches to @ref genNodesWithBoundary or @ref genNodesWithoutBoundary
   * depending on @p includeBoundary.
   *
   * @param nNodes          Number of inner (resp. inner+boundary) nodes.
   * @param out             Output vector (filled in starting at @p startIdx).
   * @param includeBoundary Whether to include the @c 0 / @c 1 boundary nodes.
   * @param startIdx        Offset at which to start writing into @p out.
   */
  void genNodes(GPCntType nNodes, base::DataVector& out, bool includeBoundary,
                size_t startIdx = 0) const;

  /**
   * @brief Out-of-place variant of @ref genNodes.
   * @param nNodes          Number of nodes.
   * @param includeBoundary Whether to include boundary nodes.
   * @return Newly allocated node vector.
   * @note Allocates memory; avoid in hot loops.
   */
  base::DataVector genNodes(GPCntType nNodes, bool includeBoundary) const;

  /**
   * @brief Generates the node sequence with boundary nodes (default
   * implementation: prepends @c 0, appends @c 1, fills the interior via
   * @ref genNodesWithoutBoundary).
   *
   * @param nNodes   Total number of nodes (including boundaries).
   * @param out      Output vector.
   * @param startIdx Output offset.
   */
  void genNodesWithBoundary(GPCntType nNodes, base::DataVector& out, size_t startIdx = 0) const;

  /**
   * @brief Out-of-place variant of @ref genNodesWithBoundary.
   * @param nNodes Number of nodes.
   * @return Newly allocated node vector.
   */
  base::DataVector genNodesWithBoundary(GPCntType nNodes) const;

  /**
   * @brief Generates the interior node sequence (no boundary nodes).
   *
   * Subclasses must provide the actual node distribution here.
   *
   * @param nNodes   Number of interior nodes.
   * @param out      Output vector.
   * @param startIdx Output offset.
   */
  virtual void genNodesWithoutBoundary(GPCntType nNodes, base::DataVector& out,
                                       size_t startIdx = 0) const = 0;

  /**
   * @brief Out-of-place variant of @ref genNodesWithoutBoundary.
   * @param nNodes Number of nodes.
   * @return Newly allocated node vector.
   */
  base::DataVector genNodesWithoutBoundary(GPCntType nNodes) const;

  /******
  Getters
  ******/
  /**
   * @brief Returns the matching 1D quadrature rule for this node family.
   * @param nNodes Number of nodes for which the rule will be used (some
   * subclasses dispatch on @p nNodes).
   * @return Non-owning pointer to the quadrature rule.
   */
  virtual QuadRule* getQuadRule(GPCntType nNodes) const = 0;

  /**
   * @brief Returns the matching 1D interpolation method for this node family.
   * @param nNodes Number of nodes for which the method will be used.
   * @return Non-owning pointer to the interpolation method.
   */
  virtual InterpolationMethod* getInterpolationMethod(GPCntType nNodes) const = 0;

  /// @brief Returns the unique identifier.
  uint64_t id() const { return id_; }

  /********
  Operators
  ********/
  /**
   * @brief Type-safe equality between node-generation functions.
   * @param other Other instance.
   * @return @c true if @c *this represents the same node family as @p other.
   */
  virtual bool operator==(const NodeGenFunc& other) const = 0;

 private:
  const uint64_t id_;  ///< Unique identifier (used for caching).
};

}  // namespace combigrid
}  // namespace sgpp
