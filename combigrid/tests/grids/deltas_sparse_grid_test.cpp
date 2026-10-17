// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file deltas_sparse_grid_test.cpp
 * @brief Boost.Test cases for @c DeltasSparseGrid.
 *
 * Covered are the construction from @c CompleteSGGenInstr and @c MIVecSGGenInstr (all
 * multi-indices of the downwards closed set, including those with combination coefficient 0,
 * the coefficients, the delta node lists, boundary handling, cached meta values and the stored
 * generation instruction) and the defining property of a deltas sparse grid: iterating over the
 * grid points of all of its tensor grids yields every grid point of the identically constructed
 * @c SparseGrid exactly once, and no other point.
 *
 * The expected delta tensor grids are assembled by hand. In every dimension, the delta nodes of
 * level l are the regular level-l nodes (@c NodeGenFunc::genNodesWithoutBoundary, framed by 0 and
 * 1 iff l >= boundary level offset) that are no regular level-(l - 1) nodes. Grid points are
 * compared via their per-dimension indices among the nodes of the finest level, which makes the
 * comparison of grid points exact.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/math/binomial.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();
static NodeGenFunc* const equidistantNodeGenFunc = getEquidistantNodeGenFunc();
static NodeGenFunc* const clenshawCurtisNodeGenFunc = getClenshawCurtisNodeGenFunc();
static NodeGenFunc* const firstTypeChebyshevNodeGenFunc = getFirstTypeChebyshevNodeGenFunc();

namespace {

constexpr double EPS = 1e-12;

/// Concrete sparse grid with a public constructor and modifier, used to assemble expected
/// grids by hand.
class ExpectedSG : public SparseGridBase {
 public:
  explicit ExpectedSG(const size_t nDim) : SparseGridBase(nDim) {}

  using SparseGridBase::addTensorGrid;
};

/// String representation of @p mi for test messages, e.g. "(1, 0, 2)".
std::string toString(const LvlMI& mi) {
  std::string result = "(";

  for (size_t dim = 0; dim < mi.size(); dim++) {
    result += (dim == 0 ? "" : ", ") + std::to_string(mi[dim]);
  }

  return result + ")";
}

/// String representation of @p point for test messages, e.g. "(0.5, 0.25)".
std::string toString(const DataVector& point) {
  std::ostringstream result;
  result << "(";

  for (size_t dim = 0; dim < point.size(); dim++) {
    result << (dim == 0 ? "" : ", ") << point[dim];
  }

  result << ")";
  return result.str();
}

std::string nameOf(const NodeGenFunc* nodeGenFunc) {
  if (nodeGenFunc == equidistantNodeGenFunc) return "equidistant";
  if (nodeGenFunc == clenshawCurtisNodeGenFunc) return "Clenshaw-Curtis";
  if (nodeGenFunc == firstTypeChebyshevNodeGenFunc) return "first type Chebyshev";
  return "unknown";
}

/**
 * Regular 1D nodes of level @p lvl in dimension @p dim, i.e. the nodes of a regular tensor grid:
 * Lvl2GPCntFunc(lvl) inner nodes, framed by 0 and 1 iff lvl >= boundary level offset.
 */
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

/**
 * Closed-form number of delta nodes of level @p lvl in dimension @p dim: the Lvl2GPCntFunc(lvl) -
 * Lvl2GPCntFunc(lvl - 1) new inner nodes, plus the boundary nodes iff lvl is the boundary level
 * offset (the first level with boundary nodes).
 */
GPCntType expectedDeltaNodeCnt(const SGGenInstr& genInstr, const size_t dim, const LvlType lvl) {
  const Lvl2GPCntFunc lvl2GPCntFunc = genInstr.getLvl2GPCntFuncForDim(dim);
  const GPCntType nInnerNodes = lvl2GPCntFunc(lvl) - (lvl == 0 ? 0 : lvl2GPCntFunc(lvl - 1));

  return nInnerNodes + (lvl == genInstr.getBoundaryLevelOffset() ? 2 : 0);
}

/// Tensor grid with the given per-dimension node lists.
TensorGrid makeTG(const std::vector<DataVector>& nodesPerDim) {
  GPMI gpCntPerDim(nodesPerDim.size());
  size_t nNodes = 0;

  for (size_t dim = 0; dim < nodesPerDim.size(); dim++) {
    gpCntPerDim[dim] = static_cast<GPCntType>(nodesPerDim[dim].size());
    nNodes += nodesPerDim[dim].size();
  }

  DataVector concatenatedNodes(nNodes);
  auto out = concatenatedNodes.begin();
  for (const DataVector& nodes : nodesPerDim) {
    out = std::copy(nodes.begin(), nodes.end(), out);
  }

  return TensorGrid(gpCntPerDim, concatenatedNodes);
}

/// Delta tensor grid of level @p mi: the tensor product of the per-dimension delta nodes.
TensorGrid expectedDeltaTG(const LvlMI& mi, const SGGenInstr& genInstr) {
  std::vector<DataVector> nodesPerDim;

  for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
    nodesPerDim.push_back(expectedDeltaNodes(genInstr, dim, mi[dim]));
  }

  return makeTG(nodesPerDim);
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

/**
 * Expected deltas sparse grid of a complete sparse grid of level @p maxLvl: every l with
 * |l|_1 <= maxLvl, also those whose closed-form coefficient (-1)^q * binom(d - 1, q),
 * q = maxLvl - |l|_1, is zero.
 */
ExpectedSG genExpectedCompleteDeltasSG(const SGGenInstr& genInstr, const LvlType maxLvl) {
  const size_t nDim = genInstr.nDim();
  ExpectedSG expected(nDim);

  forEachMIInBox(LvlMI(nDim, maxLvl), [&](const LvlMI& mi) {
    const LvlType lvlSum = mi.sumOfElems();
    if (lvlSum > maxLvl) return;

    const CTCoeffType q = static_cast<CTCoeffType>(maxLvl - lvlSum);
    const CTCoeffType coeff =
        (q % 2 == 0 ? 1 : -1) * tools::binomial(static_cast<CTCoeffType>(nDim) - 1, q);

    expected.addTensorGrid(TensorGridCTData{mi, coeff, expectedDeltaTG(mi, genInstr)});
  });

  return expected;
}

/**
 * Expected deltas sparse grid of an @c MIVecSGGenInstr with input @p inputMIs, by brute force:
 * every l of the downwards closure I of the input, with the coefficient given by the
 * inclusion-exclusion formula c_l = sum over z in {0,1}^d with l + z in I of (-1)^|z|_1.
 */
ExpectedSG genExpectedMIVecDeltasSG(const std::vector<LvlMI>& inputMIs,
                                    const SGGenInstr& genInstr) {
  const size_t nDim = genInstr.nDim();

  const auto isInClosure = [&inputMIs, nDim](const LvlMI& mi) {
    return std::any_of(inputMIs.begin(), inputMIs.end(), [&mi, nDim](const LvlMI& inputMI) {
      for (size_t dim = 0; dim < nDim; dim++) {
        if (mi[dim] > inputMI[dim]) return false;
      }
      return true;
    });
  };

  LvlMI upper(nDim, LvlType{0});
  for (const LvlMI& inputMI : inputMIs) {
    for (size_t dim = 0; dim < nDim; dim++) {
      upper[dim] = std::max(upper[dim], inputMI[dim]);
    }
  }

  ExpectedSG expected(nDim);

  forEachMIInBox(upper, [&](const LvlMI& mi) {
    if (!isInClosure(mi)) return;

    CTCoeffType coeff = 0;
    forEachMIInBox(LvlMI(nDim, LvlType{1}), [&](const LvlMI& z) {
      if (isInClosure(mi + z)) {
        coeff += z.sumOfElems() % 2 == 0 ? 1 : -1;
      }
    });

    expected.addTensorGrid(TensorGridCTData{mi, coeff, expectedDeltaTG(mi, genInstr)});
  });

  return expected;
}

/**
 * Checks that @p result contains exactly the tensor grids of @p expected (in any order). Every
 * mismatching tensor grid is reported separately before the overall comparison.
 */
void checkSG(const SparseGridBase& result, const SparseGridBase& expected) {
  BOOST_CHECK_EQUAL(result.nDim(), expected.nDim());
  BOOST_CHECK_EQUAL(result.nTG(), expected.nTG());

  for (const TensorGridCTData& expectedTGData : expected) {
    BOOST_TEST_CONTEXT("Tensor grid " << toString(expectedTGData.mi)) {
      const auto iter = result.getTensorGrid(expectedTGData.mi);

      if (iter == result.end()) {
        BOOST_ERROR("Tensor grid is missing");
      } else {
        BOOST_CHECK_EQUAL(iter->coefficient, expectedTGData.coefficient);
        BOOST_CHECK(iter->tensorGrid.getGPCntPerDim() ==
                    expectedTGData.tensorGrid.getGPCntPerDim());
        BOOST_CHECK(iter->tensorGrid == expectedTGData.tensorGrid);
      }
    }
  }

  BOOST_CHECK(result == expected);
}

/// Checks the cached meta values of @p sg against values computed from its tensor grids.
void checkMetaValues(const SparseGridBase& sg) {
  size_t maxTGGPCnt = 0;
  size_t maxTGSumOverGPCntsPerDim = 0;

  for (const TensorGridCTData& tg : sg) {
    maxTGGPCnt = std::max(maxTGGPCnt, tg.tensorGrid.getGPCntPerDim().productofElems<size_t>());
    maxTGSumOverGPCntsPerDim =
        std::max(maxTGSumOverGPCntsPerDim, tg.tensorGrid.getGPCntPerDim().sumOfElems<size_t>());
  }

  BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), maxTGGPCnt);
  BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), maxTGSumOverGPCntsPerDim);
}

/// Sum of all combination coefficients of @p sg.
CTCoeffType sumOfCoeffs(const SparseGridBase& sg) {
  CTCoeffType sum = 0;

  for (const TensorGridCTData& tg : sg) {
    sum += tg.coefficient;
  }

  return sum;
}

/**
 * Assigns random nested node generation functions (equidistant or Clenshaw-Curtis) per dimension,
 * the doubling growth function and a random boundary level offset in {0, 1, 2, 3}. Deltas sparse
 * grids require nested nodes.
 */
void setRandomNestedSettings(SGGenInstr& genInstr) {
  NodeGenFunc* const nodeGenFuncs[] = {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc};

  for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
    genInstr.setNodeGenFuncForDim(nodeGenFuncs[randGen.getUniformIndexRN(2)], dim);
  }

  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(static_cast<LvlType>(randGen.getUniformIndexRN(4)));
}

/// Checks that @p result has the same settings as @p expected.
void checkSameSettings(const SGGenInstr& result, const SGGenInstr& expected) {
  BOOST_CHECK_EQUAL(result.nDim(), expected.nDim());
  BOOST_CHECK_EQUAL(result.getBoundaryLevelOffset(), expected.getBoundaryLevelOffset());
  BOOST_CHECK(result.getDomain() == expected.getDomain());
  BOOST_CHECK(result.getNodeGenFuncs() == expected.getNodeGenFuncs());
  BOOST_CHECK(result.getLvl2GPCntFuncs() == expected.getLvl2GPCntFuncs());
}

/// Random multi-index with components in [0, @p maxComp].
LvlMI randomMI(const size_t nDim, const LvlType maxComp) {
  LvlMI mi(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(maxComp + 1));
  }

  return mi;
}

/******************************
Grid point partition utilities
******************************/

/// Exact representation of a grid point: its per-dimension indices among the finest nodes.
using GPKey = std::vector<GPCntType>;

/**
 * The finest 1D nodes per dimension of @p sg: the regular nodes of the maximum level of the
 * tensor grids of @p sg in that dimension. For nested nodes, every grid point of the sparse grid
 * is a tensor product of finest nodes.
 */
std::vector<DataVector> finestNodesPerDim(const SparseGridBase& sg, const SGGenInstr& genInstr) {
  std::vector<DataVector> result;

  for (size_t dim = 0; dim < sg.nDim(); dim++) {
    LvlType maxLvl = 0;
    for (const TensorGridCTData& tg : sg) {
      maxLvl = std::max(maxLvl, tg.mi[dim]);
    }

    result.push_back(regularNodes(genInstr, dim, maxLvl));
  }

  return result;
}

std::string toString(const GPKey& key, const std::vector<DataVector>& finestNodes) {
  DataVector point(key.size());

  for (size_t dim = 0; dim < key.size(); dim++) {
    point[dim] = finestNodes[dim][key[dim]];
  }

  return toString(point);
}

/// Grid points obtained by iterating over all grid points of all tensor grids of a sparse grid.
struct GPCounts {
  std::map<GPKey, size_t> counts;  ///< Number of occurrences of every grid point of the finest grid
  size_t nIterated = 0;            ///< Number of iterated grid points (with repetitions)
  size_t nForeign = 0;             ///< Number of iterated grid points not on the finest grid
  std::string firstForeign;        ///< Description of the first such grid point
};

/// Iterates over all grid points of all tensor grids of @p sg and counts them.
GPCounts countGridPoints(const SparseGridBase& sg, const std::vector<DataVector>& finestNodes) {
  GPCounts result;
  GPKey key(sg.nDim());

  for (const TensorGridCTData& tg : sg) {
    for (const auto& point : tg.tensorGrid.gridPointsImmutable()) {
      result.nIterated++;
      bool isOnFinestGrid = true;

      for (size_t dim = 0; dim < sg.nDim() && isOnFinestGrid; dim++) {
        const std::optional<GPCntType> nodeIdx = findNode(finestNodes[dim], point.gp[dim]);

        isOnFinestGrid = nodeIdx.has_value();
        key[dim] = nodeIdx.value_or(0);
      }

      if (isOnFinestGrid) {
        result.counts[key]++;
      } else if (result.nForeign++ == 0) {
        result.firstForeign = toString(point.gp) + " of tensor grid " + toString(tg.mi);
      }
    }
  }

  return result;
}

/**
 * Checks the defining property of a deltas sparse grid: Iterating over all grid points of all
 * tensor grids of the deltas sparse grid built from @p genInstr yields every grid point of the
 * (regular) sparse grid built from @p genInstr exactly once, and no other point.
 */
void checkGPPartition(const SGGenInstr& genInstr) {
  const SparseGrid sg(genInstr);
  const DeltasSparseGrid deltasSG(genInstr);
  const std::vector<DataVector> finestNodes = finestNodesPerDim(sg, genInstr);

  const GPCounts regular = countGridPoints(sg, finestNodes);
  const GPCounts deltas = countGridPoints(deltasSG, finestNodes);

  // Sanity check of the test itself: with nested nodes, all regular grid points are found
  BOOST_REQUIRE_EQUAL(regular.nForeign, size_t{0});

  if (deltas.nForeign > 0) {
    BOOST_ERROR(deltas.nForeign << " grid point(s) of the deltas sparse grid are no grid points "
                                << "of the sparse grid, e.g. " << deltas.firstForeign);
  }

  size_t nDuplicates = 0;
  size_t nAdditional = 0;
  size_t nMissing = 0;
  std::string firstDuplicate;
  std::string firstAdditional;
  std::string firstMissing;

  for (const auto& [key, count] : deltas.counts) {
    if (count > 1 && nDuplicates++ == 0) {
      firstDuplicate = toString(key, finestNodes) + " (" + std::to_string(count) + " times)";
    }

    if (!regular.counts.contains(key) && nAdditional++ == 0) {
      firstAdditional = toString(key, finestNodes);
    }
  }

  for (const auto& [key, count] : regular.counts) {
    if (!deltas.counts.contains(key) && nMissing++ == 0) {
      firstMissing = toString(key, finestNodes);
    }
  }

  if (nDuplicates > 0) {
    BOOST_ERROR(nDuplicates << " grid point(s) occur more than once in the deltas sparse grid, "
                            << "e.g. " << firstDuplicate);
  }
  if (nAdditional > 0) {
    BOOST_ERROR(nAdditional << " grid point(s) of the deltas sparse grid are no grid points of "
                            << "the sparse grid, e.g. " << firstAdditional);
  }
  if (nMissing > 0) {
    BOOST_ERROR(nMissing << " grid point(s) of the sparse grid are missing in the deltas sparse "
                         << "grid, e.g. " << firstMissing);
  }

  // Every grid point once <=> the number of iterated points equals the number of unique points
  BOOST_CHECK_EQUAL(deltas.nIterated, regular.counts.size());
}

/**
 * Checks that every grid point of the delta tensor grid of level l has, in every dimension k, the
 * hierarchical level l_k: l_k is the lowest level whose regular nodes contain the coordinate.
 */
void checkHierarchicalLevels(const SGGenInstr& genInstr) {
  const DeltasSparseGrid deltasSG(genInstr);
  const size_t nDim = genInstr.nDim();

  // regularNodesPerLvl[dim][lvl]
  std::vector<std::vector<DataVector>> regularNodesPerLvl(nDim);
  for (size_t dim = 0; dim < nDim; dim++) {
    LvlType maxLvl = 0;
    for (const TensorGridCTData& tg : deltasSG) {
      maxLvl = std::max(maxLvl, tg.mi[dim]);
    }

    for (LvlType lvl = 0; lvl <= maxLvl; lvl++) {
      regularNodesPerLvl[dim].push_back(regularNodes(genInstr, dim, lvl));
    }
  }

  size_t nMismatches = 0;
  std::string firstMismatch;

  for (const TensorGridCTData& tg : deltasSG) {
    for (const auto& point : tg.tensorGrid.gridPointsImmutable()) {
      for (size_t dim = 0; dim < nDim; dim++) {
        LvlType hierarchicalLvl = 0;
        while (hierarchicalLvl < regularNodesPerLvl[dim].size() &&
               !findNode(regularNodesPerLvl[dim][hierarchicalLvl], point.gp[dim])) {
          hierarchicalLvl++;
        }

        if (hierarchicalLvl != tg.mi[dim] && nMismatches++ == 0) {
          firstMismatch = toString(point.gp) + " of tensor grid " + toString(tg.mi) +
                          " has level " + std::to_string(hierarchicalLvl) + " in dimension " +
                          std::to_string(dim);
        }
      }
    }
  }

  if (nMismatches > 0) {
    BOOST_ERROR(nMismatches << " coordinate(s) of grid points do not belong to the level of "
                            << "their tensor grid, e.g. " << firstMismatch);
  }
}

}  // namespace

/***********
Construction
***********/

BOOST_AUTO_TEST_SUITE(DeltasSparseGrid_construction)

BOOST_AUTO_TEST_CASE(Zero_Dims) {
  const CompleteSGGenInstr genInstr(3, 0);
  const DeltasSparseGrid result(genInstr);

  checkSG(result, ExpectedSG(0));
  BOOST_CHECK_EQUAL(result.getMaxTGGPCnt(), size_t{0});
  BOOST_CHECK_EQUAL(result.getMaxTGSumOverGPCntsPerDim(), size_t{0});
}

BOOST_AUTO_TEST_CASE(Handwritten_2D) {
  // Equidistant nodes, doubling growth, boundary on every level: the regular level-l nodes are
  // {0, 1/2^(l+1), ..., 1}; the delta nodes are the new ones.
  CompleteSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);

  const DataVector lvl0{0.0, 0.5, 1.0};
  const DataVector lvl1{0.25, 0.75};
  const DataVector lvl2{0.125, 0.375, 0.625, 0.875};

  ExpectedSG expected(2);
  expected.addTensorGrid(TensorGridCTData{{0, 0}, 0, makeTG({lvl0, lvl0})});    // 3 x 3
  expected.addTensorGrid(TensorGridCTData{{1, 0}, -1, makeTG({lvl1, lvl0})});   // 2 x 3
  expected.addTensorGrid(TensorGridCTData{{0, 1}, -1, makeTG({lvl0, lvl1})});   // 3 x 2
  expected.addTensorGrid(TensorGridCTData{{2, 0}, 1, makeTG({lvl2, lvl0})});    // 4 x 3
  expected.addTensorGrid(TensorGridCTData{{1, 1}, 1, makeTG({lvl1, lvl1})});    // 2 x 2
  expected.addTensorGrid(TensorGridCTData{{0, 2}, 1, makeTG({lvl0, lvl2})});    // 3 x 4

  const DeltasSparseGrid result(genInstr);

  checkSG(result, expected);
  BOOST_CHECK_EQUAL(result.getMaxTGGPCnt(), size_t{12});
  BOOST_CHECK_EQUAL(result.getMaxTGSumOverGPCntsPerDim(), size_t{7});
}

BOOST_AUTO_TEST_CASE(Handwritten_2D_Boundary_Level_Offset_1) {
  // The boundary nodes are added on level 1, so they are delta nodes of level 1
  CompleteSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);

  const DataVector lvl0{0.5};
  const DataVector lvl1{0.0, 0.25, 0.75, 1.0};
  const DataVector lvl2{0.125, 0.375, 0.625, 0.875};

  ExpectedSG expected(2);
  expected.addTensorGrid(TensorGridCTData{{0, 0}, 0, makeTG({lvl0, lvl0})});
  expected.addTensorGrid(TensorGridCTData{{1, 0}, -1, makeTG({lvl1, lvl0})});
  expected.addTensorGrid(TensorGridCTData{{0, 1}, -1, makeTG({lvl0, lvl1})});
  expected.addTensorGrid(TensorGridCTData{{2, 0}, 1, makeTG({lvl2, lvl0})});
  expected.addTensorGrid(TensorGridCTData{{1, 1}, 1, makeTG({lvl1, lvl1})});
  expected.addTensorGrid(TensorGridCTData{{0, 2}, 1, makeTG({lvl0, lvl2})});

  checkSG(DeltasSparseGrid(genInstr), expected);
}

BOOST_AUTO_TEST_CASE(Handwritten_2D_Without_Boundary) {
  CompleteSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1000);

  // Clenshaw-Curtis level 1: (1 -+ cos(pi / 4)) / 2 are new, 1/2 is the level-0 node.
  // Clenshaw-Curtis level 2: every second of the 7 inner nodes, i.e. nodes 0, 2, 4 and 6.
  const double sqrt2 = std::sqrt(2.0);
  const DataVector ccNodesLvl2 = clenshawCurtisNodeGenFunc->genNodesWithoutBoundary(7);
  const DataVector cc0{0.5};
  const DataVector cc1{(2.0 - sqrt2) / 4.0, (2.0 + sqrt2) / 4.0};
  const DataVector cc2{ccNodesLvl2[0], ccNodesLvl2[2], ccNodesLvl2[4], ccNodesLvl2[6]};
  const DataVector eq0{0.5};
  const DataVector eq1{0.25, 0.75};
  const DataVector eq2{0.125, 0.375, 0.625, 0.875};

  ExpectedSG expected(2);
  expected.addTensorGrid(TensorGridCTData{{0, 0}, 0, makeTG({cc0, eq0})});
  expected.addTensorGrid(TensorGridCTData{{1, 0}, -1, makeTG({cc1, eq0})});
  expected.addTensorGrid(TensorGridCTData{{0, 1}, -1, makeTG({cc0, eq1})});
  expected.addTensorGrid(TensorGridCTData{{2, 0}, 1, makeTG({cc2, eq0})});
  expected.addTensorGrid(TensorGridCTData{{1, 1}, 1, makeTG({cc1, eq1})});
  expected.addTensorGrid(TensorGridCTData{{0, 2}, 1, makeTG({cc0, eq2})});

  checkSG(DeltasSparseGrid(genInstr), expected);
}

BOOST_AUTO_TEST_CASE(Complete_Grids) {
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
      for (size_t nDim = 2; nDim <= 4; nDim++) {
        for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
          BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, boundary level offset "
                                                 << boundaryLvlOffset << ", nDim " << nDim
                                                 << ", max. level " << maxLvl) {
            CompleteSGGenInstr genInstr(maxLvl, nDim);
            genInstr.setNodeGenFunc(nodeGenFunc);
            genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
            genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

            const DeltasSparseGrid result(genInstr);

            checkSG(result, genExpectedCompleteDeltasSG(genInstr, maxLvl));
            checkMetaValues(result);
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Mixed_Node_Families_3D) {
  const LvlType maxLvl = 3;

  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(2);

  const DeltasSparseGrid result(genInstr);

  checkSG(result, genExpectedCompleteDeltasSG(genInstr, maxLvl));
  checkMetaValues(result);
}

BOOST_AUTO_TEST_CASE(Random_Complete_Settings) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 2 + randGen.getUniformIndexRN(3);
      const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(5));

      CompleteSGGenInstr genInstr(maxLvl, nDim);
      setRandomNestedSettings(genInstr);

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", maxLvl: " << maxLvl
                                 << ", boundary level offset: "
                                 << genInstr.getBoundaryLevelOffset()) {
        const DeltasSparseGrid result(genInstr);

        checkSG(result, genExpectedCompleteDeltasSG(genInstr, maxLvl));
        checkMetaValues(result);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Contains_Tensor_Grids_With_Zero_Coefficient) {
  // Unlike SparseGrid, a deltas sparse grid needs all levels of the downwards closed set: in 3D
  // with level 4, the 4 multi-indices with |l|_1 <= 1 have a coefficient of 0.
  const LvlType maxLvl = 4;
  const CompleteSGGenInstr genInstr(maxLvl, 3);

  const DeltasSparseGrid result(genInstr);

  BOOST_CHECK_EQUAL(result.nTG(), size_t{35});  // binom(4 + 3, 3)

  size_t nZeroCoeffs = 0;
  for (const TensorGridCTData& tg : result) {
    BOOST_TEST_CONTEXT("Tensor grid " << toString(tg.mi)) {
      BOOST_CHECK_LE(tg.mi.sumOfElems(), maxLvl);
      BOOST_CHECK_EQUAL(tg.coefficient == 0, tg.mi.sumOfElems() <= 1);
      nZeroCoeffs += tg.coefficient == 0 ? 1 : 0;
    }
  }

  BOOST_CHECK_EQUAL(nZeroCoeffs, size_t{4});
  BOOST_CHECK(result.getTensorGrid(LvlMI{0, 0, 0}) != result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{1, 0, 0}) != result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{0, 0, 4}) != result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{0, 0, 5}) == result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{2, 2, 1}) == result.end());
}

BOOST_AUTO_TEST_CASE(Agrees_With_Sparse_Grid_On_Non_Zero_Coefficients) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 2 + randGen.getUniformIndexRN(3);

      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < 3; i++) {
        inputMIs.push_back(randomMI(nDim, 3));
      }
      const LvlMIVec mis(inputMIs);

      CompleteSGGenInstr completeGenInstr(static_cast<LvlType>(randGen.getUniformIndexRN(5)),
                                          nDim);
      setRandomNestedSettings(completeGenInstr);
      MIVecSGGenInstr miVecGenInstr(mis);
      setRandomNestedSettings(miVecGenInstr);

      for (const SGGenInstr* const genInstr :
           {static_cast<const SGGenInstr*>(&completeGenInstr),
            static_cast<const SGGenInstr*>(&miVecGenInstr)}) {
        BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim
                                   << (genInstr == &completeGenInstr ? ", complete" : ", MIVec")) {
          const SparseGrid sg(*genInstr);
          const DeltasSparseGrid deltasSG(*genInstr);

          // The tensor grids of the sparse grid are exactly those with a non-zero coefficient
          size_t nNonZeroCoeffs = 0;
          for (const TensorGridCTData& deltaTG : deltasSG) {
            const auto iter = sg.getTensorGrid(deltaTG.mi);

            BOOST_TEST_CONTEXT("Tensor grid " << toString(deltaTG.mi)) {
              if (deltaTG.coefficient == 0) {
                BOOST_CHECK(iter == sg.end());
              } else {
                nNonZeroCoeffs++;
                BOOST_REQUIRE(iter != sg.end());
                BOOST_CHECK_EQUAL(iter->coefficient, deltaTG.coefficient);
              }
            }
          }

          BOOST_CHECK_EQUAL(nNonZeroCoeffs, sg.nTG());
          BOOST_CHECK_EQUAL(sumOfCoeffs(deltasSG), 1);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Tensor_Grid_Sizes) {
  // Per dimension: the new inner nodes of the level plus the boundary on the boundary level
  // offset; checked against the closed form without generating any nodes
  for (const LvlType boundaryLvlOffset : {0u, 1u, 3u, 1000u}) {
    BOOST_TEST_CONTEXT("Boundary level offset " << boundaryLvlOffset) {
      CompleteSGGenInstr genInstr(5, 3);
      genInstr.setNodeGenFuncs(
          {clenshawCurtisNodeGenFunc, equidistantNodeGenFunc, clenshawCurtisNodeGenFunc});
      genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
      genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

      const DeltasSparseGrid result(genInstr);

      for (const TensorGridCTData& tg : result) {
        BOOST_TEST_CONTEXT("Tensor grid " << toString(tg.mi)) {
          size_t nGP = 1;

          for (size_t dim = 0; dim < 3; dim++) {
            const GPCntType nNodes = expectedDeltaNodeCnt(genInstr, dim, tg.mi[dim]);

            BOOST_CHECK_EQUAL(tg.tensorGrid.getGPCntPerDim()[dim], nNodes);
            nGP *= nNodes;
          }

          BOOST_CHECK_EQUAL(tg.tensorGrid.nGP(), nGP);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(MIVec_Downwards_Closed_2D) {
  // Downwards closed input {(0, 0), (1, 0), (2, 0), (0, 1), (1, 1)}: coefficients
  // c(2, 0) = c(1, 1) = 1, c(1, 0) = -1 and 0 for the other two
  const LvlMIVec mis{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}};
  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);

  const DataVector lvl0{0.0, 0.5, 1.0};
  const DataVector lvl1{0.25, 0.75};
  const DataVector lvl2{0.125, 0.375, 0.625, 0.875};

  ExpectedSG expected(2);
  expected.addTensorGrid(TensorGridCTData{{0, 0}, 0, makeTG({lvl0, lvl0})});
  expected.addTensorGrid(TensorGridCTData{{1, 0}, -1, makeTG({lvl1, lvl0})});
  expected.addTensorGrid(TensorGridCTData{{2, 0}, 1, makeTG({lvl2, lvl0})});
  expected.addTensorGrid(TensorGridCTData{{0, 1}, 0, makeTG({lvl0, lvl1})});
  expected.addTensorGrid(TensorGridCTData{{1, 1}, 1, makeTG({lvl1, lvl1})});

  const DeltasSparseGrid result(genInstr);

  checkSG(result, expected);
  checkSG(result, genExpectedMIVecDeltasSG({{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}}, genInstr));
}

BOOST_AUTO_TEST_CASE(MIVec_Non_Downwards_Closed_Input) {
  // The downwards closure of {(2, 0), (0, 3)} has 6 elements; only (2, 0), (0, 3) and (0, 0)
  // have a non-zero coefficient, but all 6 are part of the deltas sparse grid
  const LvlMIVec mis{{2, 0}, {0, 3}};
  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);

  const DeltasSparseGrid result(genInstr);

  BOOST_CHECK_EQUAL(result.nTG(), size_t{6});
  checkSG(result, genExpectedMIVecDeltasSG({{2, 0}, {0, 3}}, genInstr));
  checkMetaValues(result);
}

BOOST_AUTO_TEST_CASE(MIVec_Random_Input) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 2 + randGen.getUniformIndexRN(3);
      const size_t nInputMIs = 1 + randGen.getUniformIndexRN(5);

      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < nInputMIs; i++) {
        inputMIs.push_back(randomMI(nDim, 3));
      }

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", nInputMIs: " << nInputMIs) {
        const LvlMIVec mis(inputMIs);
        MIVecSGGenInstr genInstr(mis);
        setRandomNestedSettings(genInstr);

        const DeltasSparseGrid result(genInstr);

        checkSG(result, genExpectedMIVecDeltasSG(inputMIs, genInstr));
        checkMetaValues(result);
        BOOST_CHECK_EQUAL(sumOfCoeffs(result), 1);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(MIVec_Matches_Complete) {
  // An MIVecSGGenInstr fed with the multi-indices of a complete sparse grid yields the same
  // deltas sparse grid
  for (size_t nDim = 2; nDim <= 4; nDim++) {
    for (LvlType maxLvl = 0; maxLvl <= 4; maxLvl++) {
      BOOST_TEST_CONTEXT("nDim " << nDim << ", max. level " << maxLvl) {
        CompleteSGGenInstr completeGenInstr(maxLvl, nDim);
        completeGenInstr.setNodeGenFunc(clenshawCurtisNodeGenFunc);
        completeGenInstr.setBoundaryLevelOffset(1);

        const LvlMIVec mis = completeGenInstr.genReducedMIVec();
        MIVecSGGenInstr miVecGenInstr(mis);
        miVecGenInstr.setNodeGenFunc(clenshawCurtisNodeGenFunc);
        miVecGenInstr.setBoundaryLevelOffset(1);

        checkSG(DeltasSparseGrid(miVecGenInstr), DeltasSparseGrid(completeGenInstr));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Nodes_Independent_Of_Domain) {
  // Nodes live on the unit domain; the domain only matters for operators
  CompleteSGGenInstr unitGenInstr(3, 3);
  unitGenInstr.setBoundaryLevelOffset(1);
  CompleteSGGenInstr genInstr = unitGenInstr;
  genInstr.setDomain(HyperCubeArea{{-3.0, 5.0}, {0.5, 0.75}, {10.0, 20.0}});

  checkSG(DeltasSparseGrid(genInstr), DeltasSparseGrid(unitGenInstr));
}

BOOST_AUTO_TEST_CASE(Stores_Copy_Of_Complete_SG_Gen_Instr) {
  const LvlType maxLvl = 3;

  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);
  genInstr.setDomain(HyperCubeArea{{0.0, 1.0}, {-1.0, 1.0}, {2.0, 5.0}});

  const CompleteSGGenInstr originalGenInstr = genInstr;
  const DeltasSparseGrid deltasSG(genInstr);
  const SGGenInstr& storedGenInstr = deltasSG.getGenInstr();

  BOOST_CHECK(&storedGenInstr != &genInstr);
  BOOST_CHECK(dynamic_cast<const CompleteSGGenInstr*>(&storedGenInstr) != nullptr);
  checkSameSettings(storedGenInstr, originalGenInstr);

  // Later changes of the original instruction affect neither the stored instruction nor the grid
  genInstr.setMaxLvl(5);
  genInstr.setNodeGenFunc(clenshawCurtisNodeGenFunc);
  genInstr.setBoundaryLevelOffset(0);
  genInstr.setDomain({-5.0, 5.0});

  checkSameSettings(storedGenInstr, originalGenInstr);
  checkSG(DeltasSparseGrid(storedGenInstr), DeltasSparseGrid(originalGenInstr));
  checkSG(deltasSG, genExpectedCompleteDeltasSG(originalGenInstr, maxLvl));
}

BOOST_AUTO_TEST_CASE(Stores_Copy_Of_MIVec_SG_Gen_Instr) {
  const LvlMIVec mis{{3, 0}, {1, 2}};

  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(2);

  const MIVecSGGenInstr originalGenInstr = genInstr;
  const DeltasSparseGrid deltasSG(genInstr);
  const SGGenInstr& storedGenInstr = deltasSG.getGenInstr();

  BOOST_CHECK(&storedGenInstr != &genInstr);
  BOOST_CHECK(dynamic_cast<const MIVecSGGenInstr*>(&storedGenInstr) != nullptr);
  checkSameSettings(storedGenInstr, originalGenInstr);

  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setBoundaryLevelOffset(0);

  checkSameSettings(storedGenInstr, originalGenInstr);
  checkSG(DeltasSparseGrid(storedGenInstr), DeltasSparseGrid(originalGenInstr));
  checkSG(deltasSG, genExpectedMIVecDeltasSG({{3, 0}, {1, 2}}, originalGenInstr));
}

BOOST_AUTO_TEST_CASE(One_Dimension) {
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 2u, 1000u}) {
      for (LvlType maxLvl = 0; maxLvl <= 6; maxLvl++) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, boundary level offset "
                                               << boundaryLvlOffset << ", max. level "
                                               << maxLvl) {
          CompleteSGGenInstr genInstr(maxLvl, 1);
          genInstr.setNodeGenFunc(nodeGenFunc);
          genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
          genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

          // All levels 0, ..., maxLvl; only the finest one has a non-zero coefficient (1)
          const DeltasSparseGrid result(genInstr);

          checkSG(result, genExpectedCompleteDeltasSG(genInstr, maxLvl));
          checkGPPartition(genInstr);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Non_Nested_Nodes_Throw) {
  CompleteSGGenInstr linearGenInstr(2, 2);
  linearGenInstr.setNodeGenFunc(equidistantNodeGenFunc);
  linearGenInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);

  CompleteSGGenInstr chebyshevGenInstr(2, 2);
  chebyshevGenInstr.setNodeGenFuncs({equidistantNodeGenFunc, firstTypeChebyshevNodeGenFunc});
  chebyshevGenInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);

  BOOST_CHECK_THROW(DeltasSparseGrid{linearGenInstr}, std::invalid_argument);
  BOOST_CHECK_THROW(DeltasSparseGrid{chebyshevGenInstr}, std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

/*******************
Grid point partition
*******************/

BOOST_AUTO_TEST_SUITE(DeltasSparseGrid_grid_point_partition)

BOOST_AUTO_TEST_CASE(Handwritten_2D) {
  // Level 1, boundary everywhere: the sparse grid consists of the tensor grids (1, 0) and
  // (0, 1) with 5 x 3 and 3 x 5 points sharing the 3 x 3 points of (0, 0): 21 unique points.
  // The deltas sparse grid has 3 x 3 + 2 x 3 + 3 x 2 = 21 grid points.
  CompleteSGGenInstr genInstr(1, 2);
  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);

  const DeltasSparseGrid deltasSG(genInstr);

  size_t nGP = 0;
  for (const TensorGridCTData& tg : deltasSG) {
    nGP += tg.tensorGrid.nGP();
  }

  BOOST_CHECK_EQUAL(nGP, size_t{21});
  checkGPPartition(genInstr);
}

BOOST_AUTO_TEST_CASE(Complete_Grids) {
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
      for (size_t nDim = 2; nDim <= 4; nDim++) {
        const LvlType highestMaxLvl = nDim <= 3 ? 5 : 4;

        for (LvlType maxLvl = 0; maxLvl <= highestMaxLvl; maxLvl++) {
          BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, boundary level offset "
                                                 << boundaryLvlOffset << ", nDim " << nDim
                                                 << ", max. level " << maxLvl) {
            CompleteSGGenInstr genInstr(maxLvl, nDim);
            genInstr.setNodeGenFunc(nodeGenFunc);
            genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
            genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

            checkGPPartition(genInstr);
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Mixed_Node_Families) {
  for (const LvlType boundaryLvlOffset : {0u, 1u, 3u}) {
    BOOST_TEST_CONTEXT("Boundary level offset " << boundaryLvlOffset) {
      CompleteSGGenInstr genInstr(5, 3);
      genInstr.setNodeGenFuncs(
          {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
      genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
      genInstr.setBoundaryLevelOffset(boundaryLvlOffset);
      genInstr.setDomain(HyperCubeArea{{-1.0, 1.0}, {0.0, 10.0}, {3.0, 4.0}});

      checkGPPartition(genInstr);
    }
  }
}

BOOST_AUTO_TEST_CASE(Anisotropic_MIVec) {
  const LvlMIVec mis{{5, 0, 0}, {0, 1, 2}, {1, 1, 1}, {0, 3, 0}};

  for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
    BOOST_TEST_CONTEXT("Boundary level offset " << boundaryLvlOffset) {
      MIVecSGGenInstr genInstr(mis);
      genInstr.setNodeGenFuncs(
          {clenshawCurtisNodeGenFunc, equidistantNodeGenFunc, clenshawCurtisNodeGenFunc});
      genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
      genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

      checkGPPartition(genInstr);
    }
  }
}

BOOST_AUTO_TEST_CASE(Single_Full_Grid) {
  // A single input multi-index gives one full grid; its deltas partition it
  const LvlMIVec mis{{3, 2}};

  for (const LvlType boundaryLvlOffset : {0u, 1u, 3u, 1000u}) {
    BOOST_TEST_CONTEXT("Boundary level offset " << boundaryLvlOffset) {
      MIVecSGGenInstr genInstr(mis);
      genInstr.setNodeGenFunc(equidistantNodeGenFunc);
      genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
      genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

      BOOST_CHECK_EQUAL(SparseGrid(genInstr).nTG(), size_t{1});
      BOOST_CHECK_EQUAL(DeltasSparseGrid(genInstr).nTG(), size_t{12});
      checkGPPartition(genInstr);
    }
  }
}

BOOST_AUTO_TEST_CASE(Random_Settings) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 2 + randGen.getUniformIndexRN(3);
      const size_t nInputMIs = 1 + randGen.getUniformIndexRN(5);

      // Keeps the full grids small enough to iterate over all of their grid points
      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < nInputMIs; i++) {
        inputMIs.push_back(randomMI(nDim, nDim <= 3 ? 3 : 2));
      }
      const LvlMIVec mis(inputMIs);

      CompleteSGGenInstr completeGenInstr(static_cast<LvlType>(randGen.getUniformIndexRN(5)),
                                          nDim);
      setRandomNestedSettings(completeGenInstr);
      MIVecSGGenInstr miVecGenInstr(mis);
      setRandomNestedSettings(miVecGenInstr);

      for (const SGGenInstr* const genInstr :
           {static_cast<const SGGenInstr*>(&completeGenInstr),
            static_cast<const SGGenInstr*>(&miVecGenInstr)}) {
        BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim
                                   << (genInstr == &completeGenInstr ? ", complete" : ", MIVec")
                                   << ", boundary level offset "
                                   << genInstr->getBoundaryLevelOffset()) {
          checkGPPartition(*genInstr);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Grid_Points_Have_The_Level_Of_Their_Tensor_Grid) {
  for (NodeGenFunc* const nodeGenFunc : {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc}) {
    for (const LvlType boundaryLvlOffset : {0u, 1u, 2u, 1000u}) {
      for (size_t nDim = 2; nDim <= 3; nDim++) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, boundary level offset "
                                               << boundaryLvlOffset << ", nDim " << nDim) {
          CompleteSGGenInstr genInstr(4, nDim);
          genInstr.setNodeGenFunc(nodeGenFunc);
          genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
          genInstr.setBoundaryLevelOffset(boundaryLvlOffset);

          checkHierarchicalLevels(genInstr);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
