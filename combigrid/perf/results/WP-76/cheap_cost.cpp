// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-76: per-evaluation cost of the judged suite's cheap integrands (1 thread), called through
// SourceFunc as in the operators. Built with build_proto.sh.
#include <chrono>
#include <cstdio>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>

#include "../../../benchmarks/perf/perf_problems.hpp"

namespace perf = sgpp::combigrid::perf;
using sgpp::combigrid::SourceFunc;

int main() {
  struct F { const char* name; double (*f)(const perf::DataVector&); };
  const F fs[] = {{"exp", perf::expSum}, {"uq", perf::uqIntegrand}, {"sin", perf::sinProd},
                  {"cos", perf::cosProd}};
  for (const F& f : fs) {
    for (const size_t d : {2, 5, 7, 10, 15, 20}) {
      const auto pts = perf::randomPoints(perf::unitDomain(d), 4096, 11);
      const SourceFunc sf(f.f);
      double sink = 0;
      for (int rep = 0; rep < 2; rep++) {
        const auto t0 = std::chrono::steady_clock::now();
        const size_t n = 4'000'000;
        for (size_t i = 0; i < n; i++) sink += sf.evaluate(pts[i & 4095]);
        const double s = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
        if (rep == 1) std::printf("%s d%zu: %.1f ns per evaluation\n", f.name, d, s * 1e9 / n);
      }
      if (sink == 1.2345) std::printf("!");
    }
  }
}
