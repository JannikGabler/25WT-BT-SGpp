// WP-77: underlying evaluations of a CachedSourceFunc quadrature (SG cc b1 d10 L4, 100 us busy-wait
// function, 16 threads). Built once per snapshot against matching headers (for WP-77-before:
// HEAD's sparse_grid_base.hpp first on the include path), run with the snapshot's lib/.
// Result: before 90562-90564 evaluations (590-655 ms), after 97086-98026 (639-700 ms);
// 90561 distinct points.
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
using namespace sgpp::combigrid;
std::atomic<long> nEvals{0};
double f(const sgpp::base::DataVector& x) {
  nEvals++;
  auto t0 = std::chrono::steady_clock::now();
  while (std::chrono::steady_clock::now() - t0 < std::chrono::microseconds(100)) {}
  double s = 0; for (size_t i = 0; i < x.size(); i++) s += x[i];
  return std::exp(s);
}
int main() {
  CompleteSGGenInstr gi(4, 10);
  gi.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  gi.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  gi.setBoundaryLevelOffset(1);
  const SparseGrid sg(gi);
  for (int rep = 0; rep < 3; rep++) {
    nEvals = 0;
    CachedSourceFunc cf(f);
    auto t0 = std::chrono::steady_clock::now();
    double v = quadrature(cf, sg);
    double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    std::printf("evals %ld  time %.1f ms  (%.6f)\n", nEvals.load(), ms, v);
  }
}
