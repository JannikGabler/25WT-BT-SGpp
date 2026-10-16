/**
 * @file simpson_quadrature_rule.hpp
 * @brief 1D composite Simpson quadrature on @f$[0,1]@f$.
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/tools/hashing/fnv_1a_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>

namespace sgpp {
namespace combigrid {

namespace quadrature_rules {

/**
 * @brief Composite Simpson quadrature on equidistant nodes in @f$[0,1]@f$.
 *
 * Requires an odd number of nodes (at least 3). The weight pattern is
 * @f$h/3, 4h/3, 2h/3, 4h/3, \ldots, 2h/3, 4h/3, h/3@f$ with
 * @f$h = 1/(\mathrm{nNodes}-1)@f$.
 */
class SimpsonQuadRule : public QuadRule {
 public:
  /// @brief Constructs the rule with a deterministic id derived from its name.
  SimpsonQuadRule();

  /// @copydoc QuadRule::genWeights
  base::DataVector genWeights(const GPCntType nNodes) const override;

  /**
   * @copydoc QuadRule::genWeightsInplace
   * @throws std::invalid_argument if @p nNodes is even.
   */
  void genWeightsInplace(const std::span<double> out) const override;
};

}  // namespace quadrature_rules

}  // namespace combigrid
}  // namespace sgpp