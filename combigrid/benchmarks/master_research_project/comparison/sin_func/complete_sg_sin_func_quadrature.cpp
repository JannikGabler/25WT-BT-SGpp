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
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/benchmarking/benchmarker.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

inline double sinFunc(const sgpp::base::DataVector& x) {
  const size_t nDim = x.size();

  double result = 1.0;

  for (size_t dim = 0; dim < nDim; dim++) {
    result *= std::sin(3 * M_PI * x[dim]);
  }

  return std::abs(result);
}

inline double perform(const size_t nDim, const size_t maxLvl,
                      sgpp::combigrid::tools::Benchmarker<double>::BenchmarkerContext& ctx) {
  // Source func
  const SourceFunc sourceFunc(sinFunc);

  ctx.markCheckpoint("Creation Source func");

  // SG gen instr
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setBoundaryLevelOffset(1);
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

  ctx.markCheckpoint("Creation SG Gen. Instruction");

  // SG gen
  const SparseGrid sg(genInstr);

  ctx.markCheckpoint("SG Generation");

  // Quadrature
  const double result = quadrature(sourceFunc, sg);

  ctx.markCheckpoint("Quadrature");

  return result;
}

inline double compExactSolution(const size_t nDim) { return std::pow(2 / M_PI, nDim); }

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
  benchmarker.addCheckpoint("SG Generation");
  benchmarker.addCheckpoint("Quadrature");

  benchmarker.setAnalyticalResult(compExactSolution(nDim));

  benchmarker.run(
      [nDim, maxLvl](sgpp::combigrid::tools::Benchmarker<double>::BenchmarkerContext& ctx) {
        return perform(nDim, maxLvl, ctx);
      });

  benchmarker.printResults();
}

int main(int argc, char* argv[]) {
  if (argc != 7) {
    std::cerr << "Invalid number of arguments! Received " << (argc - 1) << " but requires 6."
              << std::endl;
    return 1;
  }

  size_t minWarmupRounds = 0;
  size_t minRounds = 0;
  double minWarmupTime = 0.0;
  double minRunTime = 0.0;
  size_t nDim = 0;
  size_t maxLvl = 0;

  try {
    minWarmupRounds = std::stoull(argv[1]);
    minRounds = std::stoull(argv[2]);
    minWarmupTime = std::stod(argv[3]);
    minRunTime = std::stod(argv[4]);
    nDim = std::stoull(argv[5]);
    maxLvl = std::stoull(argv[6]);
  } catch (const std::exception& e) {
    std::cerr << "Error while parsing arguments: " << e.what() << "\n";
    return 1;
  }

  startBenchmark(nDim, maxLvl, minWarmupRounds, minRounds, minWarmupTime, minRunTime);

  return 0;
}
