// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/operators/global_interpolation/methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

InterpolationMethod* getBarycentricFormula() {
  static global_interpolation::BarycentricFormula instance;
  return &instance;
}

}  // namespace combigrid
}  // namespace sgpp
