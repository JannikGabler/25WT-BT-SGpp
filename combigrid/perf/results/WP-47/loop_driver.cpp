#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
using namespace sgpp::combigrid;
double f(const sgpp::base::DataVector& x) { double p = 1; for (size_t i = 0; i < x.size(); i++) p *= std::cos(x[i]); return p; }
int main(int argc, char** argv) {
  size_t d = std::atoi(argv[1]); LvlType L = std::atoi(argv[2]); size_t n = std::atoi(argv[3]); bool useSG = argc > 4;
  CompleteSGGenInstr gi(L, d); gi.setNodeGenFunc(getEquidistantNodeGenFunc()); gi.setLvl2GPCntFunc(doublingLvl2GPCntFunction); gi.setBoundaryLevelOffset(1);
  DeltasSparseGrid dsg(gi); SparseGrid sg(gi); SourceFunc sf(f);
  sgpp::base::DataVector x(d, 0.3137);
  double s = 0; interpolateLinear(sf, x, dsg);
  auto t0 = std::chrono::steady_clock::now();
  for (size_t i = 0; i < n; i++) { x[0] = 0.1 + 0.8 * (i % 1000) / 1000.0; s += useSG ? interpolateLinear(sf, x, sg) : interpolateLinear(sf, x, dsg); }
  double t = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  std::printf("%s %.3f us/call (sum %.6f)\n", useSG ? "sg" : "dsg", 1e6 * t / n, s);
}
