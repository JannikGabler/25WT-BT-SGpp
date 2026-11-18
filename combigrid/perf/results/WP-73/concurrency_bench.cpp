// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file concurrency_bench.cpp
 * @brief Per-call cost of the cross-call operator data cache under concurrency.
 *
 * Mode "lookup" (needs the headers and library with misc::CrossCallOperatorDataCache): the lookup
 * of a cached object (3 entries, the last one requested) with the synchronization variants
 *   a      production: mutex + scan + shared_ptr copy (misc::CrossCallOperatorDataCache),
 *   a-rw   std::shared_mutex (shared lock) + scan + shared_ptr copy,
 *   a-snap atomic<shared_ptr<const entries>> load + scan + shared_ptr copy,
 *   c      mutex + scan + raw pointer (the cache before: clear would need no concurrent calls),
 *   b      no synchronization + scan + raw pointer (single caller),
 * by 1 and by T threads (std::thread) at the same time: ns per lookup (median of 7 runs).
 * Mode "spaced": the same lookups, each followed by about 7 us of private work (a dependent
 * chain, as a single-point call in 2 dimensions): us per iteration; the difference to variant b
 * is the cost of the synchronization in calls of that size.
 *
 * Mode "ops": one operator call at one query point on a DeltasSparseGrid (cached index, the
 * default): interpolateLinear equi b1 d2 L16 / d10 L5, interpolate equi b0 d3 L5 / cc b0 d2 L12;
 * 1 caller and T concurrent callers (std::thread, each with omp_set_num_threads(1)): us per call
 * (median of 7 runs). Compiles against the public API only, so the same source runs on both
 * snapshots.
 *
 * Usage: concurrency_bench lookup|spaced|ops <threads>
 */

#include <omp.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <typeindex>
#include <utility>
#include <vector>

#include <sgpp_combigrid.hpp>
#ifdef WITH_CACHE_HEADER
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_cache.hpp>
#endif

namespace cg = sgpp::combigrid;
using sgpp::base::DataVector;

namespace {

std::atomic<size_t> sink{0};

/// Runs body(threadIdx) on nThreads std::threads at once; returns the wall time in seconds.
double runThreads(const int nThreads, const std::function<void(int)>& body) {
  std::atomic<int> ready{0};
  std::atomic<bool> go{false};
  std::vector<std::thread> threads;
  for (int t = 0; t < nThreads; t++) {
    threads.emplace_back([&, t]() {
      omp_set_num_threads(1);
      ready++;
      while (!go.load()) {
      }
      body(t);
    });
  }
  while (ready.load() < nThreads) {
  }
  const auto start = std::chrono::steady_clock::now();
  go = true;
  for (auto& thread : threads) thread.join();
  return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

double median(std::vector<double> v) {
  std::sort(v.begin(), v.end());
  return v[v.size() / 2];
}

struct Entry {
  std::type_index type;
  std::shared_ptr<const void> object;
};

struct Obj1 {};
struct Obj2 {};
struct Obj3 {
  size_t value = 7;
};

std::vector<Entry> makeEntries() {
  return {{typeid(Obj1), std::make_shared<Obj1>()},
          {typeid(Obj2), std::make_shared<Obj2>()},
          {typeid(Obj3), std::make_shared<Obj3>()}};
}

const void* scan(const std::vector<Entry>& entries, const std::type_index type) {
  for (const Entry& entry : entries) {
    if (entry.type == type) return entry.object.get();
  }
  return nullptr;
}

std::shared_ptr<const void> scanShared(const std::vector<Entry>& entries,
                                       const std::type_index type) {
  for (const Entry& entry : entries) {
    if (entry.type == type) return entry.object;
  }
  return nullptr;
}

/// About @p iterations dependent multiply-adds (not removable by the compiler).
double privateWork(const size_t iterations, double x) {
  for (size_t i = 0; i < iterations; i++) x = std::fma(x, 0.999999, 1e-9);
  return x;
}

void lookupMode(const int nThreads, const bool spaced) {
  const size_t N = spaced ? 20000 : 2000000;
  const size_t WORK = spaced ? 3500 : 0;  // About 7 us at 2 GHz (4 cycles per iteration)
  const std::vector<Entry> entries = makeEntries();
  std::mutex mutex;
  std::shared_mutex rwMutex;
  std::atomic<std::shared_ptr<const std::vector<Entry>>> snapshot{
      std::make_shared<const std::vector<Entry>>(makeEntries())};
#ifdef WITH_CACHE_HEADER
  cg::misc::CrossCallOperatorDataCache cache;
  constexpr cg::misc::CrossCallOperatorDataDescriptor D{.operatorName = "x", .description = "y",
                                                        .key = ""};
  cache.getOrBuild<Obj1>(D, []() { return std::make_unique<Obj1>(); });
  cache.getOrBuild<Obj2>(D, []() { return std::make_unique<Obj2>(); });
  cache.getOrBuild<Obj3>(D, []() { return std::make_unique<Obj3>(); });
#endif

  const std::vector<std::pair<std::string, std::function<size_t()>>> variants{
#ifdef WITH_CACHE_HEADER
      {"a",
       [&]() {
         return cache.getOrBuild<Obj3>(D, []() { return std::make_unique<Obj3>(); })->value;
       }},
#endif
      {"a-rw",
       [&]() {
         std::shared_ptr<const void> p;
         {
           std::shared_lock lock(rwMutex);
           p = scanShared(entries, typeid(Obj3));
         }
         return static_cast<const Obj3*>(p.get())->value;
       }},
      {"a-snap",
       [&]() {
         const std::shared_ptr<const std::vector<Entry>> s = snapshot.load();
         const std::shared_ptr<const void> p = scanShared(*s, typeid(Obj3));
         return static_cast<const Obj3*>(p.get())->value;
       }},
      {"c",
       [&]() {
         const void* p;
         {
           std::lock_guard lock(mutex);
           p = scan(entries, typeid(Obj3));
         }
         return static_cast<const Obj3*>(p)->value;
       }},
      {"b", [&]() { return static_cast<const Obj3*>(scan(entries, typeid(Obj3)))->value; }}};

  for (const auto& [name, lookup] : variants) {
    for (const int threads : {1, nThreads}) {
      std::vector<double> times;
      for (int rep = 0; rep < 7; rep++) {
        const double seconds = runThreads(threads, [&](int) {
          size_t local = 0;
          double x = 1.0;
          for (size_t i = 0; i < N; i++) {
            local += lookup();
            if (WORK > 0) x = privateWork(WORK, x);
          }
          sink += local + static_cast<size_t>(x > 1e300);
        });
        times.push_back(seconds / static_cast<double>(N) * 1e9);
      }
      std::printf("%s %-7s threads %2d: %10.1f ns per %s (per thread)\n",
                  spaced ? "spaced" : "lookup", name.c_str(), threads, median(times),
                  spaced ? "iteration" : "lookup");
    }
  }
}

double cosProd(const DataVector& x) {
  double v = 1.0;
  for (size_t k = 0; k < x.size(); k++) v *= std::cos(1.3 * x[k] + 0.1 * static_cast<double>(k));
  return v;
}

struct OpCase {
  std::string name;
  bool linear;
  bool cc;
  size_t nDim;
  cg::LvlType maxLvl;
  cg::LvlType boundary;
};

void opsMode(const int nThreads) {
  const std::vector<OpCase> cases{{"interplin/dsg/equi/b1/d2/L16", true, false, 2, 16, 1},
                                  {"interplin/dsg/equi/b1/d10/L5", true, false, 10, 5, 1},
                                  {"interp/dsg/equi/b0/d3/L5", false, false, 3, 5, 0},
                                  {"interp/dsg/cc/b0/d2/L12", false, true, 2, 12, 0}};
  for (const OpCase& c : cases) {
    cg::CompleteSGGenInstr instr(c.maxLvl, c.nDim);
    instr.setDomain({0.0, 1.0});
    instr.setNodeGenFunc(c.cc ? cg::getClenshawCurtisNodeGenFunc()
                              : cg::getEquidistantNodeGenFunc());
    instr.setLvl2GPCntFunc(cg::doublingLvl2GPCntFunction);
    instr.setBoundaryLevelOffset(c.boundary);
    const cg::DeltasSparseGrid dsg(instr);
    const cg::SourceFunc func(cosProd);
    DataVector point(c.nDim);
    for (size_t k = 0; k < c.nDim; k++) point[k] = 0.137 + 0.071 * static_cast<double>(k);
    const auto call = [&]() {
      return c.linear ? cg::interpolateLinear(func, point, dsg) : cg::interpolate(func, point, dsg);
    };
    call();  // Builds the index

    // Calls per thread: about 0.2 s at 1 thread
    const auto start = std::chrono::steady_clock::now();
    call();
    const double once = std::chrono::duration<double>(std::chrono::steady_clock::now() - start)
                            .count();
    const size_t nCalls = std::max<size_t>(20, static_cast<size_t>(0.2 / std::max(once, 1e-7)));

    for (const int threads : {1, nThreads}) {
      std::vector<double> times;
      for (int rep = 0; rep < 7; rep++) {
        const double seconds = runThreads(threads, [&](int) {
          double local = 0.0;
          for (size_t i = 0; i < nCalls; i++) local += call();
          sink += static_cast<size_t>(local != 0.0);
        });
        times.push_back(seconds / static_cast<double>(nCalls) * 1e6);
      }
      std::printf("ops %-30s callers %2d: %10.3f us per call (per caller)\n", c.name.c_str(),
                  threads, median(times));
    }
  }
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    std::fprintf(stderr, "usage: %s lookup|spaced|ops <threads>\n", argv[0]);
    return 2;
  }
  const int nThreads = std::atoi(argv[2]);
  if (std::strcmp(argv[1], "lookup") == 0 || std::strcmp(argv[1], "spaced") == 0) {
    lookupMode(nThreads, std::strcmp(argv[1], "spaced") == 0);
  } else {
    opsMode(nThreads);
  }
  return sink.load() == 42 ? 1 : 0;
}
