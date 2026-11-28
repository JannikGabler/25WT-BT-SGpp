// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file barycentric_formula_getter.hpp
 * @brief Accessor for the shared generic barycentric-formula instance.
 */
#pragma once

#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Returns a pointer to the (process-wide) generic barycentric Lagrange
 * formula. The formula derives its weights from the nodes it is given, so one
 * instance serves every node family.
 * @return Non-owning pointer to a singleton method instance.
 */
InterpolationMethod* getBarycentricFormula();

}  // namespace combigrid
}  // namespace sgpp
