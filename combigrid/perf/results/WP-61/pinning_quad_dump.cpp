// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-61: prints the SG quadrature values (scalar + 3 vector outputs, 1 thread) of the
// configurations of tests/grids/grid_storage_pinning_test.cpp (helpers copied from there), to
// compare the values before / after the change of the combination (re-pinned hashes).
// Build (repo root): g++ -O2 -std=c++23 -fopenmp -Icombigrid/src -Ibase/src
//   combigrid/perf/results/WP-61/pinning_quad_dump.cpp -L<snapshot>/lib -lsgppcombigrid
//   -lsgppbase -o <scratch>/pinning_quad_dump; run with LD_LIBRARY_PATH=<snapshot>/lib.
#include <omp.h>

#include <cmath>
#include <cstdio>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <span>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {
constexpr size_t N_OUTPUTS = 3;
/// Non-polynomial, not separable test function.
double testFunc(const DataVector& x) {
  double sqDist = 0.0;
  for (size_t i = 0; i < x.size(); i++) {
    const double c = 0.3 + 0.1 * static_cast<double>(i);
    sqDist += (x[i] - c) * (x[i] - c);
  }
  return std::exp(-sqDist) * (1.0 + 0.5 * x[0]) + 0.25 * x[x.size() - 1];
}

void testFuncVec(const DataVector& x, const std::span<double> out) {
  out[0] = testFunc(x);
  double prod = 1.0;
  for (size_t i = 0; i < x.size(); i++) prod *= std::cos(x[i] + 0.1 * static_cast<double>(i));
  out[1] = prod;
  out[2] = 2.5 - x[0];
}

/// Dimension i: [-0.5 + 0.1 i, 1.0 + 0.2 i].
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t i = 0; i < nDim; i++) {
    domain[i] = {-0.5 + 0.1 * static_cast<double>(i), 1.0 + 0.2 * static_cast<double>(i)};
  }
  return domain;
}

struct Config {
  std::string nodes;  // equi, cc, cheb1 or mixed (cc in even, equi in odd dimensions)
  LvlType boundary;
  bool unitDomain;
  size_t nDim;
  bool mivec;
};

std::string toString(const Config& c) {
  return c.nodes + " b" + std::to_string(c.boundary) + (c.unitDomain ? " unit" : " shifted") +
         " d" + std::to_string(c.nDim) + (c.mivec ? " mivec" : " complete");
}

std::vector<Config> configs() {
  std::vector<Config> result;
  for (const std::string nodes : {"equi", "cc", "cheb1", "mixed"}) {
    for (const LvlType boundary : {0u, 1u, 2u}) {
      for (const bool unitDomain : {true, false}) {
        for (size_t nDim = 1; nDim <= 3; nDim++) {
          for (const bool mivec : {false, true}) {
            result.push_back({nodes, boundary, unitDomain, nDim, mivec});
          }
        }
      }
    }
  }
  return result;
}

/// Anisotropic MIs: 3 e_0, (1, ..., 1), 2 e_{d-1}, (2, 1, 0, ...) for d >= 2.
LvlMIVec anisotropicMIs(const size_t nDim) {
  LvlMI first(nDim, 0), ones(nDim, 1), last(nDim, 0), mixed(nDim, 0);
  first[0] = 3;
  last[nDim - 1] = 2;
  std::vector<LvlMI> mis{first, ones, last};
  if (nDim >= 2) {
    mixed[0] = 2;
    mixed[1] = 1;
    mis.push_back(mixed);
  }
  return LvlMIVec(mis);
}

std::unique_ptr<SGGenInstr> makeGenInstr(const Config& config, LvlMIVec& miVec) {
  std::unique_ptr<SGGenInstr> genInstr;
  if (config.mivec) {
    miVec = anisotropicMIs(config.nDim);
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(3, config.nDim);
  }
  for (size_t dim = 0; dim < config.nDim; dim++) {
    NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();
    if (config.nodes == "cc" || (config.nodes == "mixed" && dim % 2 == 0)) {
      nodeGenFunc = getClenshawCurtisNodeGenFunc();
    } else if (config.nodes == "cheb1") {
      nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
    }
    genInstr->setNodeGenFuncForDim(nodeGenFunc, dim);
  }
  genInstr->setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(config.boundary);
  genInstr->setDomain(config.unitDomain ? HyperCubeArea(config.nDim, {0.0, 1.0})
                                        : shiftedDomain(config.nDim));
  return genInstr;
}

}  // namespace

int main() {
  omp_set_num_threads(1);
  for (const Config& config : configs()) {
    if (config.nodes == "cheb1") continue;  // no SG quadrature in the pinning test
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid grid(*genInstr);
    const SourceFunc f(testFunc);
    const VectorSourceFunc fVec(testFuncVec, N_OUTPUTS);
    const double scalar = quadrature(f, grid);
    const DataVector vec = quadrature(fVec, grid);
    std::printf("%s sg|%.17g|%.17g|%.17g|%.17g\n", toString(config).c_str(), scalar, vec[0], vec[1],
                vec[2]);
  }
}
