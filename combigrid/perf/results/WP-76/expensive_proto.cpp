// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file expensive_proto.cpp
 * @brief Prototype of expensive source functions for the judged e2e suite (WP-76, phase 1).
 *
 * Not part of the build; compiled by build_proto.sh with the judged flags against lib/.
 *
 * Modes:
 *   cost                         per-evaluation cost of the cost generators at 1 and 16 threads
 *   count                        evaluations / tensor grids / points of the candidate cases
 *   case REGEX GEN COST_US SHAPE time the matching candidate cases (16 threads by default)
 *        GEN = chain | spin | mix, SHAPE = uniform | ramp
 */

#include <omp.h>
#include <x86intrin.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "../../../benchmarks/perf/perf_problems.hpp"
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>

using namespace sgpp::combigrid;  // NOLINT (prototype)
namespace perf = sgpp::combigrid::perf;
using DataVector = sgpp::base::DataVector;
using Clock = std::chrono::steady_clock;

namespace {

/******************************** cost generators ********************************/

// (chain) dependent FMA chain: latency-bound, 4 cycles per iteration on Zen 3; the empty asm
// keeps the compiler from folding, vectorizing or reassociating it.
__attribute__((noinline)) double burnChain(double w, const uint64_t iters) {
  for (uint64_t i = 0; i < iters; i++) {
    w = std::fma(w, 0.99999999, 1e-9);
    asm volatile("" : "+x"(w));
  }
  return w;
}

// (mix) four independent chains + one sqrt per iteration: throughput-bound like real numerics
// (competes with the SMT sibling for the FP pipes).
__attribute__((noinline)) double burnMix(double w, const uint64_t iters) {
  double a = w, b = w + 1, c = w + 2, d = w + 3;
  for (uint64_t i = 0; i < iters; i++) {
    a = std::fma(a, 0.99999999, 1e-9);
    b = std::fma(b, 0.99999998, 2e-9);
    c = std::fma(c, 0.99999997, 3e-9);
    d = std::sqrt(d * d + 1e-9);
    asm volatile("" : "+x"(a), "+x"(b), "+x"(c), "+x"(d));
  }
  return a + b + c + d;
}

// (spin) timed spin on the TSC (rdtscp ~37 ns): exact wall time, independent of the code.
__attribute__((noinline)) double burnSpin(const double w, const uint64_t ticks) {
  unsigned aux;
  const uint64_t start = __rdtscp(&aux);
  while (__rdtscp(&aux) - start < ticks) {
  }
  return w;
}

enum class Gen { chain, mix, spin };
Gen gen = Gen::chain;
uint64_t itersPerUnit = 0;  // iterations (ticks for spin) for a factor-1 evaluation
bool ramp = false;          // cost factor 0.1 + 2.7 x0^2 (x0 in the unit cube coordinates)
std::atomic<uint64_t> totalIters{0};
bool cached = false;      // time with a fresh CachedSourceFunc (MemoryOnly) per run
bool countIters = false;  // only in the (untimed) counting run: the atomic is contended
double nsPerIterCal = 0;

double burn(const double seed, const uint64_t iters) {
  switch (gen) {
    case Gen::chain: return burnChain(seed, iters);
    case Gen::mix: return burnMix(seed, iters);
    case Gen::spin: return burnSpin(seed, iters);
  }
  return seed;
}

double costFactor(const DataVector& x) {
  if (!ramp) return 1.0;
  const double x0 = std::clamp(x[0], 0.0, 1.0);
  return 0.1 + 2.7 * x0 * x0;
}

template <double (*F)(const DataVector&)>
double expensive(const DataVector& x) {
  const double value = F(x);
  const auto iters = static_cast<uint64_t>(static_cast<double>(itersPerUnit) * costFactor(x));
  if (countIters) totalIters.fetch_add(iters, std::memory_order_relaxed);
  double w = burn(x[0], iters);
  asm volatile("" : "+x"(w));
  return value;
}

double secondsSince(const Clock::time_point t) {
  return std::chrono::duration<double>(Clock::now() - t).count();
}

double tscHz() {
  unsigned aux;
  const auto t0 = Clock::now();
  const uint64_t c0 = __rdtscp(&aux);
  while (secondsSince(t0) < 0.2) {
  }
  return static_cast<double>(__rdtscp(&aux) - c0) / secondsSince(t0);
}

/******************************** mode cost ********************************/

// ns per evaluation of expensive<expSum> with N evaluations distributed over the threads.
double nsPerEval(const int threads, const size_t n, const size_t nDim) {
  const auto pts = perf::randomPoints(perf::unitDomain(nDim), 1024, 7);
  const auto t0 = Clock::now();
  double sink = 0;
#pragma omp parallel for num_threads(threads) schedule(static) reduction(+ : sink)
  for (size_t i = 0; i < n; i++) sink += expensive<perf::expSum>(pts[i % pts.size()]);
  const double s = secondsSince(t0);
  if (sink == 12345.678) std::printf("!");
  return s * 1e9 * threads / static_cast<double>(n);
}

void modeCost() {
  const double hz = tscHz();
  std::printf("TSC %.4f GHz\n", hz * 1e-9);
  for (const Gen g : {Gen::chain, Gen::mix, Gen::spin}) {
    gen = g;
    const char* name = g == Gen::chain ? "chain" : (g == Gen::mix ? "mix" : "spin");
    // calibrate: ns per iteration at 1 thread
    double nsPerIter;
    if (g == Gen::spin) {
      nsPerIter = 1e9 / hz;
    } else {
      itersPerUnit = 100000;
      nsPerEval(1, 200, 5);
      nsPerIter = nsPerEval(1, 2000, 5) / 100000.0;
    }
    std::printf("%s: %.4f ns per iteration (1 t)\n", name, nsPerIter);
    for (const double us : {1.0, 10.0, 100.0}) {
      itersPerUnit = static_cast<uint64_t>(us * 1000.0 / nsPerIter);
      const size_t n1 = static_cast<size_t>(0.5e6 / us), n16 = 16 * n1;
      std::vector<double> t1, t16, t8;
      for (int r = 0; r < 3; r++) {
        t1.push_back(nsPerEval(1, n1, 5));
        t8.push_back(nsPerEval(8, n16 / 2, 5));
        t16.push_back(nsPerEval(16, n16, 5));
      }
      std::sort(t1.begin(), t1.end());
      std::sort(t8.begin(), t8.end());
      std::sort(t16.begin(), t16.end());
      std::printf(
          "  %s %6.0f us target: iters %8lu | 1 t %9.1f ns | 8 t %9.1f ns-thread (eff %.3f) "
          "| 16 t %9.1f ns-thread (eff %.3f, speedup vs 1 t %.2fx)\n",
          name, us, static_cast<unsigned long>(itersPerUnit), t1[1], t8[1], t1[1] / t8[1] ,
          t16[1], t1[1] / t16[1], 16.0 * t1[1] / t16[1]);
    }
  }
}

/******************************** candidate cases ********************************/

struct CountingFunc {
  double (*func)(const DataVector&);
  std::shared_ptr<std::atomic<size_t>> count = std::make_shared<std::atomic<size_t>>(0);
  SourceFunc sourceFunc() const {
    auto f = func;
    auto c = count;
    return SourceFunc([f, c](const DataVector& x) {
      c->fetch_add(1, std::memory_order_relaxed);
      return f(x);
    });
  }
};

struct Result {
  double value, exact;
  size_t nTG, sumGP;
};

template <typename Grid>
size_t sumGP(const Grid& grid) {
  size_t s = 0;
  for (const auto& tg : grid.domainTensorGrids()) s += tg.tensorGrid.nGP();
  return s;
}

using CaseFn = std::function<Result(const SourceFunc&)>;
struct Case {
  std::string name;
  bool interpTarget;  // cosProd (else expSum)
  CaseFn fn;
};

template <typename Grid>
Case quadCase(const std::string& g, const std::string& nodes, LvlType b, size_t d, LvlType L) {
  const std::string name = "quad/" + g + "/" + nodes + "/b" + std::to_string(b) + "/d" +
                           std::to_string(d) + "/L" + std::to_string(L);
  return {name, false, [=](const SourceFunc& f) {
            CompleteSGGenInstr gi(L, d);
            perf::configure(gi, nodes, b, perf::unitDomain(d));
            const Grid grid(gi);
            return Result{quadrature(f, grid), perf::expSumIntegral(perf::unitDomain(d)),
                          grid.nTG(), sumGP(grid)};
          }};
}

Case mivecCase(const std::string& nodes, LvlType b, size_t d, LvlType S, size_t n, bool dsg) {
  const std::string name = std::string(dsg ? "mivec/dsg/" : "mivec/sg/") + nodes + "/b" +
                           std::to_string(b) + "/d" + std::to_string(d) + "/S" +
                           std::to_string(S) + "/n" + std::to_string(n);
  return {name, false, [=](const SourceFunc& f) {
            const LvlMIVec mis = perf::anisotropicMIs(d, n, S, 2000 + d);
            MIVecSGGenInstr gi(mis);
            perf::configure(gi, nodes, b, perf::unitDomain(d));
            if (dsg) {
              const DeltasSparseGrid grid(gi);
              return Result{quadrature(f, grid), perf::expSumIntegral(perf::unitDomain(d)),
                            grid.nTG(), sumGP(grid)};
            }
            const SparseGrid grid(gi);
            return Result{quadrature(f, grid), perf::expSumIntegral(perf::unitDomain(d)),
                          grid.nTG(), sumGP(grid)};
          }};
}

template <typename Grid>
Case interpCase(bool lin, const std::string& g, const std::string& nodes, LvlType b, size_t d,
                LvlType L, size_t p) {
  const std::string name = std::string(lin ? "interplin/" : "interp/") + g + "/" + nodes + "/b" +
                           std::to_string(b) + "/d" + std::to_string(d) + "/L" +
                           std::to_string(L) + "/p" + std::to_string(p);
  return {name, true, [=](const SourceFunc& f) {
            const HyperCubeArea dom = perf::shiftedDomain(d);
            CompleteSGGenInstr gi(L, d);
            perf::configure(gi, nodes, b, dom);
            const Grid grid(gi);
            double sum = 0, exact = 0;
            for (const DataVector& x : perf::randomPoints(dom, p, 1000 + d)) {
              sum += lin ? interpolateLinear(f, x, grid) : interpolate(f, x, grid);
              exact += perf::cosProd(x);
            }
            return Result{sum, exact, grid.nTG(), sumGP(grid)};
          }};
}

std::vector<Case> candidates() {
  std::vector<Case> c;
  using QS = std::tuple<size_t, LvlType, LvlType>;  // d, L, boundary offset
  for (const auto& [d, L, b] : std::vector<QS>{
           {2, 10, 0}, {2, 12, 0}, {2, 14, 0}, {3, 8, 0}, {3, 10, 0}, {5, 5, 0}, {5, 6, 0},
           {5, 6, 1}, {5, 7, 1}, {5, 8, 1}, {7, 5, 1}, {7, 6, 1}, {10, 4, 1}, {10, 5, 1},
           {10, 6, 1}, {15, 3, 1}, {15, 4, 1}, {15, 5, 1}, {20, 3, 1}, {20, 4, 1}}) {
    c.push_back(quadCase<SparseGrid>("sg", "cc", b, d, L));
    c.push_back(quadCase<DeltasSparseGrid>("dsg", "cc", b, d, L));
    c.push_back(quadCase<SparseGrid>("sg", "equi", b, d, L));
    c.push_back(quadCase<DeltasSparseGrid>("dsg", "equi", b, d, L));
  }
  for (const auto& [d, S, n] : std::vector<std::tuple<size_t, LvlType, size_t>>{
           {5, 10, 300}, {5, 12, 300}, {5, 14, 300}, {10, 8, 300}, {10, 10, 300}, {15, 7, 300},
           {15, 8, 300}, {15, 10, 300}, {20, 7, 300}, {20, 8, 300}, {20, 10, 300}, {10, 6, 300},
           {15, 5, 300}, {15, 6, 300}, {20, 5, 300}, {20, 6, 300}, {10, 7, 300}}) {
    c.push_back(mivecCase("cc", 1, d, S, n, false));
    c.push_back(mivecCase("cc", 1, d, S, n, true));
  }
  using IS = std::tuple<size_t, LvlType, size_t, LvlType>;  // d, L, points, boundary offset
  for (const auto& [d, L, p, b] : std::vector<IS>{
           {2, 8, 16, 0}, {3, 6, 16, 0}, {5, 3, 16, 0}, {5, 4, 16, 1}, {5, 5, 16, 1},
           {7, 3, 8, 1}, {7, 4, 8, 1}, {10, 2, 8, 1}, {10, 3, 8, 1}, {10, 4, 4, 1},
           {15, 3, 4, 1}, {10, 4, 16, 1}}) {
    c.push_back(interpCase<SparseGrid>(false, "sg", "cc", b, d, L, p));
    c.push_back(interpCase<DeltasSparseGrid>(false, "dsg", "cc", b, d, L, p));
  }
  for (const auto& [d, L, p] : std::vector<std::tuple<size_t, LvlType, size_t>>{
           {2, 10, 256}, {5, 5, 64}, {5, 6, 64}, {8, 3, 32}, {8, 4, 32}, {10, 3, 16},
           {10, 4, 16}, {10, 5, 8}, {15, 4, 8}, {5, 5, 16}}) {
    c.push_back(interpCase<SparseGrid>(true, "sg", "equi", 1, d, L, p));
    c.push_back(interpCase<DeltasSparseGrid>(true, "dsg", "equi", 1, d, L, p));
  }
  return c;
}

void modeCount(const std::string& re) {
  const std::regex rx(re);
  std::printf("| case | nTG | sumGP | evals | evals/sumGP |\n|---|---|---|---|---|\n");
  for (const Case& c : candidates()) {
    if (c.name.find("/equi/") != std::string::npos && c.name.rfind("quad/", 0) == 0) continue;
    if (!std::regex_search(c.name, rx)) continue;
    const CountingFunc cf{c.interpTarget ? perf::cosProd : perf::expSum};
    const Result r = c.fn(cf.sourceFunc());
    const size_t n = cf.count->load();
    std::printf("| %s | %zu | %zu | %zu | %.3f |\n", c.name.c_str(), r.nTG, r.sumGP, n,
                static_cast<double>(n) / static_cast<double>(r.sumGP));
    std::fflush(stdout);
  }
}

void modeCase(const std::string& re, double costUs, int reps) {
  if (gen == Gen::spin) {
    nsPerIterCal = 1e9 / tscHz();
  } else {
    itersPerUnit = 100000;
    const bool savedRamp = ramp;
    ramp = false;
    nsPerEval(1, 200, 5);
    nsPerIterCal = nsPerEval(1, 2000, 5) / 100000.0;
    ramp = savedRamp;
  }
  itersPerUnit = static_cast<uint64_t>(costUs * 1000.0 / nsPerIterCal);
  const int threads = omp_get_max_threads();
  std::printf("gen %s, %.0f us, %s, %d threads, %.4f ns/iter\n",
              gen == Gen::spin ? "spin" : (gen == Gen::mix ? "mix" : "chain"), costUs,
              ramp ? "ramp" : "uniform", threads, nsPerIterCal);
  std::printf("| case | evals | mean cost us | median s | min s | max s | ideal s | eff |\n"
              "|---|---|---|---|---|---|---|---|\n");
  const std::regex rx(re);
  for (const Case& c : candidates()) {
    if (!std::regex_search(c.name, rx)) continue;
    const CountingFunc cf{c.interpTarget ? expensive<perf::cosProd> : expensive<perf::expSum>};
    totalIters = 0;
    countIters = true;
    c.fn(cf.sourceFunc());  // counting run (untimed, also the warm-up)
    countIters = false;
    const size_t evals = cf.count->load();
    const SourceFunc f(c.interpTarget ? expensive<perf::cosProd> : expensive<perf::expSum>);
    const double busy = static_cast<double>(totalIters.load()) * nsPerIterCal * 1e-9;
    std::vector<double> t;
    for (int i = 0; i < reps; i++) {
      const auto t0 = Clock::now();
      if (cached) {
        const CachedSourceFunc cf2(c.interpTarget ? expensive<perf::cosProd>
                                                  : expensive<perf::expSum>);
        c.fn(cf2);
        if (i == 0) {
          const auto st = cf2.statistics();
          std::printf("  [cache: lookups %zu hits %zu inserts %zu bytes %zu]\n", st.lookups, st.hits, st.inserts, st.bytes);
        }
      } else {
        c.fn(f);
      }
      t.push_back(secondsSince(t0));
    }
    std::sort(t.begin(), t.end());
    const double med = t[t.size() / 2], ideal = busy / threads;
    std::printf("| %s | %zu | %.2f | %.4f | %.4f | %.4f | %.4f | %.3f |\n", c.name.c_str(), evals,
                busy / static_cast<double>(evals) * 1e6, med, t.front(), t.back(), ideal,
                ideal / med);
    std::fflush(stdout);
  }
}

}  // namespace

int main(int argc, char** argv) {
  const std::string mode = argc > 1 ? argv[1] : "";
  if (mode == "cost") {
    modeCost();
  } else if (mode == "count") {
    modeCount(argc > 2 ? argv[2] : "");
  } else if (mode == "case" && argc >= 6) {
    const std::string g = argv[3];
    gen = g == "spin" ? Gen::spin : (g == "mix" ? Gen::mix : Gen::chain);
    ramp = std::string(argv[5]).starts_with("ramp");
    cached = std::string(argv[5]).ends_with("cached");
    modeCase(argv[2], std::stod(argv[4]), argc > 6 ? std::stoi(argv[6]) : 3);
  } else {
    std::fprintf(stderr, "usage: cost | count | case REGEX chain|mix|spin COST_US uniform|ramp [REPS]\n");
    return 2;
  }
  return 0;
}
