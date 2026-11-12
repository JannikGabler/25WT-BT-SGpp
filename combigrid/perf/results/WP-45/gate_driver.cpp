// Scratch: work-gate crossover and split (basis values vs. sum) of the DSG global interpolation.
#include <omp.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/deltas_sparse_grid_global_interpolation_tools.hpp>
#include "perf_problems.hpp"
using namespace sgpp::combigrid;
namespace dgi = sgpp::combigrid::tools::dsg_global_interpolation;
using sgpp::base::DataVector;
template <typename F> double medianUs(F&& f, int reps) {
  std::vector<double> t;
  for (int r = 0; r < reps; r++) {
    auto s = std::chrono::steady_clock::now(); f();
    t.push_back(std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - s).count());
  }
  std::sort(t.begin(), t.end()); return t[t.size() / 2];
}
int main() {
  struct C { const char* nodes; size_t d; LvlType L; };
  std::vector<C> cs = {{"cc",1,3},{"cc",1,5},{"cc",1,7},{"cc",2,2},{"cc",2,3},{"cc",2,4},{"cc",2,5},{"cc",3,2},{"cc",3,3},{"cc",3,4},{"cc",5,2},{"cc",5,3},{"cc",2,8},{"cc",2,12},{"cc",3,9},{"cc",5,6},{"equi",3,5}};
  std::printf("| case | grid points | basis values [us] | serial [us] | parallel [us] | api [us] |\n|---|---|---|---|---|---|\n");
  volatile double sink = 0;
  for (const C& c : cs) {
    const HyperCubeArea domain = perf::shiftedDomain(c.d);
    CompleteSGGenInstr gi(c.L, c.d);
    perf::configure(gi, c.nodes, 0, domain);
    const DeltasSparseGrid dsg(gi);
    const SourceFunc f(perf::cosProd);
    const DataVector x = perf::randomPoints(domain, 1, 97)[0];
    const auto& index = dgi::getIndex(dsg);
    const DataVector nx = tools::normalizeDataVector(x, domain);
    const int reps = index.nGridPoints() > 100000 ? 41 : 401;
    const double tb = medianUs([&] { sink = sink + dgi::compBasisValues(index, nx).flat()[0]; }, reps);
    const double ts = medianUs([&] { sink = sink + dgi::evaluateInterpolant(index, dsg, nx, f, false); }, reps);
    const double tp = medianUs([&] { sink = sink + dgi::evaluateInterpolant(index, dsg, nx, f, true); }, reps);
    const double ta = medianUs([&] { sink = sink + interpolate(f, x, dsg); }, reps);
    std::printf("| %s d%zu L%u | %zu | %.2f | %.2f | %.2f | %.2f |\n", c.nodes, c.d, c.L, index.nGridPoints(), tb, ts, tp, ta);
  }
}
