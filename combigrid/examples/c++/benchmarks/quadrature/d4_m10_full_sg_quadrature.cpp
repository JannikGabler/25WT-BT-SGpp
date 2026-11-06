#include <omp.h>
#include <cmath>
#include <functional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/benchmarking/benchmarker.hpp>
#include <vector>

/*
This:          0.193557 s
MATLAB SG Kit: 1.994590 s
*/

static constexpr size_t nDim = 4;
static constexpr size_t maxLvl = 10;
static constexpr size_t warmupRuns = 2;
static constexpr size_t runs = 5;

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

double sinFunc(const sgpp::base::DataVector& x) {
  double result = 1;

  for (size_t i = 0; i < nDim; i++) {
    result *= std::sin(x[i]);
  }

  return result;
}

double perform(tools::Benchmarker<double>::BenchmarkerContext& ctx) {
  // Source func
  const SourceFunc sourceFunc(sinFunc);

  ctx.markCheckpoint("Source func");

  // SG gen instr
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());

  ctx.markCheckpoint("SG Gen. Instruction");

  // SG gen
  const SparseGrid sg(genInstr);

  ctx.markCheckpoint("SG Generation");

  // Quadrature
  const double result = quadrature(sourceFunc, sg);

  ctx.markCheckpoint("Quadrature");

  return result;
}

int main() {
  tools::Benchmarker<double> benchmarker;
  benchmarker.setWarmupRounds(warmupRuns);
  benchmarker.setRounds(runs);
  benchmarker.enableProgress(true);

  benchmarker.addCheckpoint("Source func");
  benchmarker.addCheckpoint("SG Gen. Instruction");
  benchmarker.addCheckpoint("SG Generation");
  benchmarker.addCheckpoint("Quadrature");

  benchmarker.run(perform);

  benchmarker.printResults();

  return 0;
}