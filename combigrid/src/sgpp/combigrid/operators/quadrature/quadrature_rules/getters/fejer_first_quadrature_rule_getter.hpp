// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file fejer_first_quadrature_rule_getter.hpp
 * @brief Accessor for the shared instances of the quadrature rule of the first-kind Chebyshev
 * nodes (Fejér's first rule).
 */
#pragma once

#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) quadrature rule of the first-kind Chebyshev nodes
 * (@c quadrature_rules::FejerFirstQuadRule).
 * @param includeBoundary Whether the node set includes the boundary nodes 0 and 1 (the
 * interpolatory rule on all nodes) or not (Fejér's first rule).
 * @return Non-owning pointer to one of two singleton rule instances.
 */
QuadRule* getFejerFirstQuadRule(bool includeBoundary);

}  // namespace combigrid
}  // namespace sgpp
