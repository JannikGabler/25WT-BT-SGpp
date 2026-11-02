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

inline double sinFunc(const sgpp::base::DataVector& x) {
  const size_t nDim = x.getSize();

  double result = 1;

  for (size_t i = 0; i < nDim; i++) {
    result *= std::sin(x[i]);
  }

  return result;
}

inline double perform(const size_t nDim, const size_t maxLvl,
                      sgpp::combigrid::tools::Benchmarker<double>::BenchmarkerContext& ctx) {
  // Source func
  const SourceFunc sourceFunc(sinFunc);

  ctx.markCheckpoint("Creation Source func");

  // SG gen instr
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());

  ctx.markCheckpoint("Creation SG Gen. Instruction");

  // SG gen
  const DeltasSparseGrid sg(genInstr);

  ctx.markCheckpoint("DSG Generation");

  // Quadrature
  const double result = quadrature(sourceFunc, sg);

  ctx.markCheckpoint("DSG Quadrature");

  return result;
}

inline double compExactSolution(const size_t nDim) { return std::pow(1 - std::cos(1), nDim); }

inline void startBenchmark(const size_t nDim, const size_t maxLvl, const size_t minWarmupRounds,
                           const size_t minRounds, const double minWarmupTime,
                           const double minRunTime) {
  tools::Benchmarker<double> benchmarker;
  benchmarker.setWarmupRounds(minWarmupRounds);
  benchmarker.setRounds(minRounds);
  benchmarker.setMinWarmupTime(minWarmupTime);
  benchmarker.setMinRunTime(minRunTime);
  benchmarker.enableProgress(true);

  benchmarker.addCheckpoint("Creation Source func");
  benchmarker.addCheckpoint("Creation SG Gen. Instruction");
  benchmarker.addCheckpoint("DSG Generation");
  benchmarker.addCheckpoint("DSG Quadrature");

  benchmarker.setAnalyticalResult(compExactSolution(nDim));

  benchmarker.run(
      [nDim, maxLvl](sgpp::combigrid::tools::Benchmarker<double>::BenchmarkerContext& ctx) {
        return perform(nDim, maxLvl, ctx);
      });

  benchmarker.printResults();
}

}  // namespace dsg_quadrature_benchmark
}  // namespace benchmarks

}  // namespace combigrid
}  // namespace sgpp
