/**
 * TODO: Document
 */
#pragma once

#include <omp.h>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/benchmarking/benchmarker.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace benchmarks {
namespace dsg_quadrature_benchmark {

template <size_t nDim>
inline double sinFunc(const sgpp::base::DataVector& x) {
  double result = 1;

  for (size_t i = 0; i < nDim; i++) {
    result *= std::sin(x[i]);
  }

  return result;
}

template <LvlType maxLvl, size_t nDim>
double perform(sgpp::combigrid::tools::Benchmarker<double>::BenchmarkerContext& ctx) {
  // Source func
  const SourceFunc sourceFunc(sinFunc<nDim>);

  ctx.mark_checkpoint("Creation Source func");

  // SG gen instr
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());

  ctx.mark_checkpoint("Creation SG Gen. Instruction");

  // SG gen
  const DeltasSparseGrid sg(genInstr);

  ctx.mark_checkpoint("DSG Generation");

  // Quadrature
  const double result = quadrature(sourceFunc, sg);

  ctx.mark_checkpoint("DSG Quadrature");

  return result;
}

double compExactSolution(const size_t nDim) { return std::pow(1 - std::cos(1), nDim); }

template <LvlType maxLvl, size_t nDim>
void run(const size_t nWarmupRuns, const size_t nRuns) {
  tools::Benchmarker<double> benchmarker;
  benchmarker.set_warmups(nWarmupRuns);
  benchmarker.set_runs(nRuns);
  benchmarker.enable_progress(true);

  benchmarker.add_checkpoint("Creation Source func");
  benchmarker.add_checkpoint("Creation SG Gen. Instruction");
  benchmarker.add_checkpoint("DSG Generation");
  benchmarker.add_checkpoint("DSG Quadrature");

  benchmarker.setAnalyticalResult(compExactSolution(nDim));

  benchmarker.run(perform<maxLvl, nDim>);

  benchmarker.print_results();
}

}  // namespace dsg_quadrature_benchmark
}  // namespace benchmarks

}  // namespace combigrid
}  // namespace sgpp
