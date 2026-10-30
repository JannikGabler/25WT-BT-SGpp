// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_cache_counts.cpp
 * @brief WP-25 gain bound of a source-function cache on the e2e problems (counts, no timing).
 *
 * For every e2e configuration (perf_e2e.cpp, same grids, domains and points): the number of
 * source-function evaluations of the operator vs. the number of distinct points, i.e. the upper
 * bound of the speedup of a free cache for an expensive function. Distinct points are counted
 * by recording the 64-bit hash of the bit patterns of every evaluated user-domain point
 * (collision probability < 1e-3 for 1e8 points); for nested node families they also equal the
 * point count of the deltas sparse grid of the same instruction (every point exactly once),
 * which the driver checks where it records. Cross-operator part: on the grids of the
 * interpolation cases (shifted domains), the point sets of SG quadrature, DSG quadrature, global
 * and multilinear interpolation are compared bitwise.
 *
 * Usage: perf_cache_counts [--max-record N] (default 6e7 evaluations: larger cases are counted
 * via the deltas sparse grid only). Output: Markdown tables on stdout.
 */

#include <omp.h>

#include "perf_cache_proto.hpp"  // first: configures Boost.Unordered

#include <boost/unordered/concurrent_flat_set.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <vector>

#include "perf_problems.hpp"

using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::MIVecSGGenInstr;
using sgpp::combigrid::SGGenInstr;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::perf::DataVector;

namespace perf = sgpp::combigrid::perf;
namespace cp = sgpp::combigrid::perf::cache_proto;

namespace {

size_t maxRecord = 60'000'000;

using HashSet = boost::concurrent_flat_set<uint64_t>;

/// Records the hashes of all evaluated points (thread-safe) and counts the evaluations.
struct Recorder {
  HashSet set;
  std::vector<cp::CacheStats::PerThread> counts =
      std::vector<cp::CacheStats::PerThread>(omp_get_max_threads());

  SourceFunc func() {
    return SourceFunc([this](const DataVector& x) {
      counts[omp_get_thread_num()].lookups++;
      set.insert(cp::hashPoint(x.data(), x.size()));
      return 1.0;
    });
  }

  uint64_t evaluations() const {
    uint64_t n = 0;
    for (const auto& c : counts) n += c.lookups;
    return n;
  }
};

size_t intersectionSize(const HashSet& a, const HashSet& b) {
  size_t n = 0;
  a.visit_all([&](const uint64_t h) { n += b.contains(h) ? 1 : 0; });
  return n;
}

template <typename Grid>
size_t sumOfGridPoints(const Grid& grid) {
  size_t sum = 0;
  for (const auto& tgData : grid) sum += tgData.tensorGrid.nGP();
  return sum;
}

/// Distinct points of the instruction = points of its deltas sparse grid (nested families only;
/// cheb1 is not nested and has no quadrature rule, whose exceptions would escape OpenMP regions).
std::optional<size_t> deltaPointCount(const SGGenInstr& genInstr, const std::string& nodes) {
  if (nodes == "cheb1") return std::nullopt;
  return sumOfGridPoints(DeltasSparseGrid(genInstr));
}

double interpolateAt(const bool linear, const SourceFunc& f, const DataVector& point,
                     const SparseGrid& sg) {
  if (linear) return sgpp::combigrid::interpolateLinear(f, point, sg);
  return sgpp::combigrid::interpolate(f, point, sg);
}

std::string fmtCount(const std::optional<size_t> n) {
  return n ? std::to_string(*n) : std::string("n/a");
}

std::string fmtRatio(const double a, const double b) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.2f", a / b);
  return buf;
}

/// SG / DSG quadrature: totals, distinct points, recorded checks (if small enough).
void quadratureRow(const std::string& name, const bool deltas, const std::string& nodes,
                   const LvlType boundary, const size_t nDim, const LvlType maxLvl) {
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
  const std::optional<size_t> distinct = deltaPointCount(genInstr, nodes);
  size_t total = 0;
  std::string recorded = "-";

  if (deltas) {
    total = *distinct;
    if (total <= maxRecord) {
      Recorder rec;
      sgpp::combigrid::quadrature(rec.func(), DeltasSparseGrid(genInstr));
      recorded = std::to_string(rec.evaluations()) + " evals / " + std::to_string(rec.set.size());
    }
  } else {
    const SparseGrid sg(genInstr);
    total = sumOfGridPoints(sg);
    if (total <= maxRecord) {
      Recorder rec;
      sgpp::combigrid::quadrature(rec.func(), sg);
      recorded = std::to_string(rec.set.size());
    }
  }
  std::printf("| %s | %zu | %s | %s | %s |\n", name.c_str(), total, fmtCount(distinct).c_str(),
              recorded.c_str(), fmtRatio(static_cast<double>(total),
                                         static_cast<double>(*distinct)).c_str());
}

/// Interpolation cases: per call and over all points, plus the cross-operator point sets.
void interpolationRow(const std::string& name, const bool linear, const std::string& nodes,
                      const LvlType boundary, const size_t nDim, const LvlType maxLvl,
                      const size_t nPoints) {
  const HyperCubeArea domain = perf::shiftedDomain(nDim);
  CompleteSGGenInstr genInstr(maxLvl, nDim);
  perf::configure(genInstr, nodes, boundary, domain);
  const SparseGrid sg(genInstr);
  const std::vector<DataVector> points = perf::randomPoints(domain, nPoints, 1000 + nDim);

  Recorder interp;  // all calls
  Recorder first;   // first call only
  const SourceFunc interpFunc = interp.func();
  for (const DataVector& point : points) interpolateAt(linear, interpFunc, point, sg);
  interpolateAt(linear, first.func(), points[0], sg);

  // Cross-operator: SG and DSG quadrature on the same grid / domain (n/a without a quad rule)
  std::string quadPts = "n/a", dsgPts = "n/a", interpInQuad = "n/a", dsgEqQuad = "n/a";
  if (nodes != "cheb1") {
    Recorder quad;
    sgpp::combigrid::quadrature(quad.func(), sg);
    quadPts = std::to_string(quad.set.size());
    interpInQuad = std::to_string(intersectionSize(interp.set, quad.set));
    Recorder dsg;
    sgpp::combigrid::quadrature(dsg.func(), DeltasSparseGrid(genInstr));
    dsgPts = std::to_string(dsg.set.size());
    dsgEqQuad = std::to_string(intersectionSize(dsg.set, quad.set));
  }

  const double total = static_cast<double>(interp.evaluations());
  std::printf("| %s | %zu | %zu | %.0f | %zu | %s | %s | %s | %s | %s | %s |\n", name.c_str(),
              sumOfGridPoints(sg), static_cast<size_t>(first.evaluations()), total,
              interp.set.size(),
              fmtRatio(static_cast<double>(first.evaluations()),
                       static_cast<double>(first.set.size()))
                  .c_str(),
              fmtRatio(total, static_cast<double>(interp.set.size())).c_str(), quadPts.c_str(),
              interpInQuad.c_str(), dsgPts.c_str(), dsgEqQuad.c_str());
}

void mivecRow(const std::string& name, const LvlType boundary, const size_t nDim,
              const LvlType maxSum, const size_t nMI) {
  const LvlMIVec mis = perf::anisotropicMIs(nDim, nMI, maxSum, 2000 + nDim);
  MIVecSGGenInstr genInstr(mis);
  perf::configure(genInstr, "cc", boundary, perf::unitDomain(nDim));
  const SparseGrid sg(genInstr);
  const size_t total = sumOfGridPoints(sg);
  const std::optional<size_t> distinct = deltaPointCount(genInstr, "cc");
  std::string recorded = "-";
  if (total <= maxRecord) {
    Recorder rec;
    sgpp::combigrid::quadrature(rec.func(), sg);
    recorded = std::to_string(rec.set.size());
  }
  std::printf("| %s | %zu | %s | %s | %s |\n", name.c_str(), total, fmtCount(distinct).c_str(),
              recorded.c_str(),
              fmtRatio(static_cast<double>(total), static_cast<double>(*distinct)).c_str());
}

std::string qname(const std::string& grid, const std::string& nodes, const LvlType b,
                  const size_t d, const LvlType l) {
  return "quad/" + grid + "/" + nodes + "/b" + std::to_string(b) + "/d" + std::to_string(d) +
         "/L" + std::to_string(l);
}

}  // namespace

int main(int argc, char** argv) {
  for (int i = 1; i + 1 < argc; i++) {
    if (std::string(argv[i]) == "--max-record") maxRecord = std::stoull(argv[i + 1]);
  }
  std::printf("threads: %d, recording up to %zu evaluations per case\n\n", omp_get_max_threads(),
              maxRecord);

  std::printf(
      "## Quadrature (unit domain)\n\n| case | evaluations | distinct (DSG points) | "
      "distinct (recorded; DSG: evals / distinct) | ratio |\n|---|---|---|---|---|\n");
  // e2e cases (integrand irrelevant) + smaller levels of the large ones, recorded exactly
  const struct {
    bool deltas;
    const char* nodes;
    LvlType b;
    size_t d;
    LvlType l;
  } quads[] = {
      {false, "equi", 0, 2, 20}, {false, "equi", 0, 2, 16},  {false, "equi", 0, 5, 10},
      {false, "equi", 0, 5, 8},  {false, "equi", 0, 7, 7},   {false, "equi", 0, 7, 5},
      {false, "equi", 0, 10, 4}, {false, "equi", 0, 10, 3},  {false, "equi", 1, 10, 7},
      {false, "equi", 1, 10, 5}, {false, "equi", 1, 15, 6},  {false, "equi", 1, 15, 5},
      {false, "equi", 1, 20, 5}, {false, "equi", 1, 20, 4},  {false, "cc", 0, 2, 20},
      {false, "cc", 0, 2, 16},   {false, "cc", 0, 3, 14},    {false, "cc", 0, 5, 10},
      {false, "cc", 0, 5, 8},    {false, "cc", 0, 7, 7},     {false, "cc", 0, 7, 5},
      {false, "cc", 0, 10, 4},   {false, "cc", 0, 10, 3},    {true, "equi", 0, 3, 16},
      {true, "equi", 0, 5, 12},  {true, "equi", 0, 7, 10},   {true, "equi", 0, 10, 7},
      {true, "equi", 0, 10, 5},  {true, "equi", 1, 15, 7},   {true, "equi", 1, 15, 6},
      {true, "equi", 1, 20, 6},  {true, "equi", 1, 20, 5},   {true, "cc", 0, 5, 12},
      {true, "cc", 0, 7, 10},    {true, "cc", 0, 10, 7},     {true, "cc", 0, 10, 5},
  };
  for (const auto& q : quads) {
    quadratureRow(qname(q.deltas ? "dsg" : "sg", q.nodes, q.b, q.d, q.l), q.deltas, q.nodes, q.b,
                  q.d, q.l);
  }

  std::printf(
      "\n## Interpolation (shifted domain) and cross-operator point sets on the same grid\n\n"
      "| case | SG points | evals 1st call | evals all calls | distinct all calls | ratio 1st "
      "call | ratio all calls | SG quad distinct | interp in SG quad | DSG quad distinct | DSG in "
      "SG quad |\n|---|---|---|---|---|---|---|---|---|---|---|\n");
  interpolationRow("interp/sg/cc/b0/d2/L12/p64", false, "cc", 0, 2, 12, 64);
  interpolationRow("interp/sg/cc/b0/d3/L9/p64", false, "cc", 0, 3, 9, 64);
  interpolationRow("interp/sg/cc/b0/d5/L6/p64", false, "cc", 0, 5, 6, 64);
  interpolationRow("interp/sg/cc/b0/d7/L4/p16", false, "cc", 0, 7, 4, 16);
  interpolationRow("interp/sg/cc/b0/d10/L2/p8", false, "cc", 0, 10, 2, 8);
  interpolationRow("interp/sg/cheb1/b0/d3/L6/p64", false, "cheb1", 0, 3, 6, 64);
  interpolationRow("interp/sg/cheb1/b0/d5/L4/p64", false, "cheb1", 0, 5, 4, 64);
  interpolationRow("interp/sg/equi/b0/d3/L5/p64", false, "equi", 0, 3, 5, 64);
  interpolationRow("interplin/sg/equi/b1/d2/L16/p16384", true, "equi", 1, 2, 16, 16384);
  interpolationRow("interplin/sg/equi/b1/d5/L8/p1024", true, "equi", 1, 5, 8, 1024);
  interpolationRow("interplin/sg/equi/b1/d8/L5/p256", true, "equi", 1, 8, 5, 256);
  interpolationRow("interplin/sg/equi/b1/d10/L5/p32", true, "equi", 1, 10, 5, 32);

  std::printf(
      "\n## MIVec + quadrature (cc, b1, unit domain)\n\n| case | evaluations | distinct (DSG "
      "points) | distinct (recorded) | ratio |\n|---|---|---|---|---|\n");
  mivecRow("mivec/sg/cc-exp/b1/d5/S16/n1000", 1, 5, 16, 1000);
  mivecRow("mivec/sg/cc-exp/b1/d10/S12/n1000", 1, 10, 12, 1000);
  mivecRow("mivec/sg/cc-exp/b1/d15/S10/n300", 1, 15, 10, 300);
  mivecRow("mivec/sg/cc-exp/b1/d20/S7/n300", 1, 20, 7, 300);
  return 0;
}
