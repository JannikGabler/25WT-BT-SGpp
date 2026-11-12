#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
using namespace sgpp::combigrid;
double f(const sgpp::base::DataVector& x) { double p = 1; for (size_t i = 0; i < x.size(); i++) p *= std::cos(x[i]); return p; }
int main(int argc, char** argv) {
  size_t d = std::atoi(argv[1]); LvlType L = std::atoi(argv[2]);
  CompleteSGGenInstr gi(L, d); gi.setNodeGenFunc(getEquidistantNodeGenFunc()); gi.setLvl2GPCntFunc(doublingLvl2GPCntFunction); gi.setBoundaryLevelOffset(1);
  auto t0 = std::chrono::steady_clock::now();
  DeltasSparseGrid dsg(gi); SourceFunc sf(f); sgpp::base::DataVector x(d, 0.3137);
  auto t1 = std::chrono::steady_clock::now();
  double a = interpolateLinear(sf, x, dsg);
  auto t2 = std::chrono::steady_clock::now();
  double b = interpolateLinear(sf, x, dsg);
  auto t3 = std::chrono::steady_clock::now();
  auto ms = [](auto u, auto v) { return std::chrono::duration<double>(v - u).count() * 1e3; };
  std::printf("d%zu L%u: generation %.2f ms, first call %.3f ms, second call %.3f ms (%g)\n", d, L, ms(t0, t1), ms(t1, t2), ms(t2, t3), a - b);
}
