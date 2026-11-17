// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file run_quad.cpp
 * @brief Experiment driver of the adaptive quadrature prototype: one run per call, arguments
 * key=value, output one JSON line per trace point.
 */

#include <sgpp_combigrid.hpp>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "adaptive_proto.hpp"
#include "test_functions.hpp"

using namespace sgpp::combigrid;  // NOLINT(build/namespaces)
using namespace wp62;             // NOLINT(build/namespaces)

namespace {

/// Standard Clenshaw-Curtis growth with boundary offset 1: 1, 3, 5, 9, 17, ... points.
GPCntType shiftedDoublingLvl2GPCntFunction(const LvlType lvl) {
  return lvl == 0 ? 1 : (GPCntType{1} << lvl) - 1;
}

std::map<std::string, std::string> parseArgs(const int argc, char** argv) {
  std::map<std::string, std::string> args = {
      {"func", "gauss"}, {"d", "5"},       {"decay", "alg2"},  {"c", "2"},      {"perm", "0"},
      {"nodes", "cc"},   {"growth", "doubling"}, {"b", "1"},   {"mode", "gg"},  {"profit", "1"},
      {"w", "1"},        {"buffer", "0"},  {"theta", "1"},     {"batch", "1"},  {"budget", "10000"},
      {"store", "delta"}, {"safety", "1"}, {"pred", "geo"},    {"check", "0"},  {"cost_ns", "0"},
      {"tol", "0"},      {"dump", ""},     {"tag", ""},        {"parallel", "1"}, {"trace", "1.05"},
      {"probe_tol", "1e-14"}, {"explore", "0"}, {"cache", "0"}};
  for (int i = 1; i < argc; i++) {
    const std::string arg = argv[i];
    const size_t eq = arg.find('=');
    if (eq == std::string::npos || !args.contains(arg.substr(0, eq))) {
      std::cerr << "bad argument " << arg << "\n";
      std::exit(2);
    }
    args[arg.substr(0, eq)] = arg.substr(eq + 1);
  }
  return args;
}

/// Burns roughly @p ns nanoseconds (emulates an expensive function).
double burn(const double ns) {
  static const double itersPerNs = [] {
    const double t0 = now();
    volatile double x = 1.0;
    for (int i = 0; i < 20000000; i++) x = x * 1.0000001 + 1e-9;
    return 2e7 / ((now() - t0) * 1e9);
  }();
  volatile double x = 1.0;
  const long n = static_cast<long>(ns * itersPerNs);
  for (long i = 0; i < n; i++) x = x * 1.0000001 + 1e-9;
  return x;
}

/// Sets of multi-indices whose product of per-dimension factors is at least eps.
struct ProductSetEnumerator {
  std::vector<std::vector<double>> factors;  ///< Per dimension: factor of level 1, 2, ...
  std::vector<uint32_t> dimOrder;
  std::vector<std::vector<uint32_t>> keys;
  std::vector<double> values;
  double eps = 0;
  size_t limit = 0;

  void dfs(const size_t pos, const double product, std::vector<uint32_t>& key) {
    if (keys.size() > limit) return;
    std::vector<uint32_t> sorted = key;
    std::ranges::sort(sorted);
    keys.push_back(sorted);
    values.push_back(product);
    for (size_t r = pos; r < dimOrder.size(); r++) {
      const uint32_t dim = dimOrder[r];
      if (product * factors[dim][0] < eps) break;
      for (uint32_t lvl = 1; lvl <= factors[dim].size(); lvl++) {
        const double p = product * factors[dim][lvl - 1];
        if (p < eps) break;
        key.push_back(entry(dim, lvl));
        dfs(r + 1, p, key);
        key.pop_back();
      }
    }
  }

  void enumerate(const double epsilon, const size_t maxKeys) {
    eps = epsilon;
    limit = maxKeys;
    keys.clear();
    values.clear();
    dimOrder.resize(factors.size());
    for (uint32_t k = 0; k < factors.size(); k++) dimOrder[k] = k;
    std::ranges::stable_sort(dimOrder, [this](uint32_t x, uint32_t y) {
      return factors[x][0] > factors[y][0];
    });
    std::vector<uint32_t> key;
    dfs(0, 1.0, key);
  }
};

/// Evaluates the axis multi-indices level by level while their increments stay above the
/// tolerance (1D probes of the a-priori variants); returns the relative norms per dimension.
std::vector<std::vector<double>> probeAxes(AdaptiveQuadrature& quad, const double probeTol,
                                           const uint32_t maxLvl) {
  const size_t nDim = quad.nDim;
  std::vector<std::vector<double>> norms(nDim);
  std::vector<std::vector<uint32_t>> keys = {{}};
  quad.runOrdered(keys, {1});
  std::vector<char> open(nDim, 1);
  for (uint32_t lvl = 1; lvl <= maxLvl; lvl++) {
    keys.clear();
    std::vector<uint32_t> dims;
    for (uint32_t dim = 0; dim < nDim; dim++) {
      if (!open[dim]) continue;
      keys.push_back({entry(dim, lvl)});
      dims.push_back(dim);
    }
    if (keys.empty()) break;
    quad.runOrdered(keys, {keys.size()});
    if (quad.budgetExhausted) break;
    for (size_t i = 0; i < keys.size(); i++) {
      const double value = quad.norm[static_cast<size_t>(quad.mis.find(keys[i]))];
      norms[dims[i]].push_back(value);
      if (value <= probeTol) open[dims[i]] = 0;
    }
  }
  return norms;
}

}  // namespace

int main(int argc, char** argv) {
  auto args = parseArgs(argc, argv);
  const size_t nDim = std::stoul(args["d"]);
  const std::string mode = args["mode"];

  NodeGenFunc* gen = nullptr;
  if (args["nodes"] == "cc") gen = getClenshawCurtisNodeGenFunc();
  if (args["nodes"] == "equi") gen = getEquidistantNodeGenFunc();
  if (args["nodes"] == "cheb1") gen = getFirstTypeChebyshevNodeGenFunc();
  Lvl2GPCntFunc growth = doublingLvl2GPCntFunction;
  if (args["growth"] == "linear") growth = linearLvl2GPCntFunction;
  if (args["growth"] == "std") growth = shiftedDoublingLvl2GPCntFunction;
  const LvlType boundaryOffset = static_cast<LvlType>(std::stoul(args["b"]));

  // Functions: one output, or the three-output vector case.
  const std::vector<double> a =
      coefficients(args["decay"], nDim, std::stod(args["c"]), args["perm"] == "1");
  std::vector<std::string> names = {args["func"]};
  if (args["func"] == "vec3") names = {"gauss", "osc", "corner"};
  const double costNs = std::stod(args["cost_ns"]);
  std::atomic<size_t> nCalls{0};
  std::vector<TestFunction> tests;
  std::vector<SourceFunc> sourceFuncs;
  for (const std::string& name : names) tests.push_back(makeFunction(name, a));
  for (const TestFunction& test : tests) {
    if (costNs > 0) {
      sourceFuncs.emplace_back([&test, costNs](const DataVector& x) {
        burn(costNs);
        return test.func(x);
      });
    } else {
      sourceFuncs.emplace_back(test.func);
    }
  }
  std::vector<const SourceFunc*> funcs;
  for (const SourceFunc& f : sourceFuncs) funcs.push_back(&f);
  // cache=1: the first function goes through the library's source-function cache (values are
  // then reused by point instead of by index; combine with store=tg).
  std::unique_ptr<CachedSourceFunc> cached;
  if (args["cache"] == "1") {
    cached = std::make_unique<CachedSourceFunc>(
        [&tests, &nCalls, costNs](const DataVector& x) {
          nCalls.fetch_add(1, std::memory_order_relaxed);
          if (costNs > 0) burn(costNs);
          return tests[0].func(x);
        });
    funcs[0] = cached.get();
  }

  Options opt;
  opt.selection = mode == "lazy" ? Selection::Lazy : Selection::GerstnerGriebel;
  opt.store = args["store"] == "tg" ? Store::TensorGridResults : Store::DeltaValues;
  opt.profit = args["profit"] == "1";
  opt.workWeight = std::stod(args["w"]);
  opt.maxEvals = std::stoul(args["budget"]);
  opt.tolerance = std::stod(args["tol"]);
  opt.buffer = std::stoul(args["buffer"]);
  opt.bulkTheta = std::stod(args["theta"]);
  opt.maxBatch = std::stoul(args["batch"]);
  opt.safety = std::stod(args["safety"]);
  opt.explore = std::stod(args["explore"]);
  opt.traceGrowth = std::stod(args["trace"]);
  opt.parallel = args["parallel"] == "1";
  if (args["pred"] == "max") opt.predictor = Predictor::PairMax;
  if (args["pred"] == "min") opt.predictor = Predictor::PairMin;

  if (costNs > 0) burn(1.0);  // calibrate before timing
  AdaptiveQuadrature quad(nDim, gen, growth, boundaryOffset, funcs, opt);

  if (mode == "gg" || mode == "lazy") {
    quad.run();
  } else if (mode == "level") {  // complete combination technique, level by level
    ProductSetEnumerator sets;
    sets.factors.assign(nDim, std::vector<double>(40));
    for (auto& f : sets.factors) {
      for (size_t l = 0; l < f.size(); l++) f[l] = std::pow(0.5, static_cast<double>(l + 1));
    }
    for (int level = 0; level < 40 && !quad.budgetExhausted; level++) {
      sets.enumerate(std::pow(0.5, level) * 0.999, 4000000);
      if (sets.keys.size() > 4000000) break;
      quad.runOrdered(sets.keys, {sets.keys.size()});
    }
  } else if (mode == "fit" || mode == "qo") {
    // 1D probes, then the sets of the largest predicted product profits (qo) or the weighted
    // level sets with fitted per-dimension rates (fit), in the order of the prediction.
    const auto norms = probeAxes(quad, std::stod(args["probe_tol"]), 12);
    ProductSetEnumerator sets;
    sets.factors.resize(nDim);
    for (size_t dim = 0; dim < nDim; dim++) {
      const std::vector<double>& n = norms[dim];
      std::vector<double>& f = sets.factors[dim];
      double rate = 0.5;
      if (n.size() >= 2 && n.back() > 0 && n.front() > 0) {
        rate = std::pow(n.back() / n.front(), 1.0 / static_cast<double>(n.size() - 1));
      } else if (!n.empty()) {
        rate = std::min(0.5, n.front());
      }
      rate = std::min(rate, 0.9);
      double previous = 1.0;
      for (uint32_t lvl = 1; lvl <= 30; lvl++) {
        double value;
        if (mode == "fit") {
          value = (n.empty() ? 1e-30 : std::min(1.0, n.front())) * std::pow(rate, lvl - 1.0);
        } else {
          const double normValue =
              lvl <= n.size() ? n[lvl - 1]
                              : (n.empty() ? 0.0 : n.back() * std::pow(rate, lvl - n.size()));
          value = normValue / static_cast<double>(quad.rule.cnt(lvl) - quad.rule.cnt(lvl - 1));
        }
        previous = std::min(previous, value);
        f.push_back(std::max(previous, 1e-300));
      }
    }
    double eps = 1e-2;
    for (int it = 0; it < 80; it++, eps *= 0.25) {
      sets.enumerate(eps, 3000000);
      size_t setWork = 0;
      for (const auto& key : sets.keys) {
        size_t w = 1;
        for (const uint32_t e : key) w *= quad.rule.cnt(lvlOf(e)) - quad.rule.cnt(lvlOf(e) - 1);
        setWork += w;
      }
      if (setWork > 2 * opt.maxEvals || sets.keys.size() > 3000000) break;
    }
    std::vector<size_t> order(sets.keys.size());
    for (size_t i = 0; i < order.size(); i++) order[i] = i;
    std::ranges::stable_sort(order, [&sets](size_t x, size_t y) {
      if (sets.values[x] != sets.values[y]) return sets.values[x] > sets.values[y];
      return sets.keys[x].size() < sets.keys[y].size();
    });
    std::vector<std::vector<uint32_t>> keys;
    for (const size_t i : order) keys.push_back(sets.keys[i]);
    std::vector<size_t> chunkEnds;
    for (size_t end = 8; end < keys.size(); end = end + end / 8 + 1) chunkEnds.push_back(end);
    chunkEnds.push_back(keys.size());
    quad.runOrdered(keys, chunkEnds);
  }
  const double wall = now() - quad.startTime;

  // Output: one JSON line per trace point.
  std::string config;
  for (const auto& [key, value] : args) {
    if (key != "dump") config += "\"" + key + "\":\"" + value + "\",";
  }
  for (const TracePoint& tp : quad.trace) {
    double err = 0;
    for (size_t o = 0; o < tests.size(); o++) {
      err = std::max(err, std::abs(tp.estimate[o] - tests[o].exact) / std::abs(tests[o].exact));
    }
    std::printf("{%s\"nI\":%zu,\"nOld\":%zu,\"evals\":%zu,\"tgPoints\":%zu,\"nPred\":%zu,"
                "\"rounds\":%zu,\"err\":%.6e,\"eta\":%.6e,\"wall\":%.6e,\"evalTime\":%.6e}\n",
                config.c_str(), tp.nIndices, tp.nOld, tp.evals, tp.tgPoints, tp.nPredicted,
                tp.rounds, err,
                tp.eta, tp.wall, tp.evalTime);
  }
  std::fprintf(stderr, "%s d=%zu mode=%s: nI=%zu evals=%zu tgPoints=%zu wall=%.4fs eval=%.4fs "
               "book=%.4fs storeMB=%.1f\n",
               args["func"].c_str(), nDim, mode.c_str(), quad.nEvaluated, quad.evals,
               quad.evalTGPoints, wall, quad.evalTime, wall - quad.evalTime,
               static_cast<double>(quad.arena.total) * 8e-6);
  {  // cost of one function evaluation (1 thread), for the bookkeeping shares
    DataVector x(nDim, 0.37);
    const size_t n = costNs > 0 ? 2000 : 200000;
    double sink = 0;
    const double t0 = now();
    for (size_t i = 0; i < n; i++) {
      x[i % nDim] = 0.37 + 1e-9 * static_cast<double>(i);
      sink += sourceFuncs[0].evaluate(x);
    }
    std::fprintf(stderr, "function cost: %.1f ns per evaluation (checksum %.3e); rounds %zu\n",
                 1e9 * (now() - t0) / static_cast<double>(n), sink, quad.rounds);
  }
  if (cached) {
    std::fprintf(stderr, "cache: %zu function calls for %zu tensor-grid points\n",
                 nCalls.exchange(0), quad.evalTGPoints);
  }
  if (quad.nVerified > 0) {
    std::fprintf(stderr, "lazy: verified %zu predictions, %zu surprises (true > 10 x predicted), "
                 "largest true / predicted %.3g, exploration work %zu\n",
                 quad.nVerified, quad.nSurprises, quad.maxSurprise, quad.exploredWork);
  }

  if (!args["dump"].empty()) {  // evaluated multi-indices in evaluation order (dense levels)
    std::ofstream out(args["dump"]);
    for (uint32_t idx = 0; idx < quad.mis.size(); idx++) {
      if (quad.state[idx] == State::Predicted) continue;
      std::vector<unsigned> dense(nDim, 0);
      for (const uint32_t e : quad.mis.key(idx)) dense[dimOf(e)] = lvlOf(e);
      for (size_t k = 0; k < nDim; k++) out << dense[k] << (k + 1 < nDim ? ' ' : '\n');
    }
  }

  std::fflush(stdout);
  if (args["check"] == "1") {
    // Hand-over to the library: the evaluated set as MIVecSGGenInstr -> SparseGrid /
    // DeltasSparseGrid; results, coefficients and evaluation counts must agree.
    std::vector<uint32_t> evaluated;
    for (uint32_t idx = 0; idx < quad.mis.size(); idx++) {
      if (quad.state[idx] != State::Predicted) evaluated.push_back(idx);
    }
    LvlMIVec miVec(nDim, evaluated.size());
    for (size_t i = 0; i < evaluated.size(); i++) {
      LvlMI mi(nDim, 0);
      for (const uint32_t e : quad.mis.key(evaluated[i])) mi[dimOf(e)] = lvlOf(e);
      miVec.setMI(i, mi);
    }
    MIVecSGGenInstr genInstr(miVec);
    genInstr.setDomain({0.0, 1.0});
    genInstr.setNodeGenFunc(gen);
    genInstr.setLvl2GPCntFunc(growth);
    genInstr.setBoundaryLevelOffset(boundaryOffset);
    const SparseGrid sg(genInstr);
    const SourceFunc counted([&](const DataVector& x) {
      nCalls.fetch_add(1, std::memory_order_relaxed);
      return tests[0].func(x);
    });
    const double sgResult = quadrature(counted, sg);
    const size_t sgCalls = nCalls.exchange(0);
    size_t nNonZero = 0, nCoeffMismatch = 0;
    for (const uint32_t idx : evaluated) nNonZero += quad.coeff[idx] != 0;
    for (size_t t = 0; t < sg.nTG(); t++) {
      const auto tg = sg.getUnitTensorGrid(t);
      std::vector<uint32_t> key;
      for (uint32_t k = 0; k < nDim; k++) {
        if (tg.mi[k] > 0) key.push_back(entry(k, tg.mi[k]));
      }
      const int64_t idx = quad.mis.find(key);
      nCoeffMismatch += idx < 0 || quad.coeff[static_cast<size_t>(idx)] != tg.coefficient;
    }
    std::fprintf(stderr, "check: adaptive=%.16e SG=%.16e relDiff=%.2e | TGs %zu vs non-zero "
                 "coefficients %zu, mismatches %zu | SG evaluations %zu\n",
                 quad.estimate[0], sgResult,
                 std::abs(sgResult - quad.estimate[0]) / std::abs(sgResult), sg.nTG(), nNonZero,
                 nCoeffMismatch, sgCalls);
    bool nestedForLibrary = false;
    try {
      nestedForLibrary = genInstr.areNodesOfAllDimsNested();
    } catch (const std::exception& e) {
      std::fprintf(stderr, "check: no DeltasSparseGrid: %s\n", e.what());
    }
    if (nestedForLibrary) {
      const DeltasSparseGrid dsg(genInstr);
      const double dsgResult = quadrature(counted, dsg);
      std::fprintf(stderr, "check: DSG=%.16e relDiff=%.2e | DSG evaluations %zu vs adaptive "
                   "evaluations %zu\n",
                   dsgResult, std::abs(dsgResult - quad.estimate[0]) / std::abs(dsgResult),
                   nCalls.load(), quad.evals);
    }
  }
  return 0;
}
