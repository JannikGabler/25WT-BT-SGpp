#include <cmath>
#include <cstdio>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
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
  SourceFunc f(sgpp::combigrid::perf::cosProd);
  for (const auto& point : sgpp::combigrid::perf::randomPoints(domain, 8, 4000 + nDim)) {
    const auto q = tools::normalizeDataVector(point, domain);
    volatile double sf = 0, sn = 0;
    for (const auto& tg : grid) {
      double v = tools::lin_interpolation::processTensorGrid(f, q, tg, genInstr, scratch);
      double c = tg.coefficient;
      sf = std::fma(c, v, sf);
      volatile double prod = c * v; sn = sn + prod;
    }
    std::printf("op %a fma %a nofma %a\n", interpolateLinear(f, point, grid), (double)sf, (double)sn);
  }
}
