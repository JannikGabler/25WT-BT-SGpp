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
This:          19.172908s
MATLAB SG Kit: Crash (out of RAM)
*/

static constexpr size_t nDim = 6;
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

  ctx.mark_checkpoint("Source func");

  // SG gen instr
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

  ctx.mark_checkpoint("SG Gen. Instruction");

  // SG gen
  const SparseGrid sg(genInstr);

  ctx.mark_checkpoint("SG Generation");

  // Quadrature
  const double result = quadrature(sourceFunc, sg);

  ctx.mark_checkpoint("Quadrature");

  return result;
}

int main() {
  tools::Benchmarker<double> benchmarker;
  benchmarker.set_warmups(warmupRuns);
  benchmarker.set_runs(runs);
  benchmarker.enable_progress(true);

  benchmarker.add_checkpoint("Source func");
  benchmarker.add_checkpoint("SG Gen. Instruction");
  benchmarker.add_checkpoint("SG Generation");
  benchmarker.add_checkpoint("Quadrature");

  benchmarker.run(perform);

  benchmarker.print_results();

  return 0;
}