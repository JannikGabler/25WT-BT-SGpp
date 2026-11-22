// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/fejer_first_quadrature_rule.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/fejer_first_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>

namespace sgpp {
namespace combigrid {

QuadRule* getFejerFirstQuadRule(const bool includeBoundary) {
  static quadrature_rules::FejerFirstQuadRule withoutBoundary(false);
  static quadrature_rules::FejerFirstQuadRule withBoundary(true);

  return includeBoundary ? &withBoundary : &withoutBoundary;
}

}  // namespace combigrid
}  // namespace sgpp
