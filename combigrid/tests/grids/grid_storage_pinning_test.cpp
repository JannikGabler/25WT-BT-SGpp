// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file grid_storage_pinning_test.cpp
 * @brief Boost.Test cases pinning what the operators evaluate and compute on @c SparseGrid and
 * @c DeltasSparseGrid, independently of how the grids store their tensor grids and nodes.
 *
 * For every configuration (equi / cc / cheb1 / mixed node families per dimension, boundary level
 * offsets 0 / 1 / 2, unit and shifted domains, d = 1-3, complete and anisotropic multi-index
 * grids) and both grid types (the deltas sparse grid only for nested nodes), all operators are
 * applied with scalar and vector-valued source functions at one thread. Golden values recorded
 * with the tensor-grid storage of per-tensor-grid node lists:
 * - an order-independent hash of the bit patterns of every evaluated point (with multiplicity)
 *   over all operator calls: the operators must evaluate bitwise identical points;
 * - a hash of the bit patterns of the results that are reproducible bitwise (the quadrature on
 *   both grid types, all operators on the deltas sparse grid), one for the debug build and one
 *   for the optimized build (they differ, e.g. by contracted multiply-adds);
 * - the results of the global and the linear interpolation on the sparse grid, which are pinned
 *   up to round-off (relative 1e-12 of the magnitude of the source function).
 *
 * Set the environment variable @c SGPP_PRINT_GOLDEN=1 to print the values of the current code
 * instead of checking them.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
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
#include <span>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// Number of outputs of the vector-valued source function.
constexpr size_t N_OUTPUTS = 3;

uint64_t mix(uint64_t h) {
  h ^= h >> 33;
  h *= 0xff51afd7ed558ccdULL;
  h ^= h >> 33;
  h *= 0xc4ceb9fe1a85ec53ULL;
  h ^= h >> 33;
  return h;
}

/// Hash of the bit pattern of @p x.
uint64_t hashOf(const double x) { return mix(std::bit_cast<uint64_t>(x) + 0x9e3779b97f4a7c15ULL); }

/// Hash of the bit patterns of the coordinates of @p point (order of the coordinates matters).
uint64_t hashOf(const DataVector& point) {
  uint64_t h = 0x243f6a8885a308d3ULL;
  for (const double x : point) h = mix(h ^ hashOf(x));
  return h;
}

/// Order-independent hash of the evaluated points (sum of the point hashes) and their number.
struct PointRecorder {
  std::atomic<uint64_t> sum{0};
  std::atomic<uint64_t> count{0};

  void record(const DataVector& x) {
    sum.fetch_add(hashOf(x), std::memory_order_relaxed);
    count.fetch_add(1, std::memory_order_relaxed);
  }
  uint64_t hash() const { return mix(sum.load() ^ mix(count.load())); }
};

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

/// Query points: a generic interior point and one with coordinates on the lower boundary, the
/// midpoint (a node of every nested family) and the upper boundary.
std::vector<DataVector> testPoints(const HyperCubeArea& domain) {
  const size_t nDim = domain.size();
  std::vector<DataVector> points(2, DataVector(nDim));
  for (size_t i = 0; i < nDim; i++) {
    const double lo = domain[i].first, width = domain[i].second - domain[i].first;
    points[0][i] = lo + width * std::fmod(0.137 + 0.311 * static_cast<double>(i), 1.0);
    points[1][i] = (i % 3 == 0) ? lo : (i % 3 == 1) ? lo + 0.5 * width : domain[i].second;
  }
  return points;
}

/// Golden values of one (configuration, grid type).
struct Golden {
  uint64_t pointHash;          ///< Hash of all evaluated points of all operator calls.
  uint64_t bitwiseHash;        ///< Hash of the bitwise reproducible results (debug build).
  uint64_t bitwiseHashOpt;     ///< The same for the optimized build (-O3, -march=native, ...).
  std::vector<double> interp;  ///< Sparse grid: interpolate, interpolateLinear (scalar) per point.
};

/// Hash accumulator of results (order matters).
struct ResultHash {
  uint64_t h = 0x13198a2e03707344ULL;
  void add(const double x) { h = mix(h ^ hashOf(x)); }
  void add(const DataVector& v) {
    for (const double x : v) add(x);
  }
};

/// Applies all operators to @p grid (SparseGrid or DeltasSparseGrid) at one thread (the
/// quadrature only if @p withQuadrature: first-kind Chebyshev nodes have no quadrature rule).
template <typename Grid>
Golden evaluate(const Grid& grid, const HyperCubeArea& domain, const bool isSparseGrid,
                const bool withQuadrature) {
  PointRecorder recorder;
  const SourceFunc f([&recorder](const DataVector& x) {
    recorder.record(x);
    return testFunc(x);
  });
  const VectorSourceFunc fVec(
      [&recorder](const DataVector& x, const std::span<double> out) {
        recorder.record(x);
        testFuncVec(x, out);
      },
      N_OUTPUTS);

  Golden golden{};
  ResultHash bitwise;
  if (withQuadrature) {
    bitwise.add(quadrature(f, grid));
    bitwise.add(quadrature(fVec, grid));
  }

  for (const DataVector& point : testPoints(domain)) {
    const double interp = interpolate(f, point, grid);
    const DataVector interpVec = interpolate(fVec, point, grid);
    const double interpLin = interpolateLinear(f, point, grid);
    const DataVector interpLinVec = interpolateLinear(fVec, point, grid);

    if (isSparseGrid) {
      golden.interp.push_back(interp);
      golden.interp.push_back(interpLin);
    } else {
      bitwise.add(interp);
      bitwise.add(interpVec);
      bitwise.add(interpLin);
      bitwise.add(interpLinVec);
    }
  }

  golden.pointHash = recorder.hash();
  golden.bitwiseHash = bitwise.h;
  return golden;
}

/// Runs @p check(name, golden of the current code) for every configuration and grid type at one
/// thread.
template <typename Check>
void forAllGrids(const Check& check) {
  const int nThreads = omp_get_max_threads();
  omp_set_num_threads(1);

  for (const Config& config : configs()) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const HyperCubeArea& domain = genInstr->getDomain();

    const bool nested = (config.nodes != "cheb1");
    check(toString(config) + " sg", evaluate(SparseGrid(*genInstr), domain, true, nested));
    if (nested) {
      check(toString(config) + " dsg", evaluate(DeltasSparseGrid(*genInstr), domain, false, true));
    }
  }

  omp_set_num_threads(nThreads);
}

// clang-format off
/// Golden values (see the file description), in the order of forAllGrids.
const std::vector<Golden> GOLDEN = {
    // equi b0 unit d1 complete sg
    {0xafd0444e2c4f11ffULL, 0x2a5aa45e23bd97aaULL, 0x8eaa03361308f51cULL,
     {1.074734838907456, 1.0742158842947549,
      0.91393118527122819, 0.91393118527122819}},
    // equi b0 unit d1 complete dsg
    {0x0b9251bcc5d86ea3ULL, 0xd6d122a55b90135cULL, 0x0d49865b0c92933bULL,
     {}},
    // equi b0 unit d1 mivec sg
    {0xafd0444e2c4f11ffULL, 0x2a5aa45e23bd97aaULL, 0x8eaa03361308f51cULL,
     {1.074734838907456, 1.0742158842947549,
      0.91393118527122819, 0.91393118527122819}},
    // equi b0 unit d1 mivec dsg
    {0x0b9251bcc5d86ea3ULL, 0xd6d122a55b90135cULL, 0x0d49865b0c92933bULL,
     {}},
    // equi b0 unit d2 complete sg
    {0x46d0f53605280f06ULL, 0x85e304400aef7d2aULL, 0x17bd04a04cfe8358ULL,
     {1.1500903106344462, 1.1487735336360487,
      1.0298374180359597, 1.0298374180359597}},
    // equi b0 unit d2 complete dsg
    {0xd05ce85eb4fa4df7ULL, 0x9c4e43ad29ac6759ULL, 0x85812487f39d5a3aULL,
     {}},
    // equi b0 unit d2 mivec sg
    {0x6e3c2b0c4be03691ULL, 0x0a69f6f6b962406fULL, 0xfca7394212a61800ULL,
     {1.1500908511320058, 1.1453391189133109,
      1.0298374180359597, 1.0298374180359597}},
    // equi b0 unit d2 mivec dsg
    {0x677f6629b60dad7cULL, 0xc9ef95bb01e00c6bULL, 0x8a52f96eae23f176ULL,
     {}},
    // equi b0 unit d3 complete sg
    {0x41dbd6ec81bbbdd7ULL, 0xaa0603fc089413f7ULL, 0x31b3ac15209c8183ULL,
     {1.1604884409687153, 1.1586151205975486,
      0.95468808971871366, 0.95468808971871366}},
    // equi b0 unit d3 complete dsg
    {0x62834c9b50052f6bULL, 0x2a4fc376aaf74584ULL, 0xc480b0b158be56edULL,
     {}},
    // equi b0 unit d3 mivec sg
    {0x63ca960f7ae108daULL, 0x9392025e8c1eb9a2ULL, 0x7096e263e0d53546ULL,
     {1.1604291397946009, 1.1486522184045125,
      0.95468808971871344, 0.95468808971871344}},
    // equi b0 unit d3 mivec dsg
    {0x6835ea4326a605ebULL, 0x2af628f4b62359b1ULL, 0x6a2efee87eb199d6ULL,
     {}},
    // equi b0 shifted d1 complete sg
    {0x545823869876c64cULL, 0x773dc47d53d41eecULL, 0x773dc47d53d41eecULL,
     {0.52523998455448828, 0.52551648548917407,
      0.27046931803228641, 0.27046931803228641}},
    // equi b0 shifted d1 complete dsg
    {0xd4deaf15261860adULL, 0x38b316895a386d8dULL, 0xfd3dc005b95d81d5ULL,
     {}},
    // equi b0 shifted d1 mivec sg
    {0x545823869876c64cULL, 0x773dc47d53d41eecULL, 0x773dc47d53d41eecULL,
     {0.52523998455448828, 0.52551648548917407,
      0.27046931803228641, 0.27046931803228641}},
    // equi b0 shifted d1 mivec dsg
    {0xd4deaf15261860adULL, 0x38b316895a386d8dULL, 0xfd3dc005b95d81d5ULL,
     {}},
    // equi b0 shifted d2 complete sg
    {0x179c45e69dfb55ceULL, 0xbb27b342a9b55ddbULL, 0x08bf6f6d1856d09dULL,
     {0.67393439877871686, 0.67346960102245945,
      0.49546931803228644, 0.49546931803228644}},
    // equi b0 shifted d2 complete dsg
    {0xfd9c6b52171b3076ULL, 0xa1dec167d0b1f239ULL, 0x692b3855ebaed894ULL,
     {}},
    // equi b0 shifted d2 mivec sg
    {0xba0fc6c9a1d0fadbULL, 0xcb91c74310c75bc7ULL, 0xaba8ca22e6942640ULL,
     {0.67392572933489003, 0.66850527773653423,
      0.49546931803228644, 0.49546931803228644}},
    // equi b0 shifted d2 mivec dsg
    {0xe758a636cec0b23dULL, 0xc7a4dffce02b7c0bULL, 0xe25a63c0f6fc485aULL,
     {}},
    // equi b0 shifted d3 complete sg
    {0x4aeca3d38970c02aULL, 0xdf83c19f4329c918ULL, 0xbd5da88c931c1717ULL,
     {0.71522318691504483, 0.71420425185295278,
      0.52592771607034816, 0.52592771607034816}},
    // equi b0 shifted d3 complete dsg
    {0xe7add6891e11e4a9ULL, 0x226d18f7cac0a1dfULL, 0x0325f051913b686fULL,
     {}},
    // equi b0 shifted d3 mivec sg
    {0x142aaa53e1638eccULL, 0xffadb174a32c89f4ULL, 0x53f52162f86bd3b4ULL,
     {0.71563344670579188, 0.70429981820528287,
      0.52592771607034816, 0.52592771607034816}},
    // equi b0 shifted d3 mivec dsg
    {0x55ab8387ea916b0eULL, 0x0df6d0b6ff4127baULL, 0x2639d46a9ddf3b4dULL,
     {}},
    // equi b1 unit d1 complete sg
    {0xafd0444e2c4f11ffULL, 0x2a5aa45e23bd97aaULL, 0x8eaa03361308f51cULL,
     {1.074734838907456, 1.0742158842947549,
      0.91393118527122819, 0.91393118527122819}},
    // equi b1 unit d1 complete dsg
    {0x0b9251bcc5d86ea3ULL, 0xdfc617247bc15fb3ULL, 0x7fb7ee3f53f28c36ULL,
     {}},
    // equi b1 unit d1 mivec sg
    {0xafd0444e2c4f11ffULL, 0x2a5aa45e23bd97aaULL, 0x8eaa03361308f51cULL,
     {1.074734838907456, 1.0742158842947549,
      0.91393118527122819, 0.91393118527122819}},
    // equi b1 unit d1 mivec dsg
    {0x0b9251bcc5d86ea3ULL, 0xdfc617247bc15fb3ULL, 0x7fb7ee3f53f28c36ULL,
     {}},
    // equi b1 unit d2 complete sg
    {0x3fc24f5949b6c0acULL, 0xf2c02b0458bd6681ULL, 0xe7704cdcf1e074d3ULL,
     {1.1500903120975128, 1.1487735336360487,
      1.0298374180359597, 1.0298374180359597}},
    // equi b1 unit d2 complete dsg
    {0x2a914b360909ae54ULL, 0x7db289e164df5a70ULL, 0x24f73c1053e7b551ULL,
     {}},
    // equi b1 unit d2 mivec sg
    {0xa8f14efe4bc6a89cULL, 0x8af17fb488213024ULL, 0xc164bdd5d69ddc39ULL,
     {1.1501002633420183, 1.1453391189133109,
      1.0298374180359597, 1.0298374180359597}},
    // equi b1 unit d2 mivec dsg
    {0x46250fc89b1ad6e5ULL, 0x8b2ff7a31a6b0d0eULL, 0x485e9decc4cfd390ULL,
     {}},
    // equi b1 unit d3 complete sg
    {0xcab69392681afb09ULL, 0x350b726b5ffcbc6aULL, 0x4d9fe9d7d812cb1aULL,
     {1.1604878936794611, 1.1586151205975486,
      0.954688089718712, 0.95468808971871366}},
    // equi b1 unit d3 complete dsg
    {0x5757f50d757ea1a7ULL, 0x47a0fe1eeb3f86a2ULL, 0x40a8812dc071676aULL,
     {}},
    // equi b1 unit d3 mivec sg
    {0x0e25bba71987fea5ULL, 0x89a01fa5a2880ab7ULL, 0x0089b4d36cf880d0ULL,
     {1.1604407183622192, 1.1486522184045125,
      0.95468808971871344, 0.95468808971871344}},
    // equi b1 unit d3 mivec dsg
    {0x74cb906ec0e5c9f8ULL, 0xbc26b1ca0dccd0efULL, 0x4c825b7152f245fdULL,
     {}},
    // equi b1 shifted d1 complete sg
    {0x545823869876c64cULL, 0x773dc47d53d41eecULL, 0x773dc47d53d41eecULL,
     {0.52523998455448828, 0.52551648548917407,
      0.27046931803228641, 0.27046931803228641}},
    // equi b1 shifted d1 complete dsg
    {0xd4deaf15261860adULL, 0xc3f2632a4a36df16ULL, 0xd72899797aa6c783ULL,
     {}},
    // equi b1 shifted d1 mivec sg
    {0x545823869876c64cULL, 0x773dc47d53d41eecULL, 0x773dc47d53d41eecULL,
     {0.52523998455448828, 0.52551648548917407,
      0.27046931803228641, 0.27046931803228641}},
    // equi b1 shifted d1 mivec dsg
    {0xd4deaf15261860adULL, 0xc3f2632a4a36df16ULL, 0xd72899797aa6c783ULL,
     {}},
    // equi b1 shifted d2 complete sg
    {0x84931d30dfaf3d0dULL, 0x3ef21a58db0a53c5ULL, 0x13b0d7126ea5b6feULL,
     {0.67393436206837953, 0.67346960102245945,
      0.49546931803228622, 0.49546931803228644}},
    // equi b1 shifted d2 complete dsg
    {0x0b68e58875b41fd5ULL, 0xcf9e15de21abf2e3ULL, 0x712ad58ab12fe3e1ULL,
     {}},
    // equi b1 shifted d2 mivec sg
    {0xd93647c444e9ef02ULL, 0xf7eb809221512f76ULL, 0x93a0f39da63ed3a2ULL,
     {0.67388571514220119, 0.66850527773653423,
      0.49546931803228644, 0.49546931803228644}},
    // equi b1 shifted d2 mivec dsg
    {0x9bde285d7244ab13ULL, 0x43a7e53309d72914ULL, 0xa809ac308c80d9a3ULL,
     {}},
    // equi b1 shifted d3 complete sg
    {0x741b0e670bea8ab0ULL, 0xde52cfccf8f7e991ULL, 0xe0840c340c97fb76ULL,
     {0.71522261679515153, 0.71420425185295278,
      0.52592771607034949, 0.52592771607034816}},
    // equi b1 shifted d3 complete dsg
    {0x0e3e25f1292ec31cULL, 0xa78270a2b955f361ULL, 0xd7602a239078f9cdULL,
     {}},
    // equi b1 shifted d3 mivec sg
    {0x2507393b7ff38813ULL, 0x85d0787bba42b871ULL, 0xa5ff7ae321441085ULL,
     {0.71677568816510651, 0.70429981820528287,
      0.52592771607034816, 0.52592771607034816}},
    // equi b1 shifted d3 mivec dsg
    {0xdd1f82e547b84079ULL, 0xb49570a922be026aULL, 0xe22f342fe64b33a5ULL,
     {}},
    // equi b2 unit d1 complete sg
    {0xafd0444e2c4f11ffULL, 0x2a5aa45e23bd97aaULL, 0x8eaa03361308f51cULL,
     {1.074734838907456, 1.0742158842947549,
      0.91393118527122819, 0.91393118527122819}},
    // equi b2 unit d1 complete dsg
    {0x0b9251bcc5d86ea3ULL, 0xf54d5aa90cacb56bULL, 0x964f9748afbd3fb3ULL,
     {}},
    // equi b2 unit d1 mivec sg
    {0xafd0444e2c4f11ffULL, 0x2a5aa45e23bd97aaULL, 0x8eaa03361308f51cULL,
     {1.074734838907456, 1.0742158842947549,
      0.91393118527122819, 0.91393118527122819}},
    // equi b2 unit d1 mivec dsg
    {0x0b9251bcc5d86ea3ULL, 0xf54d5aa90cacb56bULL, 0x964f9748afbd3fb3ULL,
     {}},
    // equi b2 unit d2 complete sg
    {0x947a8424e5ea732cULL, 0xd7d8ec4c7478b490ULL, 0x6ddca7089530dc13ULL,
     {1.1500875436460565, 1.1487735336360487,
      1.0298374180359597, 1.0298374180359597}},
    // equi b2 unit d2 complete dsg
    {0x76d444c79d60066eULL, 0x0afeb7b61f6ede9aULL, 0xed9b33bcc2d88dbfULL,
     {}},
    // equi b2 unit d2 mivec sg
    {0x8c46b6c6f8b5ee95ULL, 0xaee398a9fc85f530ULL, 0x1ac89c3b12f6ecdfULL,
     {1.1501732809490894, 1.1453391189133109,
      1.0298374180359597, 1.0298374180359597}},
    // equi b2 unit d2 mivec dsg
    {0x1d36c2ce63915858ULL, 0x9a9822591b42e189ULL, 0x4d51faa8d6cfcb92ULL,
     {}},
    // equi b2 unit d3 complete sg
    {0xa5eaa9c743aa2ab0ULL, 0xd391d0f7748d3a03ULL, 0x72b70e778f8ad98aULL,
     {1.1604820806319069, 1.1586151205975486,
      0.95411491695811268, 0.95468808971871366}},
    // equi b2 unit d3 complete dsg
    {0x25c1a9e9855e527cULL, 0x1f1ae8ed6d7e5cebULL, 0x347da5342bc3606dULL,
     {}},
    // equi b2 unit d3 mivec sg
    {0x899d91ed3b763edbULL, 0xdf5da99b62668366ULL, 0x5c14ed03e001824fULL,
     {1.1603564049561126, 1.1486522184045125,
      0.96726665784358423, 0.95468808971871344}},
    // equi b2 unit d3 mivec dsg
    {0x88f744fea5a8cb7bULL, 0xbc6a42ed15f92137ULL, 0x0fee17df712e90d3ULL,
     {}},
    // equi b2 shifted d1 complete sg
    {0x545823869876c64cULL, 0x773dc47d53d41eecULL, 0x773dc47d53d41eecULL,
     {0.52523998455448828, 0.52551648548917407,
      0.27046931803228641, 0.27046931803228641}},
    // equi b2 shifted d1 complete dsg
    {0xd4deaf15261860adULL, 0xe5c8093f9df8ea65ULL, 0x0f3a9343bf243732ULL,
     {}},
    // equi b2 shifted d1 mivec sg
    {0x545823869876c64cULL, 0x773dc47d53d41eecULL, 0x773dc47d53d41eecULL,
     {0.52523998455448828, 0.52551648548917407,
      0.27046931803228641, 0.27046931803228641}},
    // equi b2 shifted d1 mivec dsg
    {0xd4deaf15261860adULL, 0xe5c8093f9df8ea65ULL, 0x0f3a9343bf243732ULL,
     {}},
    // equi b2 shifted d2 complete sg
    {0x8fd88620c7ec0a0dULL, 0x80b199f9792f964dULL, 0x5df7013c0c7c3800ULL,
     {0.67396483704721821, 0.67346960102245945,
      0.49546931803228644, 0.49546931803228644}},
    // equi b2 shifted d2 complete dsg
    {0x6dd7a7ef427291aaULL, 0x3a0a2a02b00f3107ULL, 0x126be1aa64b62a3aULL,
     {}},
    // equi b2 shifted d2 mivec sg
    {0x830ec914a1c60b22ULL, 0xe83104245b5f512aULL, 0x6cd53c7ea8ecd687ULL,
     {0.67367141542595121, 0.66850527773653423,
      0.49546931803228644, 0.49546931803228644}},
    // equi b2 shifted d2 mivec dsg
    {0x167255c362dd2f82ULL, 0xfb317d90773479e1ULL, 0x7ece3a2a1e9e632fULL,
     {}},
    // equi b2 shifted d3 complete sg
    {0x4e3ca93b5ab353efULL, 0xcbf3ff7e7d582cefULL, 0x9922f866afee9cecULL,
     {0.71512226243992716, 0.71420425185295278,
      0.48520365869804616, 0.52592771607034816}},
    // equi b2 shifted d3 complete dsg
    {0xec9ef51dbfae2729ULL, 0xa2c2bb07285f7a62ULL, 0x4c432a8465095448ULL,
     {}},
    // equi b2 shifted d3 mivec sg
    {0x8986e0c08142a90bULL, 0x24d3e1df84fb7229ULL, 0x24d3e1df84fb7229ULL,
     {0.7294063396683087, 0.70429981820528287,
      0.82243862295254799, 0.52592771607034816}},
    // equi b2 shifted d3 mivec dsg
    {0xaa84ded0f70ca70bULL, 0x70feedeb944b991cULL, 0x181c8f6c6693ce63ULL,
     {}},
    // cc b0 unit d1 complete sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // cc b0 unit d1 complete dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // cc b0 unit d1 mivec sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // cc b0 unit d1 mivec dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // cc b0 unit d2 complete sg
    {0x9d010fbe67489a4dULL, 0xe06b093bb1809a7fULL, 0x783464774aa4bd50ULL,
     {1.1500903199498445, 1.1470180434016573,
      1.0298374180359597, 1.0298374180359597}},
    // cc b0 unit d2 complete dsg
    {0x2eec48ffc577e5ffULL, 0xb0ba003717f94a45ULL, 0x772cbfc7736c61c4ULL,
     {}},
    // cc b0 unit d2 mivec sg
    {0x9f3c6a1927ee9c79ULL, 0x7a9c96c128a24c7fULL, 0xe404bdd127d70c29ULL,
     {1.150091322503229, 1.1418167302960542,
      1.0298374180359597, 1.0298374180359597}},
    // cc b0 unit d2 mivec dsg
    {0xa897683a0ed18479ULL, 0x46305410f5a825fdULL, 0x96f8890d68b25d89ULL,
     {}},
    // cc b0 unit d3 complete sg
    {0x201f33f14bfc21b5ULL, 0x5af32a34f5d4801dULL, 0xb97696746a1951a3ULL,
     {1.1604884614682927, 1.1558698687084865,
      0.95468808971871366, 0.95468808971871366}},
    // cc b0 unit d3 complete dsg
    {0x206fdd1e0327d863ULL, 0xa98f2b1ac98e235bULL, 0x47d531c63d4c02e5ULL,
     {}},
    // cc b0 unit d3 mivec sg
    {0x548b0ef43bd6343aULL, 0xb1bac389b509503bULL, 0x3545dec4f13bb201ULL,
     {1.1603669983519631, 1.1391269180106591,
      0.95468808971871344, 0.95468808971871344}},
    // cc b0 unit d3 mivec dsg
    {0xabca040d5a02fb35ULL, 0xe08289cbd3c0626fULL, 0xe0c04d2b0066825aULL,
     {}},
    // cc b0 shifted d1 complete sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // cc b0 shifted d1 complete dsg
    {0x589f6cc425ff560aULL, 0xed2ff39eb250f77fULL, 0x2ac4483f14c68fe9ULL,
     {}},
    // cc b0 shifted d1 mivec sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // cc b0 shifted d1 mivec dsg
    {0x589f6cc425ff560aULL, 0xed2ff39eb250f77fULL, 0x2ac4483f14c68fe9ULL,
     {}},
    // cc b0 shifted d2 complete sg
    {0xa3ec7b8468064281ULL, 0xdcb02e5ac0a2bedeULL, 0xd1edd34a53f3bd87ULL,
     {0.67393388238651486, 0.67065602980665839,
      0.49546931803228644, 0.49546931803228644}},
    // cc b0 shifted d2 complete dsg
    {0x5f1d8d943782b771ULL, 0xd07072331749ffd9ULL, 0x48d2b6646eb2c60cULL,
     {}},
    // cc b0 shifted d2 mivec sg
    {0x63e1fdd9693d45ddULL, 0x991c55f523de19f1ULL, 0x861595faadef4d0bULL,
     {0.67391814141716921, 0.66374218920400074,
      0.49546931803228644, 0.49546931803228644}},
    // cc b0 shifted d2 mivec dsg
    {0xd2081aaa4184a37cULL, 0x8df544615fa927e9ULL, 0x35b9bc389743a900ULL,
     {}},
    // cc b0 shifted d3 complete sg
    {0xbed033c1203ac5a5ULL, 0x612744e75ff198d4ULL, 0x6f35ce0fe492bb36ULL,
     {0.71522836154106928, 0.71075500963000537,
      0.52592771607034816, 0.52592771607034816}},
    // cc b0 shifted d3 complete dsg
    {0x1ddc13baf4268d31ULL, 0xe47769d2281bcf74ULL, 0x134848da790ac350ULL,
     {}},
    // cc b0 shifted d3 mivec sg
    {0x5dee7e735b2c912eULL, 0xe3a3fae26d5d1feaULL, 0x44a3b5de8602ab81ULL,
     {0.71503863916511101, 0.6954296517702856,
      0.52592771607034816, 0.52592771607034816}},
    // cc b0 shifted d3 mivec dsg
    {0x6cc88d3b6fcad60aULL, 0x01b13d15bca873ceULL, 0xb13dd92bcbbd0f60ULL,
     {}},
    // cc b1 unit d1 complete sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // cc b1 unit d1 complete dsg
    {0xb91179c84c66dea2ULL, 0x3efa1daa4b5fc79fULL, 0x54f07dceeb360dfbULL,
     {}},
    // cc b1 unit d1 mivec sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // cc b1 unit d1 mivec dsg
    {0xb91179c84c66dea2ULL, 0x3efa1daa4b5fc79fULL, 0x54f07dceeb360dfbULL,
     {}},
    // cc b1 unit d2 complete sg
    {0x3d3fd10ad83042ceULL, 0x83196fd29b41c718ULL, 0xccca82630c69bc89ULL,
     {1.1500903314663915, 1.1470180434016573,
      1.0298374180359597, 1.0298374180359597}},
    // cc b1 unit d2 complete dsg
    {0x488b7e9d15db965fULL, 0xce464d34cda20929ULL, 0xdda49a9143fe2e86ULL,
     {}},
    // cc b1 unit d2 mivec sg
    {0x4af6615774763b0aULL, 0x7e6e6beeeabfaf3fULL, 0xbb377f6154bdb098ULL,
     {1.1501101580658555, 1.1418167302960542,
      1.0298374180359597, 1.0298374180359597}},
    // cc b1 unit d2 mivec dsg
    {0x769f853f8cb5b128ULL, 0x5343b634e140606eULL, 0x4ce9b4cc5e277191ULL,
     {}},
    // cc b1 unit d3 complete sg
    {0xfd9d76637d8d412eULL, 0x10397a803ca657ffULL, 0x0f1db468bdc4a8cfULL,
     {1.1604870782126682, 1.1558698687084865,
      0.954688089718712, 0.95468808971871366}},
    // cc b1 unit d3 complete dsg
    {0x5db31bef03ecefd4ULL, 0x818e3a4bf26d0d00ULL, 0x7c23e5d744d52149ULL,
     {}},
    // cc b1 unit d3 mivec sg
    {0x1554f05f813f684aULL, 0x93e614a5bca880adULL, 0x93e614a5bca880adULL,
     {1.1603526781457885, 1.1391269180106591,
      0.95468808971871344, 0.95468808971871344}},
    // cc b1 unit d3 mivec dsg
    {0x078e38fabe601cc6ULL, 0x40933b72b4c2dda6ULL, 0xfac2470203a09eb2ULL,
     {}},
    // cc b1 shifted d1 complete sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // cc b1 shifted d1 complete dsg
    {0x589f6cc425ff560aULL, 0x4d8e085e27b9f4e4ULL, 0xb482ed03592d7341ULL,
     {}},
    // cc b1 shifted d1 mivec sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // cc b1 shifted d1 mivec dsg
    {0x589f6cc425ff560aULL, 0x4d8e085e27b9f4e4ULL, 0xb482ed03592d7341ULL,
     {}},
    // cc b1 shifted d2 complete sg
    {0x84944ec3c5ebc935ULL, 0xe35d07699021174eULL, 0x90c62f69868df2dbULL,
     {0.67393369415312476, 0.67065602980665839,
      0.49546931803228622, 0.49546931803228644}},
    // cc b1 shifted d2 complete dsg
    {0x3f52096b52ea05c8ULL, 0x48564aefc0a8668aULL, 0x0225abd19a110710ULL,
     {}},
    // cc b1 shifted d2 mivec sg
    {0xe65d0f6a45625107ULL, 0x17ec4339afe39663ULL, 0xe112ddf5761d2e75ULL,
     {0.67383964452083167, 0.66374218920400074,
      0.49546931803228644, 0.49546931803228644}},
    // cc b1 shifted d2 mivec dsg
    {0xb7317ae7773aaa8bULL, 0x840b297193bca1c2ULL, 0x0a3ed766c54325c4ULL,
     {}},
    // cc b1 shifted d3 complete sg
    {0x0b550aa95fc7e0d9ULL, 0xf7d407fcc62ac658ULL, 0x0c5404cfef6f5741ULL,
     {0.71526058587368524, 0.71075500963000537,
      0.52592771607034949, 0.52592771607034816}},
    // cc b1 shifted d3 complete dsg
    {0xb6f79f93656eb93fULL, 0x1342f04104ec8ed3ULL, 0x05a601367edf11a8ULL,
     {}},
    // cc b1 shifted d3 mivec sg
    {0x921b9eabc0f7b45aULL, 0x416910009f63f02bULL, 0xf69e1b7e07ad1698ULL,
     {0.71366905948285908, 0.6954296517702856,
      0.52592771607034816, 0.52592771607034816}},
    // cc b1 shifted d3 mivec dsg
    {0xdcd3a96b59722185ULL, 0x284cefc4733b190dULL, 0xe0ea58177791761fULL,
     {}},
    // cc b2 unit d1 complete sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // cc b2 unit d1 complete dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // cc b2 unit d1 mivec sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // cc b2 unit d1 mivec dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // cc b2 unit d2 complete sg
    {0x760412d7815a2f61ULL, 0x5093bd5915c5a83bULL, 0x5229b4a95ed9fec3ULL,
     {1.1500898051200037, 1.1470180434016573,
      1.0298374180359597, 1.0298374180359597}},
    // cc b2 unit d2 complete dsg
    {0xd75186c0808726ceULL, 0xc93a99ef221cc755ULL, 0xa99464ad6ef54e98ULL,
     {}},
    // cc b2 unit d2 mivec sg
    {0x93317ada7476f764ULL, 0xcdf625e067e9b700ULL, 0xe3c0d2e75ea07eabULL,
     {1.1502545625484306, 1.1418167302960542,
      1.0298374180359597, 1.0298374180359597}},
    // cc b2 unit d2 mivec dsg
    {0x8a752f10bc7763c9ULL, 0x0686ac53bfbb9b51ULL, 0xa29b06ca85daac16ULL,
     {}},
    // cc b2 unit d3 complete sg
    {0x60d28cb7d799916aULL, 0x4355910a08ecc161ULL, 0x61eca01a229c4f28ULL,
     {1.1604787353407866, 1.1558698687084865,
      0.95444336081090331, 0.95468808971871366}},
    // cc b2 unit d3 complete dsg
    {0x86d80cfcca5e114eULL, 0xabb4a8b323e72771ULL, 0xe99c6a895c32d853ULL,
     {}},
    // cc b2 unit d3 mivec sg
    {0x124f83a77d07a334ULL, 0xffa437ef1a9576e8ULL, 0xffa437ef1a9576e8ULL,
     {1.1592475905941071, 1.1391269180106591,
      0.96277751266224232, 0.95468808971871344}},
    // cc b2 unit d3 mivec dsg
    {0xfd372b805f5f2d08ULL, 0xe8735a101f35ce38ULL, 0xaff5845aebb68922ULL,
     {}},
    // cc b2 shifted d1 complete sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // cc b2 shifted d1 complete dsg
    {0x589f6cc425ff560aULL, 0x959730e73868a2cfULL, 0x144c6acd38b81f49ULL,
     {}},
    // cc b2 shifted d1 mivec sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // cc b2 shifted d1 mivec dsg
    {0x589f6cc425ff560aULL, 0x959730e73868a2cfULL, 0x144c6acd38b81f49ULL,
     {}},
    // cc b2 shifted d2 complete sg
    {0xc1ecde5c66e32176ULL, 0xb8a639b74422f82bULL, 0xda12fb21326d43afULL,
     {0.67393912927963362, 0.67065602980665839,
      0.49546931803228633, 0.49546931803228644}},
    // cc b2 shifted d2 complete dsg
    {0x5741a401f09781e2ULL, 0x0b842611bbfeddf3ULL, 0x0dcc32e5c5360e3eULL,
     {}},
    // cc b2 shifted d2 mivec sg
    {0x6dce2aac5775a4e7ULL, 0x390a3c988cb4ab82ULL, 0xd409432aaa9d88cbULL,
     {0.67342433843046134, 0.66374218920400074,
      0.49546931803228644, 0.49546931803228644}},
    // cc b2 shifted d2 mivec dsg
    {0x8a4734825bba3825ULL, 0x350ba65287602505ULL, 0xa09a45381550ba10ULL,
     {}},
    // cc b2 shifted d3 complete sg
    {0xefe37f732e094f60ULL, 0x521e7e61a7335160ULL, 0xdd1dee8c5c060b4eULL,
     {0.71548083999029233, 0.71075500963000537,
      0.51000964823268347, 0.52592771607034816}},
    // cc b2 shifted d3 complete dsg
    {0xd3b6bdff9287a248ULL, 0x36ae0db47fca409bULL, 0xfe97c01e05164926ULL,
     {}},
    // cc b2 shifted d3 mivec sg
    {0xd798996a3fc28f32ULL, 0x0cedeee1b73e8859ULL, 0x301fc116ccf54bfaULL,
     {0.70678666515411692, 0.6954296517702856,
      0.70176768667511746, 0.52592771607034816}},
    // cc b2 shifted d3 mivec dsg
    {0x3a33f9a47400a863ULL, 0xc60c1846205a720cULL, 0xbba7f0a2eecac1ceULL,
     {}},
    // cheb1 b0 unit d1 complete sg
    {0x5aaa6d3d9eb1d110ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.0747348389074536, 1.0742175886571268,
      0.91393118527122819, 0.91393118527122819}},
    // cheb1 b0 unit d1 mivec sg
    {0x5aaa6d3d9eb1d110ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.0747348389074536, 1.0742175886571268,
      0.91393118527122819, 0.91393118527122819}},
    // cheb1 b0 unit d2 complete sg
    {0xa6c0b4f1175d36d5ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1500903477764446, 1.146236210289439,
      1.0298374180359597, 1.0298374180359597}},
    // cheb1 b0 unit d2 mivec sg
    {0xc65315c621223401ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1500917795787542, 1.1402055005643574,
      1.0298374180359597, 1.0298374180359597}},
    // cheb1 b0 unit d3 complete sg
    {0xd5f6f8a81c46137aULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1604884785766951, 1.1535882613158093,
      0.95468808971871366, 0.95468808971871366}},
    // cheb1 b0 unit d3 mivec sg
    {0x4ceac09d50a42b18ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1603074816744698, 1.1352124699720609,
      0.95468808971871344, 0.95468808971871344}},
    // cheb1 b0 shifted d1 complete sg
    {0xbc3b84c1bb50716fULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.52523998455451837, 0.52547936900151548,
      0.27046931803228641, 0.27046931803228641}},
    // cheb1 b0 shifted d1 mivec sg
    {0xbc3b84c1bb50716fULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.52523998455451837, 0.52547936900151548,
      0.27046931803228641, 0.27046931803228641}},
    // cheb1 b0 shifted d2 complete sg
    {0x03c5aa830249f77fULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.67393250841856223, 0.66994983717177359,
      0.49546931803228644, 0.49546931803228644}},
    // cheb1 b0 shifted d2 mivec sg
    {0xa857b2ef614e1bb9ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.67391113045682005, 0.66192318232801894,
      0.49546931803228644, 0.49546931803228644}},
    // cheb1 b0 shifted d3 complete sg
    {0xf8151f561580bb9dULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.7151947159420714, 0.70905027081818017,
      0.52592771607034816, 0.52592771607034816}},
    // cheb1 b0 shifted d3 mivec sg
    {0xb69814b6befe0c35ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.71454030960500947, 0.69381739825264055,
      0.52592771607034816, 0.52592771607034816}},
    // cheb1 b1 unit d1 complete sg
    {0x5aaa6d3d9eb1d110ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.0747348389074536, 1.0742175886571268,
      0.91393118527122819, 0.91393118527122819}},
    // cheb1 b1 unit d1 mivec sg
    {0x5aaa6d3d9eb1d110ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.0747348389074536, 1.0742175886571268,
      0.91393118527122819, 0.91393118527122819}},
    // cheb1 b1 unit d2 complete sg
    {0x3bd9f9106d3f808cULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1500903680794454, 1.146236210289439,
      1.0298374180359597, 1.0298374180359597}},
    // cheb1 b1 unit d2 mivec sg
    {0x19a588f95fdadc29ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1501196437797414, 1.1402055005643574,
      1.0298374180359597, 1.0298374180359597}},
    // cheb1 b1 unit d3 complete sg
    {0xd7a13ba50efed259ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.160486294369325, 1.1535882613158093,
      0.954688089718712, 0.95468808971871366}},
    // cheb1 b1 unit d3 mivec sg
    {0x4db0ff72fcb97224ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1602679816402275, 1.1352124699720609,
      0.95468808971871344, 0.95468808971871344}},
    // cheb1 b1 shifted d1 complete sg
    {0xbc3b84c1bb50716fULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.52523998455451837, 0.52547936900151548,
      0.27046931803228641, 0.27046931803228641}},
    // cheb1 b1 shifted d1 mivec sg
    {0xbc3b84c1bb50716fULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.52523998455451837, 0.52547936900151548,
      0.27046931803228641, 0.27046931803228641}},
    // cheb1 b1 shifted d2 complete sg
    {0x6526d14f56d59886ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.67393222844756806, 0.66994983717177359,
      0.49546931803228622, 0.49546931803228644}},
    // cheb1 b1 shifted d2 mivec sg
    {0xc22e39b82f4d9822ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.67379713212280923, 0.66192318232801894,
      0.49546931803228644, 0.49546931803228644}},
    // cheb1 b1 shifted d3 complete sg
    {0x3b8e046918234581ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.7152539061965647, 0.70905027081818017,
      0.52592771607034949, 0.52592771607034816}},
    // cheb1 b1 shifted d3 mivec sg
    {0x2d127e131cacc449ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.71089473062880193, 0.69381739825264055,
      0.52592771607034816, 0.52592771607034816}},
    // cheb1 b2 unit d1 complete sg
    {0x5aaa6d3d9eb1d110ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.0747348389074536, 1.0742175886571268,
      0.91393118527122819, 0.91393118527122819}},
    // cheb1 b2 unit d1 mivec sg
    {0x5aaa6d3d9eb1d110ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.0747348389074536, 1.0742175886571268,
      0.91393118527122819, 0.91393118527122819}},
    // cheb1 b2 unit d2 complete sg
    {0xbe0d431f015c23deULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.150096599972972, 1.146236210289439,
      1.0298374180359597, 1.0298374180359597}},
    // cheb1 b2 unit d2 mivec sg
    {0xbb5faabbf001c07eULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1503307062615711, 1.1402055005643574,
      1.0298374180359597, 1.0298374180359597}},
    // cheb1 b2 unit d3 complete sg
    {0xff9a127555f1f868ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.1604652381998313, 1.1535882613158093,
      0.95462929243239292, 0.95468808971871366}},
    // cheb1 b2 unit d3 mivec sg
    {0xd86599bc78c9c559ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {1.158209516359386, 1.1352124699720609,
      0.95859202578623426, 0.95468808971871344}},
    // cheb1 b2 shifted d1 complete sg
    {0xbc3b84c1bb50716fULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.52523998455451837, 0.52547936900151548,
      0.27046931803228641, 0.27046931803228641}},
    // cheb1 b2 shifted d1 mivec sg
    {0xbc3b84c1bb50716fULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.52523998455451837, 0.52547936900151548,
      0.27046931803228641, 0.27046931803228641}},
    // cheb1 b2 shifted d2 complete sg
    {0xb1b7c3f0dd41f575ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.67387172192229816, 0.66994983717177359,
      0.49546931803228644, 0.49546931803228644}},
    // cheb1 b2 shifted d2 mivec sg
    {0x15ecbf139ce98dfdULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.67320163754076778, 0.66192318232801894,
      0.49546931803228644, 0.49546931803228644}},
    // cheb1 b2 shifted d3 complete sg
    {0x96710a6743bbea71ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.71378637781856602, 0.70905027081818017,
      0.5224149676900689, 0.52592771607034816}},
    // cheb1 b2 shifted d3 mivec sg
    {0x0418da954fbf83c9ULL, 0x13198a2e03707344ULL, 0x13198a2e03707344ULL,
     {0.68880563982612009, 0.69381739825264055,
      0.60456768033522668, 0.52592771607034816}},
    // mixed b0 unit d1 complete sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // mixed b0 unit d1 complete dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // mixed b0 unit d1 mivec sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // mixed b0 unit d1 mivec dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // mixed b0 unit d2 complete sg
    {0xcd36a50d7dfb0553ULL, 0xbd76ae8a2f28747eULL, 0x300e02b14c4b5dcdULL,
     {1.150090320421703, 1.1489753355339904,
      1.0298374180359597, 1.0298374180359597}},
    // mixed b0 unit d2 complete dsg
    {0xfff322bf55265554ULL, 0x422d7aad507e0055ULL, 0x8745635e6e40bf97ULL,
     {}},
    // mixed b0 unit d2 mivec sg
    {0x04b860bd31412163ULL, 0x3d2d32e582e92ef7ULL, 0x616665efff4320d4ULL,
     {1.1500908511307606, 1.1454669248287423,
      1.0298374180359597, 1.0298374180359597}},
    // mixed b0 unit d2 mivec dsg
    {0x537f796ceb1cba93ULL, 0xba71815cc4398fc8ULL, 0x1df3e028d3681898ULL,
     {}},
    // mixed b0 unit d3 complete sg
    {0x347b38fba0a7474dULL, 0x6cb63cf3a7d592dbULL, 0x5bf2daaa7a83803dULL,
     {1.1604884575510701, 1.157789049079307,
      0.95468808971871366, 0.95468808971871366}},
    // mixed b0 unit d3 complete dsg
    {0x3553b8e11604802dULL, 0x273b444242c3b8c2ULL, 0x76814b40c9022cd2ULL,
     {}},
    // mixed b0 unit d3 mivec sg
    {0x167e3e227b4a8074ULL, 0x54ac2006c61beb2dULL, 0x9b7ded4fc54cad55ULL,
     {1.1604274703387298, 1.1440079535505325,
      0.95468808971871344, 0.95468808971871344}},
    // mixed b0 unit d3 mivec dsg
    {0x278f20c92ed8614dULL, 0x9a60f7e826b06093ULL, 0x3ad7b28b508fc06cULL,
     {}},
    // mixed b0 shifted d1 complete sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // mixed b0 shifted d1 complete dsg
    {0x589f6cc425ff560aULL, 0xed2ff39eb250f77fULL, 0x2ac4483f14c68fe9ULL,
     {}},
    // mixed b0 shifted d1 mivec sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // mixed b0 shifted d1 mivec dsg
    {0x589f6cc425ff560aULL, 0xed2ff39eb250f77fULL, 0x2ac4483f14c68fe9ULL,
     {}},
    // mixed b0 shifted d2 complete sg
    {0x66325f1f0fe8f011ULL, 0xb4fbf1621fb75c5cULL, 0x78df0beeceae4e4eULL,
     {0.67393386024894686, 0.67340037493408622,
      0.49546931803228644, 0.49546931803228644}},
    // mixed b0 shifted d2 complete dsg
    {0xf6fd92eb3983c7d6ULL, 0xddb64ca0c5c890cfULL, 0xc20e7963eb53ba67ULL,
     {}},
    // mixed b0 shifted d2 mivec sg
    {0xcda2464a5881649cULL, 0xb599cac930032728ULL, 0xdde5480f68ddb7fcULL,
     {0.67392573071468287, 0.66854757719633096,
      0.49546931803228644, 0.49546931803228644}},
    // mixed b0 shifted d2 mivec dsg
    {0xf784d4a6402b8d54ULL, 0x0092ce82c8c7cfa9ULL, 0xa8a2fc28dda241e0ULL,
     {}},
    // mixed b0 shifted d3 complete sg
    {0x8702f390b38fb2b6ULL, 0x354213665b6c36c0ULL, 0x4fc52f34b257ee93ULL,
     {0.71522817016060736, 0.71306255993787171,
      0.52592771607034816, 0.52592771607034816}},
    // mixed b0 shifted d3 complete dsg
    {0xa932708715b42dcbULL, 0xb6647102791a6cebULL, 0x2321a0612e02c11bULL,
     {}},
    // mixed b0 shifted d3 mivec sg
    {0x0fe3e6b6192a99ecULL, 0xab0d6661a0a5624aULL, 0x424711642f32e94dULL,
     {0.71499663754317111, 0.6996905779964131,
      0.52592771607034816, 0.52592771607034816}},
    // mixed b0 shifted d3 mivec dsg
    {0x0e8385e42f872ae7ULL, 0x59288450eb43ec32ULL, 0x48a3a20a985fbf11ULL,
     {}},
    // mixed b1 unit d1 complete sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // mixed b1 unit d1 complete dsg
    {0xb91179c84c66dea2ULL, 0x3efa1daa4b5fc79fULL, 0x54f07dceeb360dfbULL,
     {}},
    // mixed b1 unit d1 mivec sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // mixed b1 unit d1 mivec dsg
    {0xb91179c84c66dea2ULL, 0x3efa1daa4b5fc79fULL, 0x54f07dceeb360dfbULL,
     {}},
    // mixed b1 unit d2 complete sg
    {0x403d0bdbf4b46bc8ULL, 0x223ef652490bdbd3ULL, 0x52cf75aef16e11a8ULL,
     {1.1500903218809946, 1.1489753355339904,
      1.0298374180359597, 1.0298374180359597}},
    // mixed b1 unit d2 complete dsg
    {0x813f8d9728bebfc9ULL, 0x2d4a967e761d936aULL, 0x5805d7fbdd2142d2ULL,
     {}},
    // mixed b1 unit d2 mivec sg
    {0xd42ab4e12f992762ULL, 0x48a5c9aee5f2a739ULL, 0xedec5f25aeb416b9ULL,
     {1.1501002633369988, 1.1454669248287423,
      1.0298374180359597, 1.0298374180359597}},
    // mixed b1 unit d2 mivec dsg
    {0x7556ba69a47ad95aULL, 0xe424477db6753bc7ULL, 0x6db2c9b3770e2212ULL,
     {}},
    // mixed b1 unit d3 complete sg
    {0xde45d8c69382fd65ULL, 0xb8ac5426b70e4fedULL, 0x1f494e6d628a1d6fULL,
     {1.1604877016966493, 1.157789049079307,
      0.954688089718712, 0.95468808971871366}},
    // mixed b1 unit d3 complete dsg
    {0x90e87576739eaf69ULL, 0xc749501ba555d73eULL, 0x9abac689f2c7324bULL,
     {}},
    // mixed b1 unit d3 mivec sg
    {0x1b210470a900b723ULL, 0xe32a16e12d3a3420ULL, 0x564fc723085ab47cULL,
     {1.1604131501948538, 1.1440079535505325,
      0.95468808971871344, 0.95468808971871344}},
    // mixed b1 unit d3 mivec dsg
    {0xb96f6912a48ad4a1ULL, 0xe00009452a227742ULL, 0x4fd123c9568ff415ULL,
     {}},
    // mixed b1 shifted d1 complete sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // mixed b1 shifted d1 complete dsg
    {0x589f6cc425ff560aULL, 0x4d8e085e27b9f4e4ULL, 0xb482ed03592d7341ULL,
     {}},
    // mixed b1 shifted d1 mivec sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // mixed b1 shifted d1 mivec dsg
    {0x589f6cc425ff560aULL, 0x4d8e085e27b9f4e4ULL, 0xb482ed03592d7341ULL,
     {}},
    // mixed b1 shifted d2 complete sg
    {0x9d45b20cde766793ULL, 0x1c8a7bbe4ae0aeceULL, 0x1e2acfde4a5670f8ULL,
     {0.67393382769912435, 0.67340037493408622,
      0.49546931803228622, 0.49546931803228644}},
    // mixed b1 shifted d2 complete dsg
    {0x8cf365cd599e322cULL, 0x5dd2af9bd7d42ad9ULL, 0xcfab1785a63a3785ULL,
     {}},
    // mixed b1 shifted d2 mivec sg
    {0x292aca0f04e1147cULL, 0x23144fbdc7317cdaULL, 0xfdf4fb302f6f8631ULL,
     {0.67388572068250896, 0.66854757719633096,
      0.49546931803228644, 0.49546931803228644}},
    // mixed b1 shifted d2 mivec dsg
    {0xcdfa65d5adab6e57ULL, 0x56f8aecde7948655ULL, 0xcb592adfd011efc7ULL,
     {}},
    // mixed b1 shifted d3 complete sg
    {0x27ca3178ab4934cbULL, 0x854ae21360af6c75ULL, 0xcdd868da4dfe9252ULL,
     {0.71525146367106396, 0.71306255993787171,
      0.52592771607034949, 0.52592771607034816}},
    // mixed b1 shifted d3 complete dsg
    {0x001907e5821214bcULL, 0x7256277d469726b5ULL, 0xcdf6797a11a49d02ULL,
     {}},
    // mixed b1 shifted d3 mivec sg
    {0xa7372ce2f77ba23fULL, 0xd88ea81140a86858ULL, 0x127b8df3e6d26839ULL,
     {0.71362704931017873, 0.6996905779964131,
      0.52592771607034816, 0.52592771607034816}},
    // mixed b1 shifted d3 mivec dsg
    {0x8e55c4816ed90a03ULL, 0xe49a8efed6398033ULL, 0x3b17be3b050f12cdULL,
     {}},
    // mixed b2 unit d1 complete sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // mixed b2 unit d1 complete dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // mixed b2 unit d1 mivec sg
    {0x0164a09b7764b0b6ULL, 0x7cdf6aa3f7bd9b5bULL, 0x7cdf6aa3f7bd9b5bULL,
     {1.0747348389074531, 1.0743405895206195,
      0.91393118527122819, 0.91393118527122819}},
    // mixed b2 unit d1 mivec dsg
    {0xb91179c84c66dea2ULL, 0x55323b10f3448d0bULL, 0x54f07dceeb360dfbULL,
     {}},
    // mixed b2 unit d2 complete sg
    {0x6bc328c3d06a1e30ULL, 0x046e0ba0a93d56e9ULL, 0xb2b75475c4ddeeceULL,
     {1.1500900561039944, 1.1489753355339904,
      1.0298374180359597, 1.0298374180359597}},
    // mixed b2 unit d2 complete dsg
    {0x1d88ebce878b111cULL, 0x4ee63ffa73cccabdULL, 0xff32247a0d4cd4c2ULL,
     {}},
    // mixed b2 unit d2 mivec sg
    {0xb12b9c13820e7002ULL, 0x8f95726f5ec8034aULL, 0xf3a3225525c7686fULL,
     {1.1501732809443705, 1.1454669248287423,
      1.0298374180359597, 1.0298374180359597}},
    // mixed b2 unit d2 mivec dsg
    {0x697c541e70e5f286ULL, 0x7e3d2336e342e9f1ULL, 0x657745c1473fb3c5ULL,
     {}},
    // mixed b2 unit d3 complete sg
    {0x551a9919ed90f1b9ULL, 0x362b6e2bef33873dULL, 0x36639e53d37a2ea3ULL,
     {1.1604830185611257, 1.157789049079307,
      0.95444336081090331, 0.95468808971871366}},
    // mixed b2 unit d3 complete dsg
    {0xa2ae7c09b2977b41ULL, 0xb95e210dfe914006ULL, 0xb1fc278cf0f8be70ULL,
     {}},
    // mixed b2 unit d3 mivec sg
    {0x81c7242a4b087887ULL, 0x9df9690425899b88ULL, 0x0fdf4538911930aaULL,
     {1.1597417873945934, 1.1440079535505325,
      0.96277751266224232, 0.95468808971871344}},
    // mixed b2 unit d3 mivec dsg
    {0xf576814aeab01b13ULL, 0x4b3c272795178c5bULL, 0xead08e58c5365841ULL,
     {}},
    // mixed b2 shifted d1 complete sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // mixed b2 shifted d1 complete dsg
    {0x589f6cc425ff560aULL, 0x959730e73868a2cfULL, 0x144c6acd38b81f49ULL,
     {}},
    // mixed b2 shifted d1 mivec sg
    {0xd999d1b89d86d3c3ULL, 0x600825c7473adec4ULL, 0x600825c7473adec4ULL,
     {0.52523998455618148, 0.52555586674826027,
      0.27046931803228641, 0.27046931803228641}},
    // mixed b2 shifted d1 mivec dsg
    {0x589f6cc425ff560aULL, 0x959730e73868a2cfULL, 0x144c6acd38b81f49ULL,
     {}},
    // mixed b2 shifted d2 complete sg
    {0x87d21d6497944783ULL, 0xa399839f7ce7613eULL, 0x511464cd5e0ece09ULL,
     {0.6739366264167812, 0.67340037493408622,
      0.49546931803228633, 0.49546931803228644}},
    // mixed b2 shifted d2 complete dsg
    {0xdf0a4265a0bde9afULL, 0x92bc99080cc6e209ULL, 0x98590b59bf68e316ULL,
     {}},
    // mixed b2 shifted d2 mivec sg
    {0x7a596cefd1810829ULL, 0xde20fe1321d6b1b4ULL, 0xc9e7d0e94bfcb8adULL,
     {0.67367142063303331, 0.66854757719633096,
      0.49546931803228644, 0.49546931803228644}},
    // mixed b2 shifted d2 mivec dsg
    {0xe13c162d2e523a2eULL, 0x1a5895dcb7b9d0e5ULL, 0xb90ef543eb47e8faULL,
     {}},
    // mixed b2 shifted d3 complete sg
    {0x743ca6789f440d54ULL, 0x5fa75c85ac51956fULL, 0xc4f45416ab4a16a3ULL,
     {0.71544037633020041, 0.71306255993787171,
      0.51000964823268347, 0.52592771607034816}},
    // mixed b2 shifted d3 complete dsg
    {0x800041d692117e6eULL, 0x091a9e29110b559fULL, 0x65e2ef6eac7e2c57ULL,
     {}},
    // mixed b2 shifted d3 mivec sg
    {0x5246006331942dc7ULL, 0xa8f6652f3e801bfcULL, 0xabc341827dda58d5ULL,
     {0.70655809347438647, 0.6996905779964131,
      0.70176768667511746, 0.52592771607034816}},
    // mixed b2 shifted d3 mivec dsg
    {0x6934b33825c54666ULL, 0xb768d887a2abf35bULL, 0xf1e9a3a426dd20b8ULL,
     {}},
};
// clang-format on

}  // namespace

BOOST_AUTO_TEST_SUITE(grid_storage_pinning)

BOOST_AUTO_TEST_CASE(Operators_Evaluate_And_Compute_As_Before) {
  const char* const print = std::getenv("SGPP_PRINT_GOLDEN");
  if (print != nullptr && std::string(print) == "1") {
    forAllGrids([](const std::string& name, const Golden& g) {
      // One line per configuration: point hash, bitwise hash of this build, interpolation values
      // (GOLDEN keeps the bitwise hashes of the debug and the optimized build)
      std::printf("%s: 0x%016llxULL 0x%016llxULL", name.c_str(),
                  static_cast<unsigned long long>(g.pointHash),
                  static_cast<unsigned long long>(g.bitwiseHash));
      for (const double value : g.interp) std::printf(" %.17g", value);
      std::printf("\n");
      std::fflush(stdout);
    });
    return;
  }

  size_t idx = 0;
  forAllGrids([&idx](const std::string& name, const Golden& g) {
    BOOST_TEST_CONTEXT(name) {
      BOOST_REQUIRE_LT(idx, GOLDEN.size());
      const Golden& expected = GOLDEN[idx++];
      BOOST_CHECK_EQUAL(g.pointHash, expected.pointHash);
#ifdef NDEBUG
      BOOST_CHECK_EQUAL(g.bitwiseHash, expected.bitwiseHashOpt);
#else
      BOOST_CHECK_EQUAL(g.bitwiseHash, expected.bitwiseHash);
#endif
      BOOST_REQUIRE_EQUAL(g.interp.size(), expected.interp.size());
      for (size_t i = 0; i < g.interp.size(); i++) {
        BOOST_CHECK_SMALL(g.interp[i] - expected.interp[i], 1e-12);
      }
    }
  });
  BOOST_CHECK_EQUAL(idx, GOLDEN.size());
}

BOOST_AUTO_TEST_SUITE_END()
