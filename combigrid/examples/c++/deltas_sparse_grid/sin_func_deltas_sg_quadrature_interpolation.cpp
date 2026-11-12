// TODO: Redo example

// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// Deltas sparse grid: quadrature, global and linear interpolation of
//   f(x) = prod_k cos(4 pi x_k) on [0, 1]^3
// on a complete sparse grid of maximum level 6 with Clenshaw-Curtis nodes, once on a SparseGrid
// and once on the DeltasSparseGrid of the same generation instruction. Both give the same results
// up to round-off; the DeltasSparseGrid operators evaluate the function once per grid point (or
// once per distinct corner for the linear interpolation) instead of once per tensor grid the point
// belongs to. A DeltasSparseGrid requires nested nodes (equidistant or Clenshaw-Curtis nodes with
// the doubling growth): first-type Chebyshev nodes make its constructor throw.

#include <atomic>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <span>
#include <stdexcept>

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::VectorSourceFunc;

/// Number of evaluations of func (the operators call it from several threads).
std::atomic<size_t> evaluationCount{0};

double func(const DataVector& point) {
  evaluationCount.fetch_add(1, std::memory_order_relaxed);
  double result = 1.0;
  for (size_t dim = 0; dim < point.size(); dim++) {
    result *= std::cos(4 * M_PI * point[dim]);
  }
  return result;
}

/// Runs @p op and returns the number of evaluations of func it made.
template <typename Op>
size_t countEvaluations(Op op) {
  evaluationCount = 0;
  op();
  return evaluationCount.load();
}

/// Prints and checks one pair of results (SparseGrid, DeltasSparseGrid).
void compare(const char* name, double sgResult, size_t sgEvals, double dsgResult, size_t dsgEvals) {
  std::cout << name << ": SparseGrid " << sgResult << " (" << sgEvals
            << " evaluations), DeltasSparseGrid " << dsgResult << " (" << dsgEvals
            << " evaluations), difference " << std::abs(sgResult - dsgResult) << "\n";
  if (std::abs(sgResult - dsgResult) > 1e-12 || dsgEvals > sgEvals) std::exit(EXIT_FAILURE);
}

int main() {
  const SourceFunc sourceFunc(func);

  CompleteSGGenInstr genInstr(6, 3);
  genInstr.setDomain({0, 1});
  genInstr.setNodeGenFunc(sgpp::combigrid::getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);

  const SparseGrid sg(genInstr);
  const DeltasSparseGrid dsg(genInstr);
  std::cout << "SparseGrid: " << sg.nTG() << " tensor grids, DeltasSparseGrid: " << dsg.nTG()
            << " delta tensor grids\n";

  double sgResult = 0.0, dsgResult = 0.0;
  size_t sgEvals = countEvaluations([&]() { sgResult = quadrature(sourceFunc, sg); });
  size_t dsgEvals = countEvaluations([&]() { dsgResult = quadrature(sourceFunc, dsg); });
  compare("quadrature", sgResult, sgEvals, dsgResult, dsgEvals);

  const DataVector point{0.9173, 0.13494, 0.4184};
  sgEvals = countEvaluations([&]() { sgResult = interpolate(sourceFunc, point, sg); });
  dsgEvals = countEvaluations([&]() { dsgResult = interpolate(sourceFunc, point, dsg); });
  compare("interpolate", sgResult, sgEvals, dsgResult, dsgEvals);

  sgEvals = countEvaluations([&]() { sgResult = interpolateLinear(sourceFunc, point, sg); });
  dsgEvals = countEvaluations([&]() { dsgResult = interpolateLinear(sourceFunc, point, dsg); });
  compare("interpolateLinear", sgResult, sgEvals, dsgResult, dsgEvals);

  // Vector-valued source functions: one evaluation per grid point for all m outputs.
  const VectorSourceFunc vectorSourceFunc(
      [](const DataVector& p, std::span<double> out) {
        out[0] = func(p);
        out[1] = p[0] * p[1] * p[2];
      },
      2);
  const DataVector integrals = quadrature(vectorSourceFunc, dsg);
  std::cout << "vector quadrature on the DeltasSparseGrid: " << integrals[0] << ", " << integrals[1]
            << " (exact 0, 0.125)\n";

  // Non-nested nodes cannot form a deltas sparse grid.
  genInstr.setNodeGenFunc(sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc());
  try {
    const DeltasSparseGrid invalid(genInstr);
    return EXIT_FAILURE;
  } catch (const std::invalid_argument& e) {
    std::cout << "first-type Chebyshev nodes: " << e.what() << "\n";
  }
  return 0;
}
