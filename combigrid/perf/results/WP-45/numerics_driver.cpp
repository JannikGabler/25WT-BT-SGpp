// Scratch: SG vs DSG global interpolation errors on the reference / e2e problems.
#include <omp.h>
#include <cmath>
#include <cstdio>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include "perf_problems.hpp"
using namespace sgpp::combigrid;
using sgpp::base::DataVector;
int main() {
  struct C { const char* nodes; LvlType b; size_t d; LvlType L; size_t n; unsigned seed; };
  std::vector<C> cs;
  for (const char* nodes : {"cc", "equi"})
    for (LvlType b : {0u, 1u})
      for (auto [d, L] : std::vector<std::pair<size_t, LvlType>>{{1, 6}, {2, 5}, {3, 4}, {5, 3}})
        cs.push_back({nodes, b, d, L, 8, unsigned(4000 + d)});
  cs.push_back({"equi", 0, 3, 5, 64, 1003});
  cs.push_back({"cc", 0, 2, 12, 64, 1002});
  std::printf("| case | max|sg-ex| | max|dsg1-ex| | max|dsg16-ex| | max|dsg1-sg| | max|dsg16-dsg1| | sum|sg-ex| | sum|dsg1-ex| |\n|---|---|---|---|---|---|---|---|\n");
  for (const C& c : cs) {
    const HyperCubeArea domain = perf::shiftedDomain(c.d);
    CompleteSGGenInstr gi(c.L, c.d);
    perf::configure(gi, c.nodes, c.b, domain);
    const SparseGrid sg(gi);
    const DeltasSparseGrid dsg(gi);
    const SourceFunc f(perf::cosProd);
    double m1 = 0, m2 = 0, m3 = 0, m4 = 0, m5 = 0, s1 = 0, s2 = 0;
    for (const DataVector& x : perf::randomPoints(domain, c.n, c.seed)) {
      const double ex = perf::cosProd(x);
      omp_set_num_threads(1);
      const double vsg = interpolate(f, x, sg);
      const double v1 = interpolate(f, x, dsg);
      omp_set_num_threads(16);
      double v16max = 0;
      for (int r = 0; r < 5; r++) {
        const double v16 = interpolate(f, x, dsg);
        m3 = std::max(m3, std::abs(v16 - ex));
        v16max = std::max(v16max, std::abs(v16 - v1));
      }
      m1 = std::max(m1, std::abs(vsg - ex)); m2 = std::max(m2, std::abs(v1 - ex));
      m4 = std::max(m4, std::abs(v1 - vsg)); m5 = std::max(m5, v16max);
      s1 += std::abs(vsg - ex); s2 += std::abs(v1 - ex);
    }
    std::printf("| %s b%u d%zu L%u p%zu | %.2e | %.2e | %.2e | %.2e | %.2e | %.2e | %.2e |\n", c.nodes, c.b, c.d, c.L, c.n, m1, m2, m3, m4, m5, s1, s2);
  }
}
