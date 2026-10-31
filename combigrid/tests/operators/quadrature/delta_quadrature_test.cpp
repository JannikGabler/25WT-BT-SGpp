// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file delta_quadrature_test.cpp
 * @brief Boost.Test cases for the delta quadrature operator @c quadratureDelta and its internal
 * helpers in @c tools::dsg_quadrature.
 *
 * @c quadratureDelta computes the same combination-technique quadrature as @c quadrature, but
 * evaluates every grid point only once: it iterates over the grid points of the delta tensor grids
 * and weights each of them with the sum of the (coefficient-weighted) quadrature weights it has in
 * the regular tensor grids that contain it. For nested nodes, its result must therefore equal the
 * one of @c quadrature up to round-off, for every source function.
 *
 * The internal helpers are tested in isolation, with delta tensor grids assembled by hand from
 * @c NodeGenFunc::genNodesWithoutBoundary (independently of the deltas sparse grid generation).
 */

#include "sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp"
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <numeric>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();
static NodeGenFunc* const equidistantNodeGenFunc = getEquidistantNodeGenFunc();
static NodeGenFunc* const clenshawCurtisNodeGenFunc = getClenshawCurtisNodeGenFunc();

namespace {

constexpr double EPS = 1e-12;

/// Relative tolerance (w.r.t. max(1, |expected|)) for comparisons of quadrature results.
constexpr double TOL = 1e-11;

/// Computes @c quadratureDelta on the deltas sparse grid described by @p genInstr.
double runQuadratureDelta(const SourceFunc& sourceFunc, const SGGenInstr& genInstr) {
  const DeltasSparseGrid deltasSG(genInstr);
  return quadrature(sourceFunc, deltasSG);
}

/// Reference result: the regular combination-technique quadrature.
double runQuadrature(const SourceFunc& sourceFunc, const SGGenInstr& genInstr) {
  const SparseGrid sg(genInstr);
  return quadrature(sourceFunc, sg);
}

/// Checks |result - expected| <= tol * max(1, |expected|) and prints both values otherwise.
void checkQuadResult(const double result, const double expected, const double tol = TOL) {
  BOOST_CHECK_MESSAGE(std::abs(result - expected) <= tol * std::max(1.0, std::abs(expected)),
                      "quadratureDelta: " << result << ", expected: " << expected
                                          << ", difference: " << result - expected);
}

/**
 * Relative tolerance for comparing two quadratures of the sparse grid of @p genInstr: the
 * combination cancels, so round-off grows with the sum of the absolute coefficients (> 10^4 for
 * d = 12, level 4). TOL or 64 round-off units per unit of that sum, whichever is larger.
 */
double combinationTol(const SGGenInstr& genInstr) {
  double sumOfAbsCoeffs = 0.0;

  for (const CTCoeffType coeff : genInstr.genFullMIVecWithCoeffs().second) {
    sumOfAbsCoeffs += std::abs(static_cast<double>(coeff));
  }

  return std::max(TOL, 64.0 * 0x1p-52 * sumOfAbsCoeffs);
}

std::string nameOf(const NodeGenFunc* nodeGenFunc) {
  if (nodeGenFunc == equidistantNodeGenFunc) return "equidistant";
  if (nodeGenFunc == clenshawCurtisNodeGenFunc) return "Clenshaw-Curtis";
  return "unknown";
}

std::string toString(const std::vector<LvlType>& levels) {
  std::string result = "(";

  for (size_t dim = 0; dim < levels.size(); dim++) {
    result += (dim == 0 ? "" : ", ") + std::to_string(levels[dim]);
  }

  return result + ")";
}

std::string toString(const LvlMI& mi) {
  return toString(std::vector<LvlType>(mi.begin(), mi.end()));
}

/*************
Source functions
*************/

SourceFunc genConstantFunc(const double value) {
  return SourceFunc([value](const DataVector&) { return value; });
}

/// f(x) = prod_k (a_k * x_k + b_k): integrated exactly by every symmetric rule (in [0,1]^d)
SourceFunc genMultilinearFunc(const std::vector<double>& a, const std::vector<double>& b) {
  return SourceFunc([a, b](const DataVector& x) {
    double result = 1.0;
    for (size_t dim = 0; dim < x.size(); dim++) {
      result *= a[dim] * x[dim] + b[dim];
    }
    return result;
  });
}

/// Exact integral of the multilinear function over @p domain.
double integrateMultilinearFunc(const std::vector<double>& a, const std::vector<double>& b,
                                const HyperCubeArea& domain) {
  double result = 1.0;
  for (size_t dim = 0; dim < domain.size(); dim++) {
    const double lower = domain[dim].first;
    const double upper = domain[dim].second;
    result *= (upper - lower) * (a[dim] * (lower + upper) / 2.0 + b[dim]);
  }
  return result;
}

/// f(x) = prod_k (sin(x_k) + 0.3 x_k^2)
SourceFunc genProductFunc() {
  return SourceFunc([](const DataVector& x) {
    double result = 1.0;
    for (const double v : x) {
      result *= std::sin(v) + 0.3 * v * v;
    }
    return result;
  });
}

/// f(x) = exp(-|x - 0.3|^2) * (1 + x_0)
SourceFunc genGaussianFunc() {
  return SourceFunc([](const DataVector& x) {
    double sqDist = 0.0;
    for (const double v : x) {
      sqDist += (v - 0.3) * (v - 0.3);
    }
    return std::exp(-sqDist) * (1.0 + x[0]);
  });
}

/// Random domain with intervals inside [-2, 2] of length >= 0.5.
HyperCubeArea randomDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = randGen.getUniformRN(-2.0, 1.0);
    domain[dim] = {lower, lower + randGen.getUniformRN(0.5, 1.0)};
  }

  return domain;
}

/// Random multi-index with components in [0, @p maxComp].
LvlMI randomMI(const size_t nDim, const LvlType maxComp) {
  LvlMI mi(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(maxComp + 1));
  }

  return mi;
}

/*****************************
Hand-made delta tensor grids
*****************************/

/// Regular 1D nodes of level @p lvl in dimension @p dim (boundary iff lvl >= offset).
DataVector regularNodes(const SGGenInstr& genInstr, const size_t dim, const LvlType lvl) {
  const DataVector innerNodes = genInstr.getNodeGenFuncForDim(dim)->genNodesWithoutBoundary(
      genInstr.getLvl2GPCntFuncForDim(dim)(lvl));

  if (lvl < genInstr.getBoundaryLevelOffset()) {
    return innerNodes;
  }

  DataVector nodes(innerNodes.size() + 2);
  nodes[0] = 0.0;
  std::copy(innerNodes.begin(), innerNodes.end(), nodes.begin() + 1);
  nodes[innerNodes.size() + 1] = 1.0;

  return nodes;
}

/// Index of the node of the ascending @p nodes that equals @p x up to EPS, if any.
std::optional<GPCntType> findNode(const DataVector& nodes, const double x) {
  const auto iter = std::lower_bound(nodes.begin(), nodes.end(), x - EPS);

  if (iter == nodes.end() || *iter > x + EPS) {
    return std::nullopt;
  }

  return static_cast<GPCntType>(iter - nodes.begin());
}

/// Delta nodes of level @p lvl: the regular level-lvl nodes that are no level-(lvl - 1) nodes.
DataVector expectedDeltaNodes(const SGGenInstr& genInstr, const size_t dim, const LvlType lvl) {
  const DataVector nodes = regularNodes(genInstr, dim, lvl);

  if (lvl == 0) {
    return nodes;
  }

  const DataVector coarserNodes = regularNodes(genInstr, dim, lvl - 1);
  DataVector result;

  for (const double node : nodes) {
    if (!findNode(coarserNodes, node)) result.push_back(node);
  }

  return result;
}

/// Delta tensor grid of level @p mi (coefficient 0, it is not used by the tested helpers).
TensorGridCTData expectedDeltaTGData(const LvlMI& mi, const SGGenInstr& genInstr) {
  GPMI gpCntPerDim(mi.size());
  std::vector<double> nodesPerDim;

  for (size_t dim = 0; dim < mi.size(); dim++) {
    const DataVector nodes = expectedDeltaNodes(genInstr, dim, mi[dim]);

    gpCntPerDim[dim] = static_cast<GPCntType>(nodes.size());
    nodesPerDim.insert(nodesPerDim.end(), nodes.begin(), nodes.end());
  }

  return TensorGridCTData{mi, 0, TensorGrid(gpCntPerDim, DataVector(nodesPerDim))};
}

/// Calls @p func for every multi-index l with 0 <= l <= @p upper (component-wise).
void forEachMIInBox(const LvlMI& upper, const std::function<void(const LvlMI&)>& func) {
  LvlMI mi(upper.size(), LvlType{0});

  const std::function<void(size_t)> recurse = [&](const size_t dim) {
    if (dim == upper.size()) {
      func(mi);
      return;
    }

    for (LvlType lvl = 0; lvl <= upper[dim]; lvl++) {
      mi[dim] = lvl;
      recurse(dim + 1);
    }
  };

  recurse(0);
}

/// All multi-indices l with |l|_1 <= @p maxLvl, i.e. the levels of the delta tensor grids.
std::vector<LvlMI> completeSGLevels(const size_t nDim, const LvlType maxLvl) {
  std::vector<LvlMI> result;

  forEachMIInBox(LvlMI(nDim, maxLvl), [&](const LvlMI& mi) {
    if (mi.sumOfElems() <= maxLvl) result.push_back(mi);
  });

  return result;
}

/// Data that quadratureDelta precomputes for @p deltasSG, assembled with the internal helpers,
/// plus the scratch of one thread and the source function (passed to the kernel separately).
struct DeltaQuadratureData {
  DeltaQuadratureData(const DeltasSparseGrid& deltasSG, const SourceFunc& sourceFunc)
      : sourceFunc(sourceFunc),
        weights1D(tools::dsg_quadrature::comp1DWeights(deltasSG)),
        coeffTrie(deltasSG.getTensorGrids(), deltasSG.nDim()),
        context{.deltasSparseGrid = deltasSG,
                .domain = deltasSG.getGenInstr().getDomain(),
                .weights1D = weights1D,
                .coeffTrie = coeffTrie},
        scratch(deltasSG.nDim(), deltasSG.getMaxTGSumOverGPCntsPerDim()) {}

  const SourceFunc& sourceFunc;
  const misc::FlatNestedVec<double, 3> weights1D;
  const misc::CTCoeffTrie coeffTrie;
  const misc::DeltaQuadratureContext context;
  misc::DeltaQuadratureScratch scratch;
};

/// A level multi-index (as plain vector, ordered lexicographically) with its CT coefficient.
using MIWithCoeff = std::pair<std::vector<LvlType>, double>;

/// The multi-indices of the tensor grids of @p deltasSG with a non-zero coefficient that are
/// component-wise >= @p lowerBound, sorted lexicographically.
std::vector<MIWithCoeff> nonZeroCoeffMIs(const DeltasSparseGrid& deltasSG,
                                         const LvlMI& lowerBound) {
  std::vector<MIWithCoeff> result;

  for (const TensorGridCTData& tg : deltasSG) {
    if (tg.coefficient != 0 && tg.mi >= lowerBound) {
      result.emplace_back(std::vector<LvlType>(tg.mi.begin(), tg.mi.end()), tg.coefficient);
    }
  }

  std::sort(result.begin(), result.end());
  return result;
}

/**
 * The leaves of @p trie (multi-indices with coefficients) in the order of a depth-first
 * traversal. Checks on the way that the children of every node are non-empty, sorted by level
 * (strictly ascending), that the child ranges of each depth partition the next depth and that
 * the maxSuffixLvlSum of every node of depth k is the maximum of l_{k+1} + ... + l_{d-1} over the
 * leaves l below it.
 */
std::vector<MIWithCoeff> trieLeaves(const misc::CTCoeffTrie& trie) {
  std::vector<MIWithCoeff> result;
  std::vector<LvlType> prefix;

  const std::function<void(size_t, size_t, size_t)> visit =
      [&](const size_t depth, const size_t begin, const size_t end) {
        const std::span<const misc::CTCoeffTrie::Node> nodes = trie.nodesOfDepth(depth);
        BOOST_REQUIRE_LE(end, nodes.size());

        for (size_t idx = begin; idx < end; idx++) {
          if (idx > begin) BOOST_CHECK_LT(nodes[idx - 1].lvl, nodes[idx].lvl);

          prefix.push_back(nodes[idx].lvl);
          const size_t firstLeaf = result.size();

          if (depth + 1 == trie.nDim()) {
            result.emplace_back(prefix, trie.leafCoeffs()[idx]);
          } else {
            BOOST_CHECK_EQUAL(trie.childBegin(depth, idx), idx == 0 ? 0 : nodes[idx - 1].childEnd);
            BOOST_CHECK_LT(trie.childBegin(depth, idx), trie.childEnd(depth, idx));
            visit(depth + 1, trie.childBegin(depth, idx), trie.childEnd(depth, idx));
          }

          LvlType maxSuffixLvlSum = 0;
          for (size_t leaf = firstLeaf; leaf < result.size(); leaf++) {
            const std::vector<LvlType>& mi = result[leaf].first;
            const auto suffixBegin = mi.begin() + static_cast<std::ptrdiff_t>(depth + 1);
            maxSuffixLvlSum =
                std::max(maxSuffixLvlSum, std::accumulate(suffixBegin, mi.end(), LvlType{0}));
          }
          BOOST_CHECK_EQUAL(nodes[idx].maxSuffixLvlSum, maxSuffixLvlSum);

          prefix.pop_back();
        }
      };

  if (trie.nDim() > 0) {
    visit(0, 0, trie.nodesOfDepth(0).size());

    for (size_t depth = 0; depth + 1 < trie.nDim(); depth++) {
      const std::span<const misc::CTCoeffTrie::Node> nodes = trie.nodesOfDepth(depth);
      BOOST_CHECK_EQUAL(nodes.empty() ? 0 : nodes.back().childEnd,
                        trie.nodesOfDepth(depth + 1).size());
    }
  }

  BOOST_CHECK_EQUAL(trie.nLeaves(), result.size());
  return result;
}

/**
 * The leaves (levels in the projected dimensions, with the projected coefficients) of the
 * projected sub-trie that compSubTrie computed into @c data.scratch for the delta tensor grid of
 * level @p m, in depth-first order. Checks on the way that every inner node has a child (pruned),
 * that every node of projected depth t with level l_k (k = projDims[t]) points to the weights of
 * the delta nodes of level m_k in the rule of level l_k, and that the child ranges of each
 * projected depth partition the next one.
 */
std::vector<MIWithCoeff> subTrieLeaves(const DeltaQuadratureData& data, const LvlMI& m) {
  const std::vector<size_t>& projDims = data.scratch.projDims;
  std::vector<MIWithCoeff> leaves;
  std::vector<LvlType> prefix;

  const std::function<void(size_t, size_t, size_t)> visit =
      [&](const size_t depth, const size_t begin, const size_t end) {
        const size_t dim = projDims[depth];
        const auto& nodes = data.scratch.trieNodes[depth];
        const auto weightsOfDim = data.weights1D[dim];
        const size_t deltaStart = m[dim] == 0 ? 0 : weightsOfDim[m[dim] - 1].size();

        BOOST_REQUIRE_LE(end, nodes.size());

        for (size_t idx = begin; idx < end; idx++) {
          // Level of the node: the one whose weights the node points to
          std::optional<LvlType> lvl;
          for (LvlType l = m[dim]; l < weightsOfDim.size(); l++) {
            if (nodes[idx].weights == weightsOfDim[l].data() + deltaStart) lvl = l;
          }
          BOOST_REQUIRE(lvl.has_value());

          prefix.push_back(*lvl);

          if (depth + 1 == projDims.size()) {
            leaves.emplace_back(prefix, nodes[idx].coeffSum);
          } else {
            const size_t childBegin = idx == 0 ? 0 : nodes[idx - 1].childEnd;
            BOOST_CHECK_LT(childBegin, nodes[idx].childEnd);  // pruned
            visit(depth + 1, childBegin, nodes[idx].childEnd);
          }

          prefix.pop_back();
        }
      };

  visit(0, 0, data.scratch.trieNodes[0].size());

  for (size_t depth = 0; depth + 1 < projDims.size(); depth++) {
    const auto& nodes = data.scratch.trieNodes[depth];
    BOOST_CHECK_EQUAL(nodes.empty() ? 0 : nodes.back().childEnd,
                      data.scratch.trieNodes[depth + 1].size());
  }

  return leaves;
}

/**
 * Brute-force projected sub-trie of the delta tensor grid @p deltaTG of @p deltasSG (see
 * compSubTrie): the projected dimensions P (those in which @p deltaTG has more than one node, or
 * dimension 0 if there are none) and the leaves: the levels l_P of the tensor grids l >= m with a
 * non-zero coefficient, with the projected coefficients sum_l c_l prod_{k not in P} w_k(l_k),
 * where w_k(l_k) is the weight of the single delta node of level m_k in the regular 1D rule of
 * level l_k, sorted lexicographically. The weights are computed from regularNodes and
 * expectedDeltaNodes, independently of comp1DWeights.
 */
std::pair<std::vector<size_t>, std::vector<MIWithCoeff>> expectedProjectedSubTrie(
    const DeltasSparseGrid& deltasSG, const TensorGridCTData& deltaTG) {
  const SGGenInstr& genInstr = deltasSG.getGenInstr();
  const size_t nDim = deltasSG.nDim();
  const LvlMI& m = deltaTG.mi;

  std::vector<size_t> projDims;
  for (size_t dim = 0; dim < nDim; dim++) {
    if (deltaTG.tensorGrid.getGPCntPerDim()[dim] > 1) projDims.push_back(dim);
  }
  if (projDims.empty()) projDims.push_back(0);

  const auto singleNodeWeight = [&](const size_t dim, const LvlType lvl) {
    const DataVector nodes = regularNodes(genInstr, dim, lvl);
    const GPCntType nNodes = static_cast<GPCntType>(nodes.size());
    const std::optional<GPCntType> idx =
        findNode(nodes, expectedDeltaNodes(genInstr, dim, m[dim])[0]);
    BOOST_REQUIRE(idx.has_value());
    return genInstr.getNodeGenFuncForDim(dim)->getQuadRule(nNodes)->genWeights(nNodes)[*idx];
  };

  std::map<std::vector<LvlType>, double> leaves;

  for (const auto& [mi, coeff] : nonZeroCoeffMIs(deltasSG, m)) {
    std::vector<LvlType> key;
    double value = coeff;

    for (size_t dim = 0; dim < nDim; dim++) {
      if (std::ranges::find(projDims, dim) != projDims.end()) {
        key.push_back(mi[dim]);
      } else {
        value *= singleNodeWeight(dim, mi[dim]);
      }
    }

    leaves[key] += value;
  }

  return {projDims, std::vector<MIWithCoeff>(leaves.begin(), leaves.end())};
}

/**
 * Checks the projected sub-trie that compSubTrie computes for @p deltaTG against
 * expectedProjectedSubTrie: the same projected dimensions, and the same leaves with projected
 * coefficients equal up to round-off, except that leaves whose projected coefficient vanishes (up
 * to round-off) may be left out.
 */
void checkProjectedSubTrie(const DeltasSparseGrid& deltasSG, DeltaQuadratureData& data,
                           const TensorGridCTData& deltaTG) {
  tools::dsg_quadrature::compSubTrie(data.context, deltaTG.mi, data.scratch);

  const auto [projDims, expectedLeaves] = expectedProjectedSubTrie(deltasSG, deltaTG);
  BOOST_REQUIRE(data.scratch.projDims == projDims);

  const std::vector<MIWithCoeff> leaves = subTrieLeaves(data, deltaTG.mi);
  auto leaf = leaves.begin();

  for (const auto& [key, expectedCoeff] : expectedLeaves) {
    BOOST_TEST_CONTEXT("Projected levels " << toString(key)) {
      if (leaf != leaves.end() && leaf->first == key) {
        BOOST_CHECK_SMALL(leaf->second - expectedCoeff,
                          EPS * std::max(1.0, std::abs(expectedCoeff)));
        BOOST_CHECK_NE(leaf->second, 0.0);
        ++leaf;
      } else {
        BOOST_CHECK_SMALL(expectedCoeff, EPS);  // Left out: has to vanish
      }
    }
  }

  BOOST_CHECK(leaf == leaves.end());  // No other leaves
}

/**
 * Levels that are not in the (downwards closed) level set of @p deltasSG but whose components
 * exceed the maximum level of their dimension by at most one: for every dimension k, a level of
 * the set with the maximum k-th component, raised by one in dimension k + 1 (cyclically). Their
 * sub-tries are empty.
 */
std::vector<LvlMI> levelsOutside(const DeltasSparseGrid& deltasSG) {
  const size_t nDim = deltasSG.nDim();
  std::vector<LvlMI> result;

  for (size_t dim = 0; dim < nDim; dim++) {
    const TensorGridCTData* maxTG = &deltasSG.getTensorGrids()[0];

    for (const TensorGridCTData& tg : deltasSG) {
      if (tg.mi[dim] > maxTG->mi[dim]) maxTG = &tg;
    }

    LvlMI mi = maxTG->mi;
    mi[(dim + 1) % nDim]++;

    if (std::ranges::none_of(deltasSG.getTensorGrids(),
                             [&](const TensorGridCTData& tg) { return tg.mi == mi; })) {
      result.push_back(mi);
    }
  }

  return result;
}

/**
 * Checks the projected sub-trie of every delta tensor grid of @p deltasSG (checkProjectedSubTrie)
 * and that the sub-tries of the levels outside the level set (@ref levelsOutside) are empty.
 */
void checkSubTries(const DeltasSparseGrid& deltasSG) {
  const SourceFunc sourceFunc = genConstantFunc(1.0);
  DeltaQuadratureData data(deltasSG, sourceFunc);

  for (const TensorGridCTData& deltaTG : deltasSG) {
    BOOST_TEST_CONTEXT("Delta tensor grid " << toString(deltaTG.mi)) {
      checkProjectedSubTrie(deltasSG, data, deltaTG);
    }
  }

  for (const LvlMI& outsideMI : levelsOutside(deltasSG)) {
    BOOST_TEST_CONTEXT("Level outside the level set " << toString(outsideMI)) {
      BOOST_REQUIRE(nonZeroCoeffMIs(deltasSG, outsideMI).empty());
      tools::dsg_quadrature::compSubTrie(data.context, outsideMI, data.scratch);

      for (const auto& nodes : data.scratch.trieNodes) {
        BOOST_CHECK(nodes.empty());
      }
    }
  }
}

/**
 * Brute-force global weights of grid points of a sparse grid @p sg: the global weight of a grid
 * point is the sum of c_l * w_l(point) over all tensor grids of @p sg containing it, where w_l is
 * the tensor product of the 1D quadrature weights of the tensor grid. The 1D nodes and weights of
 * the tensor grids are precomputed once.
 */
class BruteForceGlobalWeights {
 public:
  explicit BruteForceGlobalWeights(const SparseGrid& sg) {
    const std::vector<NodeGenFunc*>& nodeGenFuncs = sg.getGenInstr().getNodeGenFuncs();

    for (const TensorGridCTData& tg : sg) {
      const GPMI& gpCntPerDim = tg.tensorGrid.getGPCntPerDim();
      const DataVector& nodesPerDim = tg.tensorGrid.getNodesPerDim();
      TG& entry = tgs.emplace_back(TG{static_cast<double>(tg.coefficient), {}, {}});
      size_t offset = 0;

      for (size_t dim = 0; dim < sg.nDim(); dim++) {
        const GPCntType nNodes = gpCntPerDim[dim];
        const auto nodesBegin = nodesPerDim.begin() + static_cast<std::ptrdiff_t>(offset);

        entry.nodes.emplace_back(std::vector<double>(nodesBegin, nodesBegin + nNodes));
        entry.weights.push_back(nodeGenFuncs[dim]->getQuadRule(nNodes)->genWeights(nNodes));
        offset += nNodes;
      }
    }
  }

  double operator()(const DataVector& point) const {
    double result = 0.0;

    for (const TG& tg : tgs) {
      double weight = tg.coeff;

      for (size_t dim = 0; dim < tg.nodes.size() && weight != 0.0; dim++) {
        const std::optional<GPCntType> nodeIdx = findNode(tg.nodes[dim], point[dim]);
        // 0 if the tensor grid does not contain the grid point
        weight = nodeIdx ? weight * tg.weights[dim][*nodeIdx] : 0.0;
      }

      result += weight;
    }

    return result;
  }

 private:
  struct TG {
    double coeff;
    std::vector<DataVector> nodes;    // nodes[dim]: the 1D nodes, ascending
    std::vector<DataVector> weights;  // weights[dim][i]: the 1D quadrature weight of nodes[dim][i]
  };

  std::vector<TG> tgs;
};

/**
 * The downwards closed anisotropic level set {l : sum_k a_k * l_k <= @p maxWeightedSum} with the
 * weights a_k = 1 + k / 2 (@p increasing) or a_k = 1 + (nDim - 1 - k) / 2 (integer division).
 * The 1-norm of the levels below a prefix does not determine which levels lie below it.
 */
LvlMIVec anisotropicLevels(const size_t nDim, const LvlType maxWeightedSum, const bool increasing) {
  std::vector<LvlType> weights(nDim);
  LvlMI upper(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    weights[dim] = static_cast<LvlType>(1 + (increasing ? dim : nDim - 1 - dim) / 2);
    upper[dim] = maxWeightedSum / weights[dim];
  }

  std::vector<LvlMI> levels;

  forEachMIInBox(upper, [&](const LvlMI& mi) {
    LvlType weightedSum = 0;
    for (size_t dim = 0; dim < nDim; dim++) weightedSum += weights[dim] * mi[dim];
    if (weightedSum <= maxWeightedSum) levels.push_back(mi);
  });

  return LvlMIVec(levels);
}

/// Permutation of 0, ..., n - 1 drawn with randGen (Fisher-Yates).
std::vector<size_t> randomPermutation(const size_t n) {
  std::vector<size_t> result(n);

  for (size_t i = 0; i < n; i++) result[i] = i;
  for (size_t i = n; i > 1; i--) std::swap(result[i - 1], result[randGen.getUniformIndexRN(i)]);

  return result;
}

/**
 * Closed-form number of unique grid points of the sparse grid with the downwards closed level set
 * @p levels: sum over l of prod_k (number of delta nodes of level l_k in dimension k).
 */
size_t expectedNumberOfUniqueGridPoints(const SGGenInstr& genInstr,
                                        const std::vector<LvlMI>& levels) {
  size_t result = 0;

  for (const LvlMI& mi : levels) {
    size_t nGP = 1;

    for (size_t dim = 0; dim < mi.size(); dim++) {
      const Lvl2GPCntFunc f = genInstr.getLvl2GPCntFuncForDim(dim);
      const size_t nInnerNodes = f(mi[dim]) - (mi[dim] == 0 ? 0 : f(mi[dim] - 1));
      nGP *= nInnerNodes + (mi[dim] == genInstr.getBoundaryLevelOffset() ? 2 : 0);
    }

    result += nGP;
  }

  return result;
}

}  // namespace

/********
Operator
********/

BOOST_AUTO_TEST_SUITE(quadratureDelta_operator)

BOOST_AUTO_TEST_CASE(Single_Tensor_Grid) {
  // Level 0: the sparse grid is the single tensor grid (0, ..., 0) with coefficient 1
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u}) {
      for (size_t nDim = 1; nDim <= 4; nDim++) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, boundary level offset "
                                               << boundaryLvlOffset << ", nDim " << nDim) {
          CompleteSGGenInstr genInstr(0, nDim);
          genInstr.setNodeGenFunc(nodeGenFunc);
          genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

          checkQuadResult(runQuadratureDelta(genProductFunc(), genInstr),
                          runQuadrature(genProductFunc(), genInstr));
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Constant_Function_Is_Exact) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
      for (const LvlType boundaryLvlOffset : {0u, 1u, 1000u}) {
        for (size_t nDim = 1; nDim <= 4; nDim++) {
          for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
            BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                               << " nodes, boundary level offset " << boundaryLvlOffset << ", nDim "
                               << nDim << ", max. level " << maxLvl) {
              const double value = randGen.getUniformRN(-2.0, 2.0);

              CompleteSGGenInstr genInstr(maxLvl, nDim);
              genInstr.setNodeGenFunc(nodeGenFunc);
              genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
              genInstr.setDomain(randomDomain(nDim));

              checkQuadResult(runQuadratureDelta(genConstantFunc(value), genInstr),
                              value * genInstr.getVolumeOfDomain());
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Multilinear_Function_Is_Exact) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
      for (const LvlType boundaryLvlOffset : {0u, 1u, 1000u}) {
        for (size_t nDim = 1; nDim <= 4; nDim++) {
          for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
            BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                               << " nodes, boundary level offset " << boundaryLvlOffset << ", nDim "
                               << nDim << ", max. level " << maxLvl) {
              std::vector<double> a(nDim);
              std::vector<double> b(nDim);
              for (size_t dim = 0; dim < nDim; dim++) {
                a[dim] = randGen.getUniformRN(-2.0, 2.0);
                b[dim] = randGen.getUniformRN(-2.0, 2.0);
              }

              CompleteSGGenInstr genInstr(maxLvl, nDim);
              genInstr.setNodeGenFunc(nodeGenFunc);
              genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
              genInstr.setDomain(randomDomain(nDim));

              checkQuadResult(runQuadratureDelta(genMultilinearFunc(a, b), genInstr),
                              integrateMultilinearFunc(a, b, genInstr.getDomain()));
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Matches_Quadrature_Complete) {
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
      for (size_t nDim = 1; nDim <= 4; nDim++) {
        for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
          BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                             << " nodes, boundary level offset " << boundaryLvlOffset << ", nDim "
                             << nDim << ", max. level " << maxLvl) {
            CompleteSGGenInstr genInstr(maxLvl, nDim);
            genInstr.setNodeGenFunc(nodeGenFunc);
            genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

            for (const SourceFunc& sourceFunc : {genProductFunc(), genGaussianFunc()}) {
              checkQuadResult(runQuadratureDelta(sourceFunc, genInstr),
                              runQuadrature(sourceFunc, genInstr));
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Matches_Quadrature_Higher_Dimensions) {
  // Deeper tries: most partial sums of the global weights are factored over several dimensions
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 1000u}) {
      for (size_t nDim = 5; nDim <= 7; nDim++) {
        for (LvlType maxLvl = 0; maxLvl <= 3; maxLvl++) {
          BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                             << " nodes, boundary level offset " << boundaryLvlOffset << ", nDim "
                             << nDim << ", max. level " << maxLvl) {
            CompleteSGGenInstr genInstr(maxLvl, nDim);
            genInstr.setNodeGenFunc(nodeGenFunc);
            genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

            checkQuadResult(runQuadratureDelta(genGaussianFunc(), genInstr),
                            runQuadrature(genGaussianFunc(), genInstr));
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Matches_Quadrature_High_Dimensions_Boundary_Offsets) {
  // Deep tries in which most dimensions of most delta tensor grids have a single node (boundary
  // level offset >= 1): complete sparse grids and anisotropic level sets. (With offset 0, every
  // tensor grid has at least 3^nDim grid points.)
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
      for (const LvlType boundaryLvlOffset : {1u, 2u}) {
        for (const size_t nDim : {8, 10, 12}) {
          for (LvlType maxLvl = 2; maxLvl <= 4; maxLvl++) {
            BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                               << " nodes, complete, boundary level offset " << boundaryLvlOffset
                               << ", nDim " << nDim << ", max. level " << maxLvl) {
              CompleteSGGenInstr genInstr(maxLvl, nDim);
              genInstr.setNodeGenFunc(nodeGenFunc);
              genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

              checkQuadResult(runQuadratureDelta(genGaussianFunc(), genInstr),
                              runQuadrature(genGaussianFunc(), genInstr), combinationTol(genInstr));
            }
          }
        }

        for (const size_t nDim : {8, 10}) {
          for (const LvlType maxWeightedSum : {4u, 6u}) {
            for (const bool increasing : {true, false}) {
              BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                                 << " nodes, anisotropic, boundary level offset "
                                 << boundaryLvlOffset << ", nDim " << nDim << ", max. weighted sum "
                                 << maxWeightedSum << (increasing ? ", increasing" : ", decreasing")
                                 << " weights") {
                const LvlMIVec levels = anisotropicLevels(nDim, maxWeightedSum, increasing);
                MIVecSGGenInstr genInstr(levels);
                genInstr.setNodeGenFunc(nodeGenFunc);
                genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
                genInstr.setDomain(randomDomain(nDim));

                checkQuadResult(runQuadratureDelta(genProductFunc(), genInstr),
                                runQuadrature(genProductFunc(), genInstr),
                                combinationTol(genInstr));
              }
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Matches_Quadrature_Full_Grid) {
  // A single input multi-index: the sparse grid is one full tensor grid of a non-zero level
  const LvlMIVec mis{{3, 2}};

  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 1000u}) {
      BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                         << " nodes, boundary level offset " << boundaryLvlOffset) {
        MIVecSGGenInstr genInstr(mis);
        genInstr.setNodeGenFunc(nodeGenFunc);
        genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

        checkQuadResult(runQuadratureDelta(genGaussianFunc(), genInstr),
                        runQuadrature(genGaussianFunc(), genInstr));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Matches_Quadrature_Random_Settings) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 2 + randGen.getUniformIndexRN(3);
      const size_t nInputMIs = 1 + randGen.getUniformIndexRN(4);

      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < nInputMIs; i++) {
        inputMIs.push_back(randomMI(nDim, 3));
      }
      const LvlMIVec mis(inputMIs);

      CompleteSGGenInstr completeGenInstr(static_cast<LvlType>(randGen.getUniformIndexRN(5)), nDim);
      MIVecSGGenInstr miVecGenInstr(mis);

      for (SGGenInstr* const genInstr : {static_cast<SGGenInstr*>(&completeGenInstr),
                                         static_cast<SGGenInstr*>(&miVecGenInstr)}) {
        for (size_t dim = 0; dim < nDim; dim++) {
          genInstr->setNodeGenFuncForDim(randGen.getUniformIndexRN(2) == 0
                                             ? equidistantNodeGenFunc
                                             : clenshawCurtisNodeGenFunc,
                                         dim);
        }
        genInstr->setBoundaryLevelOffset(static_cast<LvlType>(randGen.getUniformIndexRN(4)));
        genInstr->setDomain(randomDomain(nDim));

        BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim
                                   << (genInstr == &completeGenInstr ? ", complete" : ", MIVec")
                                   << ", boundary level offset "
                                   << genInstr->getBoundaryLevelOffset()) {
          checkQuadResult(runQuadratureDelta(genGaussianFunc(), *genInstr),
                          runQuadrature(genGaussianFunc(), *genInstr));
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Evaluates_Every_Grid_Point_Once) {
  // The purpose of the delta quadrature: no grid point is evaluated twice
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 1000u}) {
      for (size_t nDim = 2; nDim <= 3; nDim++) {
        for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
          BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                             << " nodes, boundary level offset " << boundaryLvlOffset << ", nDim "
                             << nDim << ", max. level " << maxLvl) {
            CompleteSGGenInstr genInstr(maxLvl, nDim);
            genInstr.setNodeGenFunc(nodeGenFunc);
            genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

            // The source function may be called concurrently. Points are recorded rounded to
            // multiples of 2^-30, far below the node distances.
            std::atomic<size_t> nEvaluations{0};
            std::mutex mutex;
            std::vector<std::vector<std::int64_t>> evaluatedPoints;

            const SourceFunc sourceFunc([&](const DataVector& point) {
              std::vector<std::int64_t> roundedPoint(point.size());
              for (size_t dim = 0; dim < point.size(); dim++) {
                roundedPoint[dim] = std::llround(std::ldexp(point[dim], 30));
              }

              nEvaluations++;
              const std::lock_guard<std::mutex> lock(mutex);
              evaluatedPoints.push_back(std::move(roundedPoint));
              return 1.0;
            });

            runQuadratureDelta(sourceFunc, genInstr);

            BOOST_CHECK_EQUAL(nEvaluations.load(), expectedNumberOfUniqueGridPoints(
                                                       genInstr, completeSGLevels(nDim, maxLvl)));

            std::sort(evaluatedPoints.begin(), evaluatedPoints.end());
            const size_t nDistinctPoints =
                static_cast<size_t>(std::unique(evaluatedPoints.begin(), evaluatedPoints.end()) -
                                    evaluatedPoints.begin());
            BOOST_CHECK_EQUAL(nDistinctPoints, evaluatedPoints.size());
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Non_Nested_Nodes_Throw) {
  // quadratureDelta requires nested nodes
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes") {
      CompleteSGGenInstr genInstr(2, 2);
      genInstr.setNodeGenFunc(nodeGenFunc);
      genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);

      BOOST_CHECK_THROW(runQuadratureDelta(genProductFunc(), genInstr), std::invalid_argument);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

/*****************
Internal operations
*****************/

BOOST_AUTO_TEST_SUITE(quadratureDelta_internals)

BOOST_AUTO_TEST_CASE(Coefficient_Trie_Structure) {
  // Shuffled input with a zero coefficient, which has to be skipped
  std::vector<TensorGridCTData> tgs;
  tgs.push_back(TensorGridCTData{LvlMI{1, 0}, 1, TensorGrid()});
  tgs.push_back(TensorGridCTData{LvlMI{2, 0}, 0, TensorGrid()});
  tgs.push_back(TensorGridCTData{LvlMI{0, 0}, -1, TensorGrid()});
  tgs.push_back(TensorGridCTData{LvlMI{0, 1}, 1, TensorGrid()});

  const misc::CTCoeffTrie trie(tgs, 2);

  // Lexicographic order: (0, 0), (0, 1), (1, 0). Nodes: {lvl, maxSuffixLvlSum, childEnd}
  using Node = misc::CTCoeffTrie::Node;
  const std::vector<Node> expectedDepth0{{0, 1, 2}, {1, 0, 3}};
  const std::vector<Node> expectedDepth1{{0, 0, 0}, {1, 0, 0}, {0, 0, 0}};
  const std::vector<double> expectedCoeffs{-1.0, 1.0, 1.0};

  BOOST_CHECK_EQUAL(trie.nDim(), size_t{2});
  BOOST_CHECK_EQUAL(trie.nLeaves(), size_t{3});
  BOOST_CHECK(std::ranges::equal(trie.nodesOfDepth(0), expectedDepth0));
  BOOST_CHECK(std::ranges::equal(trie.nodesOfDepth(1), expectedDepth1));
  BOOST_CHECK(std::ranges::equal(trie.leafCoeffs(), expectedCoeffs));
  BOOST_CHECK_EQUAL(trie.childBegin(0, 1), size_t{2});
  BOOST_CHECK_EQUAL(trie.childEnd(0, 1), size_t{3});

  // Only zero coefficients: empty trie
  const misc::CTCoeffTrie emptyTrie{std::span<const TensorGridCTData>(tgs).subspan(1, 1), 2};
  BOOST_CHECK_EQUAL(emptyTrie.nLeaves(), size_t{0});
  BOOST_CHECK(emptyTrie.nodesOfDepth(0).empty());
  BOOST_CHECK(emptyTrie.nodesOfDepth(1).empty());
}

BOOST_AUTO_TEST_CASE(Coefficient_Trie_Of_Sparse_Grids) {
  // The leaves of the trie are exactly the levels of the tensor grids with a non-zero coefficient
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t nDim = 1; nDim <= 5; nDim++) {
      for (LvlType maxLvl = 0; maxLvl <= 5; maxLvl++) {
        BOOST_TEST_CONTEXT("Complete, nDim " << nDim << ", max. level " << maxLvl) {
          const DeltasSparseGrid deltasSG{CompleteSGGenInstr(maxLvl, nDim)};
          const misc::CTCoeffTrie trie(deltasSG.getTensorGrids(), nDim);

          BOOST_CHECK(trieLeaves(trie) == nonZeroCoeffMIs(deltasSG, LvlMI(nDim, LvlType{0})));
        }
      }
    }

    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 2 + randGen.getUniformIndexRN(3);
      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < 1 + randGen.getUniformIndexRN(4); i++) {
        inputMIs.push_back(randomMI(nDim, 3));
      }
      const LvlMIVec mis(inputMIs);

      BOOST_TEST_CONTEXT("MIVec, run " << run << ", nDim " << nDim) {
        const DeltasSparseGrid deltasSG{MIVecSGGenInstr(mis)};
        const misc::CTCoeffTrie trie(deltasSG.getTensorGrids(), nDim);

        BOOST_CHECK(trieLeaves(trie) == nonZeroCoeffMIs(deltasSG, LvlMI(nDim, LvlType{0})));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Coefficient_Trie_Independent_Of_Input_Order) {
  // The trie is built from the tensor grids in any order: shuffled input gives the same trie
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlMIVec anisotropic = anisotropicLevels(8, 6, true);
    const DeltasSparseGrid complete{CompleteSGGenInstr(4, 6)};
    const DeltasSparseGrid miVec{MIVecSGGenInstr(anisotropic)};

    for (const DeltasSparseGrid* const deltasSG : {&complete, &miVec}) {
      BOOST_TEST_CONTEXT((deltasSG == &complete ? "Complete" : "Anisotropic")) {
        const std::span<const TensorGridCTData> tgs = deltasSG->getTensorGrids();
        std::vector<TensorGridCTData> shuffled;
        for (const size_t idx : randomPermutation(tgs.size())) shuffled.push_back(tgs[idx]);

        const misc::CTCoeffTrie trie(tgs, deltasSG->nDim());
        const misc::CTCoeffTrie shuffledTrie(shuffled, deltasSG->nDim());

        for (size_t depth = 0; depth < trie.nDim(); depth++) {
          BOOST_CHECK(
              std::ranges::equal(trie.nodesOfDepth(depth), shuffledTrie.nodesOfDepth(depth)));
        }
        BOOST_CHECK(std::ranges::equal(trie.leafCoeffs(), shuffledTrie.leafCoeffs()));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(One_Dimensional_Weights) {
  // weights1D[dim][lvl] are the 1D quadrature weights of the regular nodes of level lvl in delta
  // order: delta level by delta level, each in the order of the delta nodes. The quadrature rule
  // has to be chosen for the number of regular nodes: e.g. for equidistant nodes, Simpson or
  // trapezoidal rule depends on its parity, which differs from the parity of the number of delta
  // nodes.
  for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
    BOOST_TEST_CONTEXT("Boundary level offset " << boundaryLvlOffset) {
      constexpr LvlType maxLvl = 4;

      CompleteSGGenInstr genInstr(maxLvl, 3);
      genInstr.setNodeGenFuncs(
          {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
      genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
      const DeltasSparseGrid deltasSG(genInstr);

      const misc::FlatNestedVec<double, 3> weights = tools::dsg_quadrature::comp1DWeights(deltasSG);

      BOOST_REQUIRE_EQUAL(weights.nEntries(1), size_t{3});

      for (size_t dim = 0; dim < 3; dim++) {
        BOOST_REQUIRE_EQUAL(weights[dim].size(), size_t{maxLvl + 1});

        for (LvlType lvl = 0; lvl <= maxLvl; lvl++) {
          const DataVector nodes = regularNodes(genInstr, dim, lvl);
          const GPCntType nNodes = static_cast<GPCntType>(nodes.size());
          const DataVector expected =
              genInstr.getNodeGenFuncForDim(dim)->getQuadRule(nNodes)->genWeights(nNodes);
          const std::span<const double> weightsOfLvl = weights[dim][lvl];

          BOOST_TEST_CONTEXT("Dimension " << dim << ", level " << lvl) {
            BOOST_REQUIRE_EQUAL(weightsOfLvl.size(), expected.size());

            size_t idx = 0;
            double sum = 0.0;

            for (LvlType deltaLvl = 0; deltaLvl <= lvl; deltaLvl++) {
              // The delta nodes of deltaLvl follow the ones of the lower levels
              BOOST_CHECK_EQUAL(idx, deltaLvl == 0 ? 0 : weights[dim][deltaLvl - 1].size());

              for (const double deltaNode : expectedDeltaNodes(genInstr, dim, deltaLvl)) {
                const std::optional<GPCntType> nodeIdx = findNode(nodes, deltaNode);

                BOOST_REQUIRE(nodeIdx.has_value());
                BOOST_REQUIRE_LT(idx, weightsOfLvl.size());
                BOOST_CHECK_SMALL(weightsOfLvl[idx] - expected[*nodeIdx], EPS);
                sum += weightsOfLvl[idx++];
              }
            }

            BOOST_CHECK_EQUAL(idx, weightsOfLvl.size());
            BOOST_CHECK_SMALL(sum - 1.0, EPS);
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Sub_Trie_Of_Delta_Tensor_Grids) {
  // compSubTrie: the leaves of the projected sub-trie are the levels of the tensor grids l >= m
  // with a non-zero coefficient in the dimensions in which the delta tensor grid has more than one
  // node, with the projected coefficients; every inner node has a child, and every node of a
  // projected depth with level l_k points to the weights of the delta nodes of level m_k in the
  // rule of level l_k
  for (const LvlType boundaryLvlOffset : {0u, 1u, 1000u}) {
    for (size_t nDim = 1; nDim <= 4; nDim++) {
      for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
        BOOST_TEST_CONTEXT("Boundary level offset " << boundaryLvlOffset << ", nDim " << nDim
                                                    << ", max. level " << maxLvl) {
          CompleteSGGenInstr genInstr(maxLvl, nDim);
          genInstr.setNodeGenFunc(clenshawCurtisNodeGenFunc);
          genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

          checkSubTries(DeltasSparseGrid(genInstr));
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Sub_Trie_Of_Delta_Tensor_Grids_High_Dimensions) {
  // As Sub_Trie_Of_Delta_Tensor_Grids for deep tries: complete sparse grids (where all tensor
  // grids below a prefix have the same maximum level sum), anisotropic and random level sets
  // (where they do not), with the boundary on level 0, 1 or 2
  for (const LvlType boundaryLvlOffset : {0u, 1u, 2u}) {
    for (const size_t nDim : {8, 10}) {
      for (const LvlType maxLvl : {2u, 3u}) {
        BOOST_TEST_CONTEXT("Complete, boundary level offset "
                           << boundaryLvlOffset << ", nDim " << nDim << ", max. level " << maxLvl) {
          CompleteSGGenInstr genInstr(maxLvl, nDim);
          genInstr.setNodeGenFunc(equidistantNodeGenFunc);
          genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

          checkSubTries(DeltasSparseGrid(genInstr));
        }
      }

      for (const LvlType maxWeightedSum : {4u, 6u}) {
        for (const bool increasing : {true, false}) {
          BOOST_TEST_CONTEXT("Anisotropic, boundary level offset "
                             << boundaryLvlOffset << ", nDim " << nDim << ", max. weighted sum "
                             << maxWeightedSum << (increasing ? ", increasing" : ", decreasing")
                             << " weights") {
            const LvlMIVec levels = anisotropicLevels(nDim, maxWeightedSum, increasing);
            MIVecSGGenInstr genInstr(levels);
            genInstr.setNodeGenFunc(equidistantNodeGenFunc);
            genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

            checkSubTries(DeltasSparseGrid(genInstr));
          }
        }
      }
    }
  }

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 10; run++) {
      const size_t nDim = 8 + randGen.getUniformIndexRN(2);
      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < 1 + randGen.getUniformIndexRN(4); i++) {
        inputMIs.push_back(randomMI(nDim, 2));
      }
      const LvlMIVec mis(inputMIs);

      BOOST_TEST_CONTEXT("MIVec, run " << run << ", nDim " << nDim) {
        MIVecSGGenInstr genInstr(mis);
        genInstr.setBoundaryLevelOffset(static_cast<LvlType>(randGen.getUniformIndexRN(3)));

        checkSubTries(DeltasSparseGrid(genInstr));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Node_Index_Map_Functions) {
  CompleteSGGenInstr genInstr(2, 3);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  const DeltasSparseGrid deltasSG(genInstr);

  const std::vector<NodeIdxLvlMapFunc> maps =
      tools::dsg_quadrature::compNodeIdxLvlMapFuncs(deltasSG);

  BOOST_REQUIRE_EQUAL(maps.size(), size_t{3});
  for (size_t dim = 0; dim < 3; dim++) {
    const NodeGenFunc* const nodeGenFunc = genInstr.getNodeGenFuncForDim(dim);

    BOOST_CHECK(maps[dim] ==
                nodeGenFunc->getNodeIdxMapFromDeltaTGLvlToRegularLvl(doublingLvl2GPCntFunction));
  }
}

BOOST_AUTO_TEST_CASE(Map_Node_Idx_Preserves_The_Node) {
  // Maps the index of a delta node (with the boundary nodes on the boundary level offset) to the
  // index of the same node in a regular tensor grid of a higher or equal level
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 5u, 1000u}) {
      CompleteSGGenInstr genInstr(0, 1);
      genInstr.setNodeGenFunc(nodeGenFunc);
      genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

      const NodeIdxLvlMapFunc map =
          nodeGenFunc->getNodeIdxMapFromDeltaTGLvlToRegularLvl(doublingLvl2GPCntFunction);

      for (LvlType deltaLvl = 0; deltaLvl <= 5; deltaLvl++) {
        const DataVector deltaNodes = expectedDeltaNodes(genInstr, 0, deltaLvl);

        for (LvlType targetLvl = deltaLvl; targetLvl <= 6; targetLvl++) {
          BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                             << " nodes, boundary level offset " << boundaryLvlOffset
                             << ", delta level " << deltaLvl << " -> level " << targetLvl) {
            const DataVector targetNodes = regularNodes(genInstr, 0, targetLvl);

            for (GPCntType i = 0; i < deltaNodes.size(); i++) {
              const GPCntType mappedIdx =
                  tools::dsg_quadrature::mapNodeIdx(static_cast<GPCntType>(deltaNodes.size()),
                                                    static_cast<GPCntType>(targetNodes.size()),
                                                    deltaLvl, targetLvl, i, map, boundaryLvlOffset);

              BOOST_TEST_CONTEXT("Delta node index " << i << " -> " << mappedIdx) {
                BOOST_REQUIRE_LT(mappedIdx, targetNodes.size());
                BOOST_CHECK_SMALL(targetNodes[mappedIdx] - deltaNodes[i], EPS);
              }
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Global_Weights_Of_Delta_Grid_Points) {
  // forEachGPWithGlobalWeight visits the grid points of a delta tensor grid in its order, mapped
  // onto the domain, with their global weights
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
      for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
        for (size_t nDim = 1; nDim <= 3; nDim++) {
          for (LvlType maxLvl = 0; maxLvl <= 3; maxLvl++) {
            BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                               << " nodes, boundary level offset " << boundaryLvlOffset << ", nDim "
                               << nDim << ", max. level " << maxLvl) {
              CompleteSGGenInstr genInstr(maxLvl, nDim);
              genInstr.setNodeGenFunc(nodeGenFunc);
              genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
              genInstr.setDomain(randomDomain(nDim));
              const HyperCubeArea& domain = genInstr.getDomain();

              // The regular sparse grid is only used for the brute-force reference
              const SparseGrid sg(genInstr);
              const BruteForceGlobalWeights bruteForceGlobalWeight(sg);
              const DeltasSparseGrid deltasSG(genInstr);
              const SourceFunc sourceFunc = genConstantFunc(1.0);
              DeltaQuadratureData data(deltasSG, sourceFunc);

              // The global weights of all grid points sum up to the sum of the coefficients, 1
              double sumOfGlobalWeights = 0.0;

              for (const LvlMI& deltaMI : completeSGLevels(nDim, maxLvl)) {
                const TensorGridCTData deltaTG = expectedDeltaTGData(deltaMI, genInstr);

                std::vector<std::pair<DataVector, double>> visited;
                tools::dsg_quadrature::forEachGPWithGlobalWeight(
                    data.context, deltaTG, data.scratch,
                    [&](const DataVector& gp, const double globalWeight) {
                      visited.emplace_back(gp, globalWeight);
                    });

                BOOST_REQUIRE_EQUAL(visited.size(), deltaTG.tensorGrid.nGP());
                size_t gpIdx = 0;

                for (const auto& point : deltaTG.tensorGrid.gridPointsImmutable()) {
                  const auto& [gp, globalWeight] = visited[gpIdx++];

                  BOOST_TEST_CONTEXT("Delta tensor grid " << toString(deltaMI) << ", grid point "
                                                          << point.gp.toString()) {
                    for (size_t dim = 0; dim < nDim; dim++) {
                      const double width = domain[dim].second - domain[dim].first;
                      BOOST_CHECK_SMALL(gp[dim] - (point.gp[dim] * width + domain[dim].first), EPS);
                    }

                    BOOST_CHECK_SMALL(globalWeight - bruteForceGlobalWeight(point.gp), EPS);
                  }

                  sumOfGlobalWeights += globalWeight;
                }
              }

              BOOST_CHECK_SMALL(sumOfGlobalWeights - 1.0, EPS);
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Global_Weights_Of_Delta_Grid_Points_High_Dimensions) {
  // As Global_Weights_Of_Delta_Grid_Points for deep tries (most dimensions of most delta tensor
  // grids have a single node if the boundary level offset is >= 1), complete and anisotropic
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    constexpr size_t nDim = 8;
    const LvlMIVec anisotropic = anisotropicLevels(nDim, 6, true);

    for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
      for (const LvlType boundaryLvlOffset : {1u, 2u}) {
        CompleteSGGenInstr complete2(2, nDim);
        CompleteSGGenInstr complete3(3, nDim);
        MIVecSGGenInstr miVec(anisotropic);

        for (SGGenInstr* const genInstr :
             {static_cast<SGGenInstr*>(&complete2), static_cast<SGGenInstr*>(&complete3),
              static_cast<SGGenInstr*>(&miVec)}) {
          BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                             << " nodes, boundary level offset " << boundaryLvlOffset << ", "
                             << (genInstr == &miVec       ? "anisotropic"
                                 : genInstr == &complete2 ? "complete, max. level 2"
                                                          : "complete, max. level 3")) {
            genInstr->setNodeGenFunc(nodeGenFunc);
            genInstr->setBoundaryLevelOffset(boundaryLvlOffset);
            genInstr->setDomain(randomDomain(nDim));
            const HyperCubeArea& domain = genInstr->getDomain();

            const SparseGrid sg(*genInstr);
            const BruteForceGlobalWeights bruteForceGlobalWeight(sg);
            const DeltasSparseGrid deltasSG(*genInstr);
            const SourceFunc sourceFunc = genConstantFunc(1.0);
            DeltaQuadratureData data(deltasSG, sourceFunc);

            double sumOfGlobalWeights = 0.0;
            size_t nChecked = 0;
            size_t nGP = 0;

            for (const TensorGridCTData& deltaTG : deltasSG) {
              nGP += deltaTG.tensorGrid.nGP();
              tools::dsg_quadrature::forEachGPWithGlobalWeight(
                  data.context, deltaTG, data.scratch,
                  [&](const DataVector& gp, const double globalWeight) {
                    // Back onto the unit domain, where the tensor grids of sg are
                    DataVector unitGP(nDim);
                    for (size_t dim = 0; dim < nDim; dim++) {
                      unitGP[dim] =
                          (gp[dim] - domain[dim].first) / (domain[dim].second - domain[dim].first);
                    }

                    if (std::abs(globalWeight - bruteForceGlobalWeight(unitGP)) > EPS) {
                      BOOST_ERROR("Delta tensor grid " << toString(deltaTG.mi) << ", grid point "
                                                       << unitGP.toString() << ": global weight "
                                                       << globalWeight << ", expected "
                                                       << bruteForceGlobalWeight(unitGP));
                    }

                    sumOfGlobalWeights += globalWeight;
                    nChecked++;
                  });
            }

            BOOST_CHECK_EQUAL(nChecked, nGP);
            BOOST_CHECK_SMALL(sumOfGlobalWeights - 1.0, EPS);
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Delta_Tensor_Grids_Independent_Of_Visit_Order) {
  // The scratch is reused across the delta tensor grids a thread processes, in an order that
  // depends on the scheduling: the grid points, their global weights and the result of every
  // delta tensor grid must be bitwise the same as with a fresh scratch
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    constexpr size_t nDim = 8;
    const LvlMIVec anisotropic = anisotropicLevels(nDim, 6, false);

    for (const LvlType boundaryLvlOffset : {0u, 1u, 2u}) {
      // Offset 0: every delta tensor grid has >= 3^nDim grid points
      CompleteSGGenInstr complete(boundaryLvlOffset == 0 ? 2 : 3, nDim);
      MIVecSGGenInstr miVec(anisotropic);

      for (SGGenInstr* const genInstr :
           {static_cast<SGGenInstr*>(&complete), static_cast<SGGenInstr*>(&miVec)}) {
        BOOST_TEST_CONTEXT("Boundary level offset "
                           << boundaryLvlOffset
                           << (genInstr == &miVec ? ", anisotropic" : ", complete")) {
          genInstr->setNodeGenFunc(clenshawCurtisNodeGenFunc);
          genInstr->setBoundaryLevelOffset(boundaryLvlOffset);
          genInstr->setDomain(randomDomain(nDim));

          const DeltasSparseGrid deltasSG(*genInstr);
          const SourceFunc sourceFunc = genGaussianFunc();
          DeltaQuadratureData shared(deltasSG, sourceFunc);

          using Visited = std::vector<std::pair<std::vector<double>, double>>;
          const auto visit = [&](const TensorGridCTData& deltaTG,
                                 misc::DeltaQuadratureScratch& scratch) {
            Visited visited;
            tools::dsg_quadrature::forEachGPWithGlobalWeight(
                shared.context, deltaTG, scratch,
                [&](const DataVector& gp, const double globalWeight) {
                  visited.emplace_back(std::vector<double>(gp.begin(), gp.end()), globalWeight);
                });
            return visited;
          };

          for (const size_t tgIdx : randomPermutation(deltasSG.nTG())) {
            const TensorGridCTData& deltaTG = deltasSG.getTensorGrid(tgIdx);
            misc::DeltaQuadratureScratch fresh(nDim, deltasSG.getMaxTGSumOverGPCntsPerDim());

            BOOST_TEST_CONTEXT("Delta tensor grid " << toString(deltaTG.mi)) {
              BOOST_CHECK(visit(deltaTG, shared.scratch) == visit(deltaTG, fresh));
              BOOST_CHECK_EQUAL(tools::dsg_quadrature::processDeltaTensorGrid(
                                    shared.context, sourceFunc, deltaTG, shared.scratch),
                                tools::dsg_quadrature::processDeltaTensorGrid(
                                    shared.context, sourceFunc, deltaTG, fresh));
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Delta_Tensor_Grids_Reproduce_Quadrature) {
  // Summing processDeltaTensorGrid over the delta tensor grids of all levels of the downwards
  // closed set gives the quadrature (on the unit domain, i.e. before scaling with the volume)
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
      for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
        for (size_t nDim = 2; nDim <= 3; nDim++) {
          for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
            BOOST_TEST_CONTEXT(nameOf(nodeGenFunc)
                               << " nodes, boundary level offset " << boundaryLvlOffset << ", nDim "
                               << nDim << ", max. level " << maxLvl) {
              CompleteSGGenInstr genInstr(maxLvl, nDim);
              genInstr.setNodeGenFunc(nodeGenFunc);
              genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
              genInstr.setDomain(randomDomain(nDim));

              const SparseGrid sg(genInstr);
              const DeltasSparseGrid deltasSG(genInstr);
              const SourceFunc sourceFunc = genGaussianFunc();
              DeltaQuadratureData data(deltasSG, sourceFunc);

              double result = 0.0;
              for (const LvlMI& deltaMI : completeSGLevels(nDim, maxLvl)) {
                result += tools::dsg_quadrature::processDeltaTensorGrid(
                    data.context, sourceFunc, expectedDeltaTGData(deltaMI, genInstr), data.scratch);
              }

              checkQuadResult(result * genInstr.getVolumeOfDomain(), quadrature(sourceFunc, sg));
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
