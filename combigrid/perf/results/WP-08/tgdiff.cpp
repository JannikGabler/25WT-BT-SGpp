#include <bit>
#include <cstdint>
#include <cstdio>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include "perf_problems.hpp"
using namespace sgpp::combigrid;
int main() {
  const size_t nDim = 5;
  const HyperCubeArea domain = sgpp::combigrid::perf::shiftedDomain(nDim);
  CompleteSGGenInstr genInstr(4, nDim);
  sgpp::combigrid::perf::configure(genInstr, "equi", 0, domain);
  const SparseGrid grid(genInstr);
  misc::LinInterpScratch scratch(nDim);
  int p = 0;
  for (const auto& point : sgpp::combigrid::perf::randomPoints(domain, 8, 4000 + nDim)) {
    const auto q = tools::normalizeDataVector(point, domain);
    int t = 0;
    for (const auto& tg : grid) {
      size_t cnt = 0; uint64_t h = 0;
      SourceFunc f([&](const sgpp::base::DataVector& x) { for (double v : x) h = h * 1000003 ^ std::bit_cast<uint64_t>(v); cnt++; return sgpp::combigrid::perf::cosProd(x); });
      double v = tools::lin_interpolation::processTensorGrid(f, q, tg, genInstr, scratch);
      std::printf("p%d tg%d %a h%016llx n%zu", p, t, v, (unsigned long long)h, cnt);
      for (size_t c = 0; c < cnt; c++) {
        size_t k = 0;
        SourceFunc ind([&](const sgpp::base::DataVector&) { return (k++ == c) ? 1.0 : 0.0; });
        std::printf(" %a", tools::lin_interpolation::processTensorGrid(ind, q, tg, genInstr, scratch));
      }
      std::printf("\n");
      t++;
    }
    p++;
  }
}
