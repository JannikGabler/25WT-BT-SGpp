// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Textbook operators: quadrature, global and linear interpolation of
//   f(x) = prod_k cos(4 pi x_k) on [0, 1]^3
// on a complete sparse grid of maximum level 6 with Clenshaw-Curtis nodes, once with the textbook
// operators (textbookQuadrature, textbookInterpolate, textbookInterpolateLinear: a plain loop over
// the tensor grids, each solved on its own; written to be read) and once with the optimized
// operators (quadrature, interpolate, interpolateLinear). Both give the same results up to
// round-off and evaluate the function at the same points; the optimized operators are faster and
// run in parallel. The textbook operators also take a VectorSourceFunc (one evaluation per grid
// point for all outputs).

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_global_interpolation.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <span>

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::VectorSourceFunc;

double func(const DataVector& point) {
  double result = 1.0;
  for (size_t dim = 0; dim < point.size(); dim++) {
    result *= std::cos(4 * M_PI * point[dim]);
  }
  return result;
}

/// Runs @p op and returns its runtime in milliseconds.
template <typename Op>
double milliseconds(Op op) {
  const auto start = std::chrono::steady_clock::now();
  op();
  return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start)
      .count();
}

/// Prints and checks one pair of results (textbook, optimized).
void compare(const char* name, double textbook, double textbookMs, double optimized,
             double optimizedMs) {
  std::cout << name << ": textbook " << textbook << " (" << textbookMs << " ms), optimized "
            << optimized << " (" << optimizedMs << " ms), difference "
            << std::abs(textbook - optimized) << "\n";
  if (std::abs(textbook - optimized) > 1e-12) std::exit(EXIT_FAILURE);
}

int main() {
  const SourceFunc sourceFunc(func);

  CompleteSGGenInstr genInstr(6, 3);
  genInstr.setDomain({0, 1});
  genInstr.setNodeGenFunc(sgpp::combigrid::getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);
  const SparseGrid sg(genInstr);

  double textbook = 0.0, optimized = 0.0;
  double textbookMs = milliseconds([&]() { textbook = textbookQuadrature(sourceFunc, sg); });
  double optimizedMs = milliseconds([&]() { optimized = quadrature(sourceFunc, sg); });
  compare("quadrature (exact 0)", textbook, textbookMs, optimized, optimizedMs);

  const DataVector point{0.9173, 0.13494, 0.4184};
  std::cout << "f(point) = " << func(point) << "\n";
  textbookMs = milliseconds([&]() { textbook = textbookInterpolate(sourceFunc, point, sg); });
  optimizedMs = milliseconds([&]() { optimized = interpolate(sourceFunc, point, sg); });
  compare("interpolate", textbook, textbookMs, optimized, optimizedMs);

  textbookMs = milliseconds([&]() { textbook = textbookInterpolateLinear(sourceFunc, point, sg); });
  optimizedMs = milliseconds([&]() { optimized = interpolateLinear(sourceFunc, point, sg); });
  compare("interpolateLinear", textbook, textbookMs, optimized, optimizedMs);

  // Vector-valued source function: output 1 is x_0 x_1 x_2 (exact integral 1/8)
  const VectorSourceFunc vectorSourceFunc(
      [](const DataVector& p, std::span<double> out) {
        out[0] = func(p);
        out[1] = p[0] * p[1] * p[2];
      },
      2);
  const DataVector integrals = textbookQuadrature(vectorSourceFunc, sg);
  std::cout << "vector textbook quadrature: " << integrals[0] << ", " << integrals[1]
            << " (exact 0, 0.125)\n";
  if (std::abs(integrals[1] - 0.125) > 1e-12) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
