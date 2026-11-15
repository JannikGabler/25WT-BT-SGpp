// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_weights_test.cpp
 * @brief Boost.Test cases pinning the 1D quadrature weights that the quadrature on a
 * @c SparseGrid applies to its tensor grids, and the Clenshaw-Curtis weights bitwise.
 *
 * The quadrature of a tensor grid multiplies the source function at every grid point by the
 * product of the 1D weights of the quadrature rules of its dimensions. These weights must be
 * bitwise the weights of the rule (@c QuadRule::genWeights) for the node count of the dimension,
 * for every tensor grid and dimension, independently of how and by which thread they are
 * computed.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_weight_table.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// One grid configuration of the weight tests.
struct WeightConfig {
  std::string nodes;  ///< "equi", "cc" or "mixed" (alternating per dimension, starting with cc).
  bool linearGrowth;  ///< Linear instead of doubling growth.
  LvlType boundaryLvlOffset;
  bool shiftedDomain;  ///< Unequal shifted domains per dimension instead of [0, 1]^d.
  size_t nDim;
  bool miVec;  ///< Anisotropic multi-index grid instead of the complete one.
};

std::string toString(const WeightConfig& c) {
  return c.nodes + (c.linearGrowth ? " linear" : " doubling") + " b" +
         std::to_string(c.boundaryLvlOffset) + (c.shiftedDomain ? " shifted" : " unit") + " d" +
         std::to_string(c.nDim) + (c.miVec ? " mivec" : " complete");
}

std::vector<WeightConfig> weightConfigs() {
  std::vector<WeightConfig> configs;
  for (const std::string nodes : {"equi", "cc", "mixed"}) {
    for (const bool linearGrowth : {false, true}) {
      for (const LvlType offset : {0u, 1u, 2u}) {
        for (const bool shifted : {false, true}) {
          for (size_t nDim = 1; nDim <= 3; nDim++) {
            for (const bool miVec : {false, true}) {
              configs.push_back({nodes, linearGrowth, offset, shifted, nDim, miVec});
            }
          }
        }
      }
    }
  }
  return configs;
}

/// Anisotropic multi-indices of the MIVec grids (the closure is computed by the instruction).
LvlMIVec anisotropicMIs(const size_t nDim) {
  std::vector<LvlMI> mis;
  for (size_t dim = 0; dim < nDim; dim++) {
    LvlMI mi(nDim, 1);
    mi[dim] = static_cast<LvlType>(6 - dim);  // Large levels: Clenshaw-Curtis weights by FFT
    mis.push_back(mi);
  }
  return LvlMIVec(mis);
}

/// Generation instruction of @p config (@p miVec must outlive it).
std::unique_ptr<SGGenInstr> makeGenInstr(const WeightConfig& config, const LvlMIVec& miVec) {
  const size_t nDim = config.nDim;
  std::unique_ptr<SGGenInstr> genInstr;
  if (config.miVec) {
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(static_cast<LvlType>(8 - 2 * nDim), nDim);
  }

  HyperCubeArea domain(nDim, {0.0, 1.0});
  for (size_t dim = 0; dim < nDim; dim++) {
    NodeGenFunc* const nodeGenFunc =
        (config.nodes == "equi" || (config.nodes == "mixed" && dim % 2 == 1))
            ? getEquidistantNodeGenFunc()
            : getClenshawCurtisNodeGenFunc();
    genInstr->setNodeGenFuncForDim(nodeGenFunc, dim);
    if (config.shiftedDomain) {
      domain[dim] = {-1.5 + 0.25 * static_cast<double>(dim), 0.5 + 0.75 * static_cast<double>(dim)};
    }
  }
  genInstr->setDomain(domain);
  genInstr->setLvl2GPCntFunc(config.linearGrowth ? linearLvl2GPCntFunction
                                                 : doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(config.boundaryLvlOffset);
  return genInstr;
}

/// Whether @p a and @p b are bitwise equal.
bool bitwiseEqual(const std::span<const double> a, const std::span<const double> b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); i++) {
    if (std::bit_cast<uint64_t>(a[i]) != std::bit_cast<uint64_t>(b[i])) return false;
  }
  return true;
}

/**
 * Checks for every tensor grid of @p sg and every dimension that the quadrature applies the
 * weights of the quadrature rule of the dimension (bitwise).
 */
void checkWeightsOfAllTensorGrids(const SparseGrid& sg) {
  const SGGenInstr& genInstr = sg.getGenInstr();
  const misc::QuadWeightTable weightTable(sg);
  const misc::QuadratureContext context{.sparseGrid = sg, .weightTable = weightTable};
  misc::QuadratureScratch scratch(sg.nDim());

  for (const auto& tgData : sg.domainTensorGrids()) {
    const TensorGridView tg = tgData.tensorGrid;
    tools::quadrature::getWeights(context, tg, scratch);

    for (size_t dim = 0; dim < sg.nDim(); dim++) {
      const GPCntType nNodes = tg.nGPInDim(dim);
      const DataVector expected =
          genInstr.getNodeGenFuncForDim(dim)->getQuadRule(nNodes)->genWeights(nNodes);
      const std::span<const double> used(scratch.rows.weightsOfDim[dim], nNodes);

      BOOST_CHECK_MESSAGE(bitwiseEqual(used, expected),
                          "dimension " << dim << ", " << nNodes << " nodes");
    }
  }
}

/// The weights of every (dimension, level) of @p sg in @p table, concatenated (dimension-major).
std::vector<double> allWeights(const SparseGrid& sg, const misc::QuadWeightTable& table) {
  const NodeTable& unitTable = sg.getUnitNodeTable();
  std::vector<double> result;
  for (size_t dim = 0; dim < sg.nDim(); dim++) {
    for (LvlType lvl = 0; lvl <= unitTable.maxLvl(dim); lvl++) {
      const double* const weights = table.weights(dim, lvl);
      result.insert(result.end(), weights, weights + unitTable.nNodes(dim, lvl));
    }
  }
  return result;
}

/// FNV-1a style hash of the bit patterns of @p values.
uint64_t hashBits(const std::span<const double> values, uint64_t h) {
  for (const double x : values) {
    h = (h ^ std::bit_cast<uint64_t>(x)) * 0x100000001b3ULL;
  }
  return h;
}

/// Node counts of the bitwise Clenshaw-Curtis pin (doubling growth with boundary up to 2^15 + 1,
/// odd and even counts).
const std::vector<GPCntType> CC_PIN_NODE_COUNTS = {2,  3,   4,   5,   6,   9,    17,   33,   64,
                                                   65, 100, 129, 257, 513, 1025, 4097, 32769};

/// Hash of the Clenshaw-Curtis weights for CC_PIN_NODE_COUNTS (genWeights and
/// genWeightsInplace; recorded with the debug build and Eigen's FFT, weights by a real FFT of
/// length n - 1).
constexpr uint64_t CC_PIN_HASH_DEBUG = 0x60ec02f99a7622e3ULL;

uint64_t hashCCWeights() {
  const QuadRule* const rule = getClenshawCurtisQuadRule();
  uint64_t h = 0xcbf29ce484222325ULL;
  for (const GPCntType nNodes : CC_PIN_NODE_COUNTS) {
    const DataVector weights = rule->genWeights(nNodes);
    BOOST_REQUIRE_EQUAL(weights.size(), nNodes);
    h = hashBits(weights, h);

    std::vector<double> inplace(nNodes, -1.0);
    rule->genWeightsInplace(inplace);
    BOOST_CHECK_MESSAGE(bitwiseEqual(inplace, weights), nNodes << " nodes");
  }
  return h;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(quadrature_weights)

BOOST_AUTO_TEST_CASE(Tensor_Grids_Use_The_Rule_Weights_Bitwise) {
  // Mixed rules per dimension (cc, equi: Simpson for odd and trapezoidal for even node counts),
  // unequal domains (the weights are those on [0, 1]), boundary level offsets, dimensions sharing
  // their node lists
  for (const WeightConfig& config : weightConfigs()) {
    BOOST_TEST_CONTEXT(toString(config)) {
      const LvlMIVec miVec = anisotropicMIs(config.nDim);
      const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
      const SparseGrid sg(*genInstr);
      checkWeightsOfAllTensorGrids(sg);
    }
  }
}

BOOST_AUTO_TEST_CASE(Weight_Table_Shares_Rows_Of_Dimensions_With_The_Same_Rule) {
  // Unequal domains do not matter (the weights are those on [0, 1]); dimensions with different
  // node-generation functions have rows of their own
  for (const std::string nodes : {"cc", "mixed"}) {
    const WeightConfig config{nodes, false, 1, true, 3, false};
    BOOST_TEST_CONTEXT(toString(config)) {
      const LvlMIVec miVec = anisotropicMIs(config.nDim);
      const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
      const SparseGrid sg(*genInstr);
      const misc::QuadWeightTable table(sg);

      for (LvlType lvl = 0; lvl <= sg.getUnitNodeTable().maxLvl(0); lvl++) {
        const bool shared = sg.getUnitNodeTable().sharesRowsWith(0, 1);
        BOOST_CHECK_EQUAL(shared, nodes == "cc");
        BOOST_CHECK_EQUAL(table.weights(0, lvl) == table.weights(1, lvl), shared);
        BOOST_CHECK(table.weights(0, lvl) == table.weights(2, lvl));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Weight_Table_Is_Thread_Count_Independent) {
  // All threads request all rows at once (in different orders), the first request of a row
  // computes it: the weights equal those of a serial computation bitwise, and every thread sees
  // the same rows
  const WeightConfig config{"mixed", false, 0, true, 2, true};
  const LvlMIVec miVec(std::vector<LvlMI>{LvlMI{12, 1}, LvlMI{1, 11}});
  const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
  const SparseGrid sg(*genInstr);

  const misc::QuadWeightTable serialTable(sg);
  const std::vector<double> expected = allWeights(sg, serialTable);

  for (const int nThreads : {2, 4, 16}) {
    BOOST_TEST_CONTEXT(nThreads << " threads") {
      const misc::QuadWeightTable table(sg);
      std::vector<std::vector<double>> seen(static_cast<size_t>(nThreads));
      const NodeTable& unitTable = sg.getUnitNodeTable();

#pragma omp parallel num_threads(nThreads)
      {
        // Request the rows in a thread-dependent order first, then collect them in the fixed one
        const size_t thread = static_cast<size_t>(omp_get_thread_num());
        for (size_t i = 0; i < 2 * 13; i++) {
          const size_t dim = (i + thread) % 2;
          const LvlType lvl = static_cast<LvlType>((i / 2 + thread) % (unitTable.maxLvl(dim) + 1));
          table.weights(dim, lvl);
        }
        seen[thread] = allWeights(sg, table);
      }

      for (const std::vector<double>& weights : seen) {
        BOOST_CHECK(bitwiseEqual(weights, expected));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Eager_Fill_Equals_Lazy_Fill) {
  // computeAllRows outside a parallel region and from teams of 2 and 16 threads gives bitwise the
  // weights computed on first use; mixed rules, rows shared by dimensions and rows of their own
  for (const std::string nodes : {"cc", "mixed"}) {
    const WeightConfig config{nodes, false, 1, true, 3, true};
    const LvlMIVec miVec(std::vector<LvlMI>{LvlMI{12, 1, 1}, LvlMI{1, 11, 1}, LvlMI{1, 1, 10}});
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid sg(*genInstr);

    const misc::QuadWeightTable lazyTable(sg);
    const std::vector<double> expected = allWeights(sg, lazyTable);

    for (const int nThreads : {0, 2, 16}) {
      BOOST_TEST_CONTEXT(nodes << ", " << nThreads << " threads") {
        const misc::QuadWeightTable eagerTable(sg);
        if (nThreads == 0) {
          eagerTable.computeAllRows();
        } else {
#pragma omp parallel num_threads(nThreads)
          eagerTable.computeAllRows();
        }
        BOOST_CHECK(bitwiseEqual(allWeights(sg, eagerTable), expected));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Clenshaw_Curtis_Weights_Bitwise) {
  // The weights of the debug build (no contraction of multiply-adds, no link-time optimization)
  // with Eigen's FFT are pinned bitwise; the optimized build and the Armadillo backend (other
  // round-off) are checked by genWeightsInplace == genWeights only.
  const uint64_t h = hashCCWeights();
  const char* const print = std::getenv("SGPP_PRINT_GOLDEN");
  if (print != nullptr && std::string(print) == "1") {
    std::printf("Clenshaw-Curtis weights hash: 0x%016llxULL\n", static_cast<unsigned long long>(h));
    return;
  }
#if !defined(NDEBUG) && defined(USE_EIGEN)
  BOOST_CHECK_EQUAL(h, CC_PIN_HASH_DEBUG);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
