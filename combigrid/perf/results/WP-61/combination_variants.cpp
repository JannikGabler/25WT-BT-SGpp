// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-61: round-off and cost of summation variants for the SG quadrature's combination
// sum_l c_l Q_l. For every judged SG quadrature / mivec e2e grid (the expensive/ grids with the
// plain expSum, which returns the same values) it computes the per-tensor-grid estimates Q_l
// with the library (tools::quadrature::estimateTensorGrids, 16 threads, deterministic) and
//   plain     the library's ordered loop sum += c * Q (product rounded; -ffp-contract=off),
//   plainfma  the same with std::fma (contracted form),
//   split16   the old reduction(+): 16 partial sums (round-robin TGs), combined serially,
//   pairwise  recursive halving, plain loop below 32 terms (TG order kept),
//   neumaier  compensated summation incl. the exact product errors (fma; "Dot2"-like),
// times each variant (min over repetitions, 1 thread) and writes per case
// <out>/<idx>.bin = [nTG:int64][exact integral:f64][library result:f64][coefficients:int32 x nTG]
// [estimates:f64 x nTG][variants:f64 x 5]. combination_variants.py computes the exact sum.
// Build (repo root):
//   g++ -O2 -std=c++23 -march=native -ffp-contract=off -fopenmp -DNDEBUG -Icombigrid/src
//     -Ibase/src -Icombigrid/benchmarks/perf combigrid/perf/results/WP-61/combination_variants.cpp
//     -L<snapshot>/lib -lsgppcombigrid -lsgppbase -o <scratch>/combination_variants
#include <omp.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <span>
#include <string>
#include <vector>

#include "perf_problems.hpp"

namespace cg = sgpp::combigrid;
using sgpp::base::DataVector;

namespace {

using Coeffs = std::span<const cg::CTCoeffType>;
using Values = std::span<const double>;

double plain(Coeffs c, Values q) {
  double s = 0.0;
  for (size_t i = 0; i < q.size(); i++) s += c[i] * q[i];
  return s;
}

double plainFma(Coeffs c, Values q) {
  double s = 0.0;
  for (size_t i = 0; i < q.size(); i++) s = std::fma(c[i], q[i], s);
  return s;
}

double split16(Coeffs c, Values q) {
  double partial[16] = {};
  for (size_t i = 0; i < q.size(); i++) partial[i % 16] += c[i] * q[i];
  double s = 0.0;
  for (const double p : partial) s += p;
  return s;
}

double pairwise(Coeffs c, Values q) {
  if (q.size() <= 32) return plain(c, q);
  const size_t half = q.size() / 2;
  return pairwise(c.first(half), q.first(half)) + pairwise(c.subspan(half), q.subspan(half));
}

double neumaier(Coeffs c, Values q) {
  double s = 0.0, comp = 0.0;
  for (size_t i = 0; i < q.size(); i++) {
    const double p = c[i] * q[i];
    comp += std::fma(c[i], q[i], -p);  // exact error of the product
    const double t = s + p;
    comp += (std::abs(s) >= std::abs(p)) ? (s - t) + p : (p - t) + s;
    s = t;
  }
  return s + comp;
}

double timeUs(const std::function<double(Coeffs, Values)>& f, Coeffs c, Values q) {
  double best = 1e30;
  volatile double sink = 0.0;
  for (int rep = 0; rep < 200; rep++) {
    const auto t0 = std::chrono::steady_clock::now();
    sink = sink + f(c, q);
    const auto t1 = std::chrono::steady_clock::now();
    best = std::min(best, std::chrono::duration<double, std::micro>(t1 - t0).count());
  }
  return best;
}

struct Case {
  std::string name;
  std::string nodes;
  std::string integrand;  // exp, sin, uq
  cg::LvlType boundary;
  size_t nDim;
  cg::LvlType level;  // complete grids; mivec: max 1-norm
  size_t nMI;         // 0: complete grid
};

void runCase(const Case& c, const std::string& path) {
  std::optional<cg::LvlMIVec> mis;
  std::unique_ptr<cg::SGGenInstr> genInstr;
  if (c.nMI == 0) {
    genInstr = std::make_unique<cg::CompleteSGGenInstr>(c.level, c.nDim);
  } else {
    mis.emplace(cg::perf::anisotropicMIs(c.nDim, c.nMI, c.level, 2000 + c.nDim));
    genInstr = std::make_unique<cg::MIVecSGGenInstr>(*mis);
  }
  const cg::HyperCubeArea domain = cg::perf::unitDomain(c.nDim);
  cg::perf::configure(*genInstr, c.nodes, c.boundary, domain);
  const cg::SparseGrid sg(*genInstr);

  double (*func)(const DataVector&) = cg::perf::expSum;
  double exact = cg::perf::expSumIntegral(domain);
  if (c.integrand == "sin") {
    func = cg::perf::sinProd;
    exact = cg::perf::sinProdIntegral(domain);
  } else if (c.integrand == "uq") {
    func = cg::perf::uqIntegrand;
    exact = cg::perf::uqIntegral(c.nDim);
  }
  const cg::SourceFunc f(func);

  const size_t nTG = sg.nTG();
  std::vector<double> q(nTG);
  const cg::misc::QuadWeightTable table(sg);
  const cg::misc::QuadratureContext context{.sparseGrid = sg, .weightTable = table};
  omp_set_num_threads(16);
  cg::tools::quadrature::estimateTensorGrids(context, f, q);
  const double library = cg::quadrature(f, sg);
  omp_set_num_threads(1);

  const Coeffs coeffs = sg.getCoefficients();
  const std::vector<std::function<double(Coeffs, Values)>> variants{plain, plainFma, split16,
                                                                    pairwise, neumaier};
  std::vector<double> results;
  std::printf("%-48s nTG %6zu", c.name.c_str(), nTG);
  for (const auto& v : variants) {
    results.push_back(v(coeffs, q));
    std::printf(" %8.1f", timeUs(v, coeffs, q));
  }
  std::printf("  (us: plain plainfma split16 pairwise neumaier)\n");

  FILE* const out = std::fopen(path.c_str(), "wb");
  const int64_t n = static_cast<int64_t>(nTG);
  std::fwrite(&n, sizeof(n), 1, out);
  std::fwrite(&exact, sizeof(double), 1, out);
  std::fwrite(&library, sizeof(double), 1, out);
  std::fwrite(coeffs.data(), sizeof(cg::CTCoeffType), nTG, out);
  std::fwrite(q.data(), sizeof(double), nTG, out);
  std::fwrite(results.data(), sizeof(double), results.size(), out);
  std::fclose(out);
}

}  // namespace

int main(int argc, char** argv) {
  const std::string outdir = argv[1];
  const std::vector<Case> cases{
      {"quad/sg/equi-exp/b0/d2/L20", "equi", "exp", 0, 2, 20, 0},
      {"quad/sg/equi-exp/b0/d5/L10", "equi", "exp", 0, 5, 10, 0},
      {"quad/sg/equi-exp/b0/d7/L7", "equi", "exp", 0, 7, 7, 0},
      {"quad/sg/equi-exp/b0/d10/L4", "equi", "exp", 0, 10, 4, 0},
      {"quad/sg/equi-exp/b1/d10/L7", "equi", "exp", 1, 10, 7, 0},
      {"quad/sg/equi-exp/b1/d15/L6", "equi", "exp", 1, 15, 6, 0},
      {"quad/sg/equi-exp/b1/d20/L5", "equi", "exp", 1, 20, 5, 0},
      {"quad/sg/equi-sin/b0/d7/L7", "equi", "sin", 0, 7, 7, 0},
      {"quad/sg/cc-uq/b0/d2/L20", "cc", "uq", 0, 2, 20, 0},
      {"quad/sg/cc-uq/b0/d3/L14", "cc", "uq", 0, 3, 14, 0},
      {"quad/sg/cc-uq/b0/d5/L10", "cc", "uq", 0, 5, 10, 0},
      {"quad/sg/cc-uq/b0/d7/L7", "cc", "uq", 0, 7, 7, 0},
      {"quad/sg/cc-uq/b0/d10/L4", "cc", "uq", 0, 10, 4, 0},
      {"mivec/sg/cc-exp/b1/d5/S16/n1000", "cc", "exp", 1, 5, 16, 1000},
      {"mivec/sg/cc-exp/b1/d10/S12/n1000", "cc", "exp", 1, 10, 12, 1000},
      {"mivec/sg/cc-exp/b1/d15/S10/n300", "cc", "exp", 1, 15, 10, 300},
      {"mivec/sg/cc-exp/b1/d20/S7/n300", "cc", "exp", 1, 20, 7, 300},
      {"(expensive/)quad/sg/cc-exp/b1/d10/L4", "cc", "exp", 1, 10, 4, 0},
      {"(expensive/)quad/sg/cc-exp/b1/d20/L3", "cc", "exp", 1, 20, 3, 0},
      {"(expensive/)quad/sg/cc-exp/b1/d15/L5", "cc", "exp", 1, 15, 5, 0},
      {"(expensive/)mivec/sg/cc-exp/b1/d15/S6/n300", "cc", "exp", 1, 15, 6, 300},
      {"(expensive/)mivec/sg/cc-exp/b1/d20/S10/n300", "cc", "exp", 1, 20, 10, 300},
  };
  FILE* const names = std::fopen((outdir + "/cases.txt").c_str(), "w");
  for (size_t i = 0; i < cases.size(); i++) {
    std::fprintf(names, "%s\n", cases[i].name.c_str());
    std::fflush(names);
    runCase(cases[i], outdir + "/" + std::to_string(i) + ".bin");
    std::fflush(stdout);
  }
  std::fclose(names);
}
