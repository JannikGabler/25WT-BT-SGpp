#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/benchmarking/benchmarker.hpp>
#include <vector>

static constexpr size_t nDim = 2;
static constexpr size_t maxLvl = 10;

static constexpr double invDimCnt = 1.0 / nDim;

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

/*
Implements f(\vec x) = \prod_{i=1}^d (4 * (1 - x[i])^3 * e^{-x[i]/d})
*/
inline double integrant(const DataVector& x) {
  double result = 1.0;

  for (size_t dim = 0; dim < nDim; dim++) {
    result *= std::sin(3 * M_PI * x[dim]);
  }

  return std::abs(result);
}

double computeAnalyticalSolution() { return std::pow(2 / M_PI, nDim); }

double perform(tools::Benchmarker<double>::BenchmarkerContext& ctx) {
  // Source func
  const SourceFunc sourceFunc(integrant);

  ctx.mark_checkpoint("Source func");

  // SG gen instr
  FullSGGenInstr genInstr(maxLvl, nDim);
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

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Invalid number of arguments! Received " << (argc - 1) << " but requires 2."
              << std::endl;
    return 1;
  }

  size_t warmupRuns = 0;
  size_t runs = 0;

  try {
    warmupRuns = std::stoull(argv[1]);
    runs = std::stoull(argv[2]);
  } catch (const std::exception& e) {
    std::cerr << "Error while parsing arguments: " << e.what() << "\n";
    return 1;
  }

  tools::Benchmarker<double> benchmarker;
  benchmarker.set_warmups(warmupRuns);
  benchmarker.set_runs(runs);
  benchmarker.setAnalyticalResult(computeAnalyticalSolution());

  benchmarker.enable_progress(true);

  benchmarker.add_checkpoint("Source func");
  benchmarker.add_checkpoint("SG Gen. Instruction");
  benchmarker.add_checkpoint("SG Generation");
  benchmarker.add_checkpoint("Quadrature");

  benchmarker.run(perform);

  benchmarker.print_results();

  return 0;
}