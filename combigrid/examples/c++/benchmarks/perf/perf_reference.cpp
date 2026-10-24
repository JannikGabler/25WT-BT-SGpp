// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_reference.cpp
 * @brief Reference outputs of the combigrid performance suite, used to verify optimized code.
 *
 * PROTECTED FILE (see combigrid/perf/README.md). Uses the public API of the module only.
 *
 * Prints one JSON object per case (JSON lines). Every case contains
 * - @c values / @c exact:  numerical results (quadrature value, interpolated values) and their
 *                          exact values; the reference check compares the @em errors,
 * - @c magnitude, @c sumAbsCoeffs, @c sumGP: quantities for the round-off floor of the check,
 * - @c ints:   integer data that must not change (number of tensor grids, number of grid points,
 *              order-independent hash of all (level multi-index, coefficient) pairs),
 * - @c floats: checksum of all node positions (relative tolerance),
 * - @c info:   informative data whose change is reported but allowed (number of evaluations).
 *
 * Usage: perf_reference [--list] [--filter REGEX] [--threads N]
 * The check is performed by combigrid/perf/scripts/perf.py refcheck.
 */

#include <omp.h>

#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <optional>
#include <regex>
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
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "perf_harness.hpp"
#include "perf_problems.hpp"

using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::CTCoeffType;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::MIVecSGGenInstr;
using sgpp::combigrid::SGGenInstr;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::perf::DataVector;
using sgpp::combigrid::perf::fmtDouble;
using sgpp::combigrid::perf::jsonString;

namespace perf = sgpp::combigrid::perf;

namespace {

uint64_t mix(uint64_t z) {
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}

std::string hex(const uint64_t value) {
  static const char* const digits = "0123456789abcdef";
  std::string out(16, '0');
  for (int i = 15; i >= 0; i--) out[static_cast<size_t>(15 - i)] = digits[(value >> (4 * i)) & 0xF];
  return out;
}

/// Data of one reference case.
struct Record {
  std::string name;
  std::string op;
  std::vector<double> values;
  std::vector<double> exact;
  double magnitude = 1.0;
  double sumAbsCoeffs = 0.0;
  double sumGP = 0.0;
  std::vector<std::pair<std::string, std::string>> ints;  // values as strings
  std::vector<std::pair<std::string, double>> floats;
  std::vector<std::pair<std::string, double>> info;

  std::string json() const {
    auto array = [](const std::vector<double>& v) {
      std::string s = "[";
      for (size_t i = 0; i < v.size(); i++) s += (i ? "," : "") + fmtDouble(v[i]);
      return s + "]";
    };
    std::string s = "{\"case\":" + jsonString(name) + ",\"op\":" + jsonString(op) +
                    ",\"values\":" + array(values) + ",\"exact\":" + array(exact) +
                    ",\"magnitude\":" + fmtDouble(magnitude) +
                    ",\"sumAbsCoeffs\":" + fmtDouble(sumAbsCoeffs) +
                    ",\"sumGP\":" + fmtDouble(sumGP) + ",\"ints\":{";
    for (size_t i = 0; i < ints.size(); i++) {
      s += (i ? "," : "") + jsonString(ints[i].first) + ":" + jsonString(ints[i].second);
    }
    s += "},\"floats\":{";
    for (size_t i = 0; i < floats.size(); i++) {
      s += (i ? "," : "") + jsonString(floats[i].first) + ":" + fmtDouble(floats[i].second);
    }
    s += "},\"info\":{";
    for (size_t i = 0; i < info.size(); i++) {
      s += (i ? "," : "") + jsonString(info[i].first) + ":" + fmtDouble(info[i].second);
    }
    return s + "}}";
  }
};

/// Structure data of a (deltas) sparse grid: independent of the order of the tensor grids.
template <typename Grid>
void addStructure(Record& record, const Grid& grid) {
  uint64_t tgHash = 0;
  size_t sumGP = 0;
  double sumAbsCoeffs = 0.0;
  double nodeChecksum = 0.0;

  for (const auto& tgData : grid) {
    uint64_t h = 0x243f6a8885a308d3ULL;
    for (const LvlType lvl : tgData.mi) h = mix(h ^ (static_cast<uint64_t>(lvl) + 1));
    h = mix(h ^ static_cast<uint64_t>(static_cast<int64_t>(tgData.coefficient) + (1LL << 32)));
    tgHash += h;

    sumGP += tgData.tensorGrid.nGP();
    sumAbsCoeffs += std::abs(static_cast<double>(tgData.coefficient));

    const auto& nPerDim = tgData.tensorGrid.getGPCntPerDim();
    const DataVector& nodes = tgData.tensorGrid.getNodesPerDim();
    size_t offset = 0;
    for (size_t k = 0; k < nPerDim.size(); k++) {
      for (size_t i = 0; i < nPerDim[k]; i++) {
        nodeChecksum += nodes[offset + i] * static_cast<double>((k + 1) * (i % 7 + 1));
      }
      offset += nPerDim[k];
    }
  }

  record.sumGP = static_cast<double>(sumGP);
  record.sumAbsCoeffs = sumAbsCoeffs;
  record.ints.emplace_back("nTG", std::to_string(grid.nTG()));
  record.ints.emplace_back("sumGP", std::to_string(sumGP));
  record.ints.emplace_back("tgHash", hex(tgHash));
  record.floats.emplace_back("nodeChecksum", nodeChecksum);
}

/// Source function that counts its evaluations.
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

using CaseFunc = std::function<Record()>;

std::string cfg(const std::string& nodes, const LvlType boundary, const size_t nDim,
                const LvlType maxLvl) {
  return nodes + "/b" + std::to_string(boundary) + "/d" + std::to_string(nDim) + "/L" +
         std::to_string(maxLvl);
}

struct Integrand {
  std::string name;
  double (*func)(const DataVector&);
  double (*exact)(const HyperCubeArea&);
  bool shifted;  // shifted domain (else unit cube)
};

double uqExact(const HyperCubeArea& domain) { return perf::uqIntegral(domain.size()); }

const Integrand EXP_SHIFTED{"exp-shifted", perf::expSum, perf::expSumIntegral, true};
const Integrand SIN_UNIT{"sin-unit", perf::sinProd, perf::sinProdIntegral, false};
const Integrand UQ_UNIT{"uq-unit", perf::uqIntegrand, uqExact, false};

template <typename Grid>
CaseFunc quadCase(const std::string& name, const std::string& nodes, const LvlType boundary,
                  const size_t nDim, const LvlType maxLvl, const Integrand& integrand) {
  return [=]() {
    const HyperCubeArea domain =
        integrand.shifted ? perf::shiftedDomain(nDim) : perf::unitDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, boundary, domain);
    const Grid grid(genInstr);
    const CountingFunc counting{integrand.func};

    Record record;
    record.name = name;
    record.op = "quad";
    record.values.push_back(quadrature(counting.sourceFunc(), grid));
    record.exact.push_back(integrand.exact(domain));
    record.magnitude = std::abs(record.exact[0]);
    addStructure(record, grid);
    record.info.emplace_back("nEvals", static_cast<double>(counting.count->load()));
    return record;
  };
}

CaseFunc mivecCase(const std::string& name, const std::string& nodes, const LvlType boundary,
                   const size_t nDim, const LvlType maxSum, const size_t nMI) {
  return [=]() {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    const LvlMIVec mis = perf::anisotropicMIs(nDim, nMI, maxSum, 3000 + nDim);
    MIVecSGGenInstr genInstr(mis);
    perf::configure(genInstr, nodes, boundary, domain);
    const SparseGrid grid(genInstr);
    const CountingFunc counting{perf::expSum};

    Record record;
    record.name = name;
    record.op = "quad";
    record.values.push_back(quadrature(counting.sourceFunc(), grid));
    record.exact.push_back(perf::expSumIntegral(domain));
    record.magnitude = std::abs(record.exact[0]);
    addStructure(record, grid);
    record.info.emplace_back("nEvals", static_cast<double>(counting.count->load()));

    // Coefficients of the full downwards closure (including zero coefficients)
    const LvlMIVec closure = LvlMIVec(mis).downwardsClosure();
    const std::vector<CTCoeffType> coeffs = sgpp::combigrid::tools::computeCTCoeffs(closure);
    uint64_t closureHash = 0;
    for (size_t i = 0; i < closure.nMI(); i++) {
      uint64_t h = 0x13198a2e03707344ULL;
      for (size_t k = 0; k < nDim; k++) h = mix(h ^ (static_cast<uint64_t>(closure[i][k]) + 1));
      closureHash += mix(h ^ static_cast<uint64_t>(static_cast<int64_t>(coeffs[i]) + (1LL << 32)));
    }
    record.ints.emplace_back("closureSize", std::to_string(closure.nMI()));
    record.ints.emplace_back("closureCoeffHash", hex(closureHash));
    return record;
  };
}

CaseFunc interpCase(const std::string& name, const bool linear, const std::string& nodes,
                    const LvlType boundary, const size_t nDim, const LvlType maxLvl) {
  return [=]() {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, boundary, domain);
    const SparseGrid grid(genInstr);
    const CountingFunc counting{perf::cosProd};
    const SourceFunc sourceFunc = counting.sourceFunc();

    Record record;
    record.name = name;
    record.op = linear ? "interplin" : "interp";
    for (const DataVector& point : perf::randomPoints(domain, 8, 4000 + nDim)) {
      record.values.push_back(linear ? interpolateLinear(sourceFunc, point, grid)
                                     : interpolate(sourceFunc, point, grid));
      record.exact.push_back(perf::cosProd(point));
    }
    record.magnitude = 1.0;  // max |cosProd|
    addStructure(record, grid);
    record.info.emplace_back("nEvals", static_cast<double>(counting.count->load()));
    return record;
  };
}

std::vector<std::pair<std::string, CaseFunc>> allCases() {
  std::vector<std::pair<std::string, CaseFunc>> cases;
  auto add = [&](const std::string& name, CaseFunc func) { cases.emplace_back(name, func); };

  const std::vector<std::pair<size_t, LvlType>> quadSizes{{1, 9}, {2, 8}, {3, 7}, {5, 5}, {8, 3}};
  for (const std::string nodes : {"equi", "cc"}) {
    for (const LvlType boundary : {0u, 1u}) {
      for (const auto& [nDim, maxLvl] : quadSizes) {
        for (const Integrand* integrand : {&EXP_SHIFTED, &SIN_UNIT}) {
          const std::string name =
              "quad/sg/" + cfg(nodes, boundary, nDim, maxLvl) + "/" + integrand->name;
          add(name, quadCase<SparseGrid>(name, nodes, boundary, nDim, maxLvl, *integrand));
        }
        const std::string name =
            "quad/dsg/" + cfg(nodes, boundary, nDim, maxLvl) + "/" + EXP_SHIFTED.name;
        add(name, quadCase<DeltasSparseGrid>(name, nodes, boundary, nDim, maxLvl, EXP_SHIFTED));
      }
    }
  }
  for (const auto& [nDim, maxLvl] : std::vector<std::pair<size_t, LvlType>>{{2, 10}, {5, 6},
                                                                            {10, 3}}) {
    const std::string sgName = "quad/sg/" + cfg("cc", 0, nDim, maxLvl) + "/" + UQ_UNIT.name;
    add(sgName, quadCase<SparseGrid>(sgName, "cc", 0, nDim, maxLvl, UQ_UNIT));
    const std::string dsgName = "quad/dsg/" + cfg("cc", 0, nDim, maxLvl) + "/" + UQ_UNIT.name;
    add(dsgName, quadCase<DeltasSparseGrid>(dsgName, "cc", 0, nDim, maxLvl, UQ_UNIT));
  }

  for (const std::string nodes : {"equi", "cc"}) {
    for (const auto& [nDim, maxSum, nMI] :
         std::vector<std::tuple<size_t, LvlType, size_t>>{{3, 8, 40}, {6, 7, 80}, {10, 6, 60}}) {
      const std::string name = "mivec/sg/" + nodes + "/b1/d" + std::to_string(nDim) + "/S" +
                               std::to_string(maxSum) + "/n" + std::to_string(nMI) +
                               "/exp-shifted";
      add(name, mivecCase(name, nodes, 1, nDim, maxSum, nMI));
    }
  }

  const std::vector<std::pair<size_t, LvlType>> interpSizes{{1, 6}, {2, 5}, {3, 4}, {5, 3}};
  for (const auto& [nodes, boundary] : std::vector<std::pair<std::string, LvlType>>{
           {"cc", 0}, {"cc", 1}, {"cheb1", 0}, {"equi", 0}}) {
    for (const auto& [nDim, maxLvl] : interpSizes) {
      const std::string name = "interp/sg/" + cfg(nodes, boundary, nDim, maxLvl) + "/cos-shifted";
      add(name, interpCase(name, false, nodes, boundary, nDim, maxLvl));
    }
  }
  const std::vector<std::pair<size_t, LvlType>> linearSizes{{1, 6}, {2, 6}, {3, 5}, {5, 4}};
  for (const std::string nodes : {"equi", "cc"}) {
    for (const LvlType boundary : {0u, 1u}) {
      for (const auto& [nDim, maxLvl] : linearSizes) {
        const std::string name =
            "interplin/sg/" + cfg(nodes, boundary, nDim, maxLvl) + "/cos-shifted";
        add(name, interpCase(name, true, nodes, boundary, nDim, maxLvl));
      }
    }
  }
  return cases;
}

}  // namespace

int main(int argc, char** argv) {
  bool list = false;
  std::optional<std::regex> filter;

  for (int i = 1; i < argc; i++) {
    const std::string arg = argv[i];
    if (arg == "--list") {
      list = true;
    } else if (arg == "--filter" && i + 1 < argc) {
      filter.emplace(argv[++i]);
    } else if (arg == "--threads" && i + 1 < argc) {
      omp_set_num_threads(std::stoi(argv[++i]));
    } else if (arg == "--build-info") {
      std::printf("%s\n", perf::buildInfoJson().c_str());
      return 0;
    } else {
      std::fprintf(stderr, "Usage: perf_reference [--list] [--filter REGEX] [--threads N]\n");
      return 2;
    }
  }

  for (const auto& [name, func] : allCases()) {
    if (filter && !std::regex_search(name, *filter)) continue;
    if (list) {
      std::printf("%s\n", name.c_str());
    } else {
      std::printf("%s\n", func().json().c_str());
      std::fflush(stdout);
    }
  }
  return 0;
}
