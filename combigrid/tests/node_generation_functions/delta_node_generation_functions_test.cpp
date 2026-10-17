// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file delta_node_generation_functions_test.cpp
 * @brief Boost.Test cases for the delta-node functionality of the node generation functions:
 * @c tools::getDeltaNodeCnt, @c NodeGenFunc::genDeltaNodes* (the nodes a level adds to the
 * previous level), @c NodeGenFunc::isNested (and @c SGGenInstr::areNodesOfAllDimsNested) and the
 * node index maps @c getNodeIdxMapBetweenLvls / @c getNodeIdxMapFromDeltaTGLvlToRegularLvl.
 *
 * Expected delta nodes are computed only from @c NodeGenFunc::genNodesWithoutBoundary as the set
 * difference of two consecutive levels, independently of the delta node generation code.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/functions/node_generation_functions/equidistant_node_generation_function_utilities.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/deltas_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static NodeGenFunc* const equidistantNodeGenFunc = getEquidistantNodeGenFunc();
static NodeGenFunc* const clenshawCurtisNodeGenFunc = getClenshawCurtisNodeGenFunc();
static NodeGenFunc* const firstTypeChebyshevNodeGenFunc = getFirstTypeChebyshevNodeGenFunc();

namespace {

constexpr double EPS = 1e-12;

/// Highest level tested with the doubling growth function (2^(MAX_LVL + 1) - 1 inner nodes).
constexpr LvlType MAX_LVL = 8;

/// Growth function that is unknown to every node generation function.
GPCntType triplingLvl2GPCntFunction(const LvlType lvl) {
  GPCntType result = 1;
  for (LvlType i = 0; i < lvl; i++) result *= 3;
  return result;
}

/// The node generation functions whose nodes are nested w.r.t. the doubling growth function.
const std::vector<NodeGenFunc*>& nestedNodeGenFuncs() {
  static const std::vector<NodeGenFunc*> funcs{equidistantNodeGenFunc, clenshawCurtisNodeGenFunc};
  return funcs;
}

std::string nameOf(const NodeGenFunc* nodeGenFunc) {
  if (nodeGenFunc == equidistantNodeGenFunc) return "equidistant";
  if (nodeGenFunc == clenshawCurtisNodeGenFunc) return "Clenshaw-Curtis";
  if (nodeGenFunc == firstTypeChebyshevNodeGenFunc) return "first type Chebyshev";
  return "unknown";
}

/// Inner nodes of level @p lvl, i.e. the nodes of a regular tensor grid without boundary.
DataVector innerNodes(const NodeGenFunc* nodeGenFunc, const Lvl2GPCntFunc lvl2GPCntFunc,
                      const LvlType lvl) {
  return nodeGenFunc->genNodesWithoutBoundary(lvl2GPCntFunc(lvl));
}

bool containsNode(const DataVector& nodes, const double node) {
  for (const double other : nodes) {
    if (std::abs(other - node) <= EPS) return true;
  }
  return false;
}

/// The inner nodes of level @p lvl that are no inner nodes of level @p lvl - 1 (in order).
DataVector expectedInnerDeltaNodes(const NodeGenFunc* nodeGenFunc,
                                   const Lvl2GPCntFunc lvl2GPCntFunc, const LvlType lvl) {
  const DataVector nodes = innerNodes(nodeGenFunc, lvl2GPCntFunc, lvl);

  if (lvl == 0) return nodes;

  const DataVector coarserNodes = innerNodes(nodeGenFunc, lvl2GPCntFunc, lvl - 1);
  DataVector result;

  for (const double node : nodes) {
    if (!containsNode(coarserNodes, node)) result.push_back(node);
  }

  return result;
}

/// @p nodes framed by the boundary nodes 0 and 1.
DataVector withBoundary(const DataVector& nodes) {
  DataVector result(nodes.size() + 2);
  result[0] = 0.0;
  for (size_t i = 0; i < nodes.size(); i++) result[i + 1] = nodes[i];
  result[nodes.size() + 1] = 1.0;
  return result;
}

/// Checks that @p result has the size of @p expected and the same entries (up to EPS).
void checkNodes(const DataVector& result, const DataVector& expected) {
  BOOST_CHECK_EQUAL(result.size(), expected.size());

  for (size_t i = 0; i < std::min(result.size(), expected.size()); i++) {
    BOOST_TEST_CONTEXT("Node " << i) { BOOST_CHECK_SMALL(result[i] - expected[i], EPS); }
  }
}

}  // namespace

/*********************
Number of delta nodes
*********************/

BOOST_AUTO_TEST_SUITE(NodeGenFunc_delta_node_count)

BOOST_AUTO_TEST_CASE(Doubling_Growth) {
  // Level l has 2^(l+1) - 1 inner nodes, so it adds 2^l new nodes to level l - 1
  for (LvlType lvl = 0; lvl <= 15; lvl++) {
    BOOST_TEST_CONTEXT("Level " << lvl) {
      const GPCntType expected = GPCntType{1} << lvl;

      BOOST_CHECK_EQUAL(tools::getDeltaNodeCnt(lvl, doublingLvl2GPCntFunction, false), expected);
      BOOST_CHECK_EQUAL(tools::getDeltaNodeCnt(lvl, doublingLvl2GPCntFunction, true),
                        expected + 2);
    }
  }
}

BOOST_AUTO_TEST_CASE(Difference_Of_Consecutive_Node_Counts) {
  for (LvlType lvl = 0; lvl <= 15; lvl++) {
    BOOST_TEST_CONTEXT("Level " << lvl) {
      const GPCntType nNodesOnLvl = doublingLvl2GPCntFunction(lvl);
      const GPCntType nNodesOnPrevLvl = lvl == 0 ? 0 : doublingLvl2GPCntFunction(lvl - 1);

      BOOST_CHECK_EQUAL(tools::getDeltaNodeCnt(lvl, doublingLvl2GPCntFunction, false),
                        nNodesOnLvl - nNodesOnPrevLvl);
    }
  }
}

BOOST_AUTO_TEST_CASE(Number_Of_New_Nodes) {
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    for (LvlType lvl = 0; lvl <= MAX_LVL; lvl++) {
      BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, level " << lvl) {
        const size_t nNewNodes =
            expectedInnerDeltaNodes(nodeGenFunc, doublingLvl2GPCntFunction, lvl).size();

        BOOST_CHECK_EQUAL(tools::getDeltaNodeCnt(lvl, doublingLvl2GPCntFunction, false),
                          nNewNodes);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

/**********
Delta nodes
**********/

BOOST_AUTO_TEST_SUITE(NodeGenFunc_delta_nodes)

BOOST_AUTO_TEST_CASE(Equidistant_Handwritten) {
  const NodeGenFunc* const ngf = equidistantNodeGenFunc;
  const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;

  checkNodes(ngf->genDeltaNodesWithoutBoundary(0, f), DataVector{0.5});
  checkNodes(ngf->genDeltaNodesWithoutBoundary(1, f), DataVector{0.25, 0.75});
  checkNodes(ngf->genDeltaNodesWithoutBoundary(2, f), DataVector{0.125, 0.375, 0.625, 0.875});

  checkNodes(ngf->genDeltaNodesWithBoundary(0, f), DataVector{0.0, 0.5, 1.0});
  checkNodes(ngf->genDeltaNodesWithBoundary(1, f), DataVector{0.0, 0.25, 0.75, 1.0});
}

BOOST_AUTO_TEST_CASE(Clenshaw_Curtis_Handwritten) {
  // Level 1: the inner nodes are (1 - cos(k pi / 4)) / 2 for k = 1, 2, 3; k = 2 is the midpoint
  const NodeGenFunc* const ngf = clenshawCurtisNodeGenFunc;
  const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;
  const double sqrt2 = std::sqrt(2.0);

  checkNodes(ngf->genDeltaNodesWithoutBoundary(0, f), DataVector{0.5});
  checkNodes(ngf->genDeltaNodesWithoutBoundary(1, f),
             DataVector{(2.0 - sqrt2) / 4.0, (2.0 + sqrt2) / 4.0});
  checkNodes(ngf->genDeltaNodesWithBoundary(1, f),
             DataVector{0.0, (2.0 - sqrt2) / 4.0, (2.0 + sqrt2) / 4.0, 1.0});
}

BOOST_AUTO_TEST_CASE(Without_Boundary_Are_The_New_Inner_Nodes) {
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    for (LvlType lvl = 0; lvl <= MAX_LVL; lvl++) {
      BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, level " << lvl) {
        checkNodes(nodeGenFunc->genDeltaNodesWithoutBoundary(lvl, doublingLvl2GPCntFunction),
                   expectedInnerDeltaNodes(nodeGenFunc, doublingLvl2GPCntFunction, lvl));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(With_Boundary_Are_Framed_By_Zero_And_One) {
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    for (LvlType lvl = 0; lvl <= MAX_LVL; lvl++) {
      BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, level " << lvl) {
        checkNodes(
            nodeGenFunc->genDeltaNodesWithBoundary(lvl, doublingLvl2GPCntFunction),
            withBoundary(expectedInnerDeltaNodes(nodeGenFunc, doublingLvl2GPCntFunction, lvl)));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Boundary_Flag_Selects_Variant) {
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    for (LvlType lvl = 0; lvl <= MAX_LVL; lvl++) {
      BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, level " << lvl) {
        const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;

        checkNodes(nodeGenFunc->genDeltaNodes(lvl, f, false),
                   nodeGenFunc->genDeltaNodesWithoutBoundary(lvl, f));
        checkNodes(nodeGenFunc->genDeltaNodes(lvl, f, true),
                   nodeGenFunc->genDeltaNodesWithBoundary(lvl, f));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(In_Place_Variant_Writes_Only_Into_The_Span) {
  constexpr double SENTINEL = -1.0;

  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    for (LvlType lvl = 0; lvl <= MAX_LVL; lvl++) {
      for (const bool includeBoundary : {false, true}) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, level " << lvl << ", boundary: "
                                               << includeBoundary) {
          const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;
          const size_t nNodes = tools::getDeltaNodeCnt(lvl, f, includeBoundary);

          // The span is surrounded by one sentinel on each side
          std::vector<double> buffer(nNodes + 2, SENTINEL);
          nodeGenFunc->genDeltaNodes(lvl, f, std::span(buffer).subspan(1, nNodes),
                                     includeBoundary);

          BOOST_CHECK_EQUAL(buffer.front(), SENTINEL);
          BOOST_CHECK_EQUAL(buffer.back(), SENTINEL);
          checkNodes(DataVector(std::vector<double>(buffer.begin() + 1, buffer.end() - 1)),
                     nodeGenFunc->genDeltaNodes(lvl, f, includeBoundary));
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Delta_Nodes_Of_All_Levels_Partition_The_Finest_Level) {
  // 1D analogue of the grid point partition of a deltas sparse grid: The delta nodes of the
  // levels 0, ..., L (with the boundary on the boundary level offset) contain every node of the
  // regular level-L nodes exactly once and nothing else.
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    for (LvlType maxLvl = 0; maxLvl <= MAX_LVL; maxLvl++) {
      for (LvlType boundaryLvlOffset = 0; boundaryLvlOffset <= maxLvl + 1; boundaryLvlOffset++) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, max. level " << maxLvl
                                               << ", boundary level offset "
                                               << boundaryLvlOffset) {
          const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;

          DataVector finestNodes = innerNodes(nodeGenFunc, f, maxLvl);
          if (maxLvl >= boundaryLvlOffset) finestNodes = withBoundary(finestNodes);

          std::vector<size_t> occurrences(finestNodes.size(), 0);
          size_t nForeignNodes = 0;

          for (LvlType lvl = 0; lvl <= maxLvl; lvl++) {
            for (const double node : nodeGenFunc->genDeltaNodes(lvl, f, lvl == boundaryLvlOffset)) {
              bool found = false;

              for (size_t i = 0; i < finestNodes.size(); i++) {
                if (std::abs(finestNodes[i] - node) <= EPS) {
                  occurrences[i]++;
                  found = true;
                }
              }

              if (!found) nForeignNodes++;
            }
          }

          BOOST_CHECK_EQUAL(nForeignNodes, size_t{0});

          for (size_t i = 0; i < finestNodes.size(); i++) {
            BOOST_TEST_CONTEXT("Node " << finestNodes[i]) {
              BOOST_CHECK_EQUAL(occurrences[i], size_t{1});
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Non_Nested_Nodes_Throw) {
  const std::vector<std::pair<NodeGenFunc*, Lvl2GPCntFunc>> nonNestedSettings{
      {equidistantNodeGenFunc, linearLvl2GPCntFunction},
      {clenshawCurtisNodeGenFunc, linearLvl2GPCntFunction},
      {firstTypeChebyshevNodeGenFunc, linearLvl2GPCntFunction},
      {firstTypeChebyshevNodeGenFunc, doublingLvl2GPCntFunction}};

  for (const auto& [nodeGenFunc, f] : nonNestedSettings) {
    for (LvlType lvl = 0; lvl <= 3; lvl++) {
      BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, "
                                             << (f == linearLvl2GPCntFunction ? "linear"
                                                                              : "doubling")
                                             << " growth, level " << lvl) {
        BOOST_CHECK_THROW(nodeGenFunc->genDeltaNodesWithoutBoundary(lvl, f),
                          std::invalid_argument);
        BOOST_CHECK_THROW(nodeGenFunc->genDeltaNodesWithBoundary(lvl, f), std::invalid_argument);
        BOOST_CHECK_THROW(nodeGenFunc->genDeltaNodes(lvl, f, false), std::invalid_argument);
        BOOST_CHECK_THROW(nodeGenFunc->genDeltaNodes(lvl, f, true), std::invalid_argument);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Unknown_Growth_Function_Throws) {
  for (const NodeGenFunc* const nodeGenFunc :
       {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc}) {
    BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes") {
      BOOST_CHECK_THROW(nodeGenFunc->genDeltaNodesWithoutBoundary(2, triplingLvl2GPCntFunction),
                        sgpp::base::not_implemented_exception);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

/*********
Nestedness
*********/

BOOST_AUTO_TEST_SUITE(NodeGenFunc_nestedness)

BOOST_AUTO_TEST_CASE(Known_Settings) {
  BOOST_CHECK(equidistantNodeGenFunc->isNested(doublingLvl2GPCntFunction));
  BOOST_CHECK(!equidistantNodeGenFunc->isNested(linearLvl2GPCntFunction));

  BOOST_CHECK(clenshawCurtisNodeGenFunc->isNested(doublingLvl2GPCntFunction));
  BOOST_CHECK(!clenshawCurtisNodeGenFunc->isNested(linearLvl2GPCntFunction));

  BOOST_CHECK(!firstTypeChebyshevNodeGenFunc->isNested(doublingLvl2GPCntFunction));
  BOOST_CHECK(!firstTypeChebyshevNodeGenFunc->isNested(linearLvl2GPCntFunction));
}

BOOST_AUTO_TEST_CASE(Agrees_With_Brute_Force) {
  // Nested: every node of every level is also a node of the next level
  for (NodeGenFunc* const nodeGenFunc :
       {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc}) {
    for (const Lvl2GPCntFunc f : {linearLvl2GPCntFunction, doublingLvl2GPCntFunction}) {
      BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, "
                                             << (f == linearLvl2GPCntFunction ? "linear"
                                                                              : "doubling")
                                             << " growth") {
        bool nested = true;

        for (LvlType lvl = 0; lvl < 6; lvl++) {
          const DataVector finerNodes = innerNodes(nodeGenFunc, f, lvl + 1);

          for (const double node : innerNodes(nodeGenFunc, f, lvl)) {
            nested = nested && containsNode(finerNodes, node);
          }
        }

        BOOST_CHECK_EQUAL(nodeGenFunc->isNested(f), nested);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Unknown_Growth_Function_Throws) {
  for (const NodeGenFunc* const nodeGenFunc :
       {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc}) {
    BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes") {
      BOOST_CHECK_THROW(nodeGenFunc->isNested(triplingLvl2GPCntFunction),
                        sgpp::base::not_implemented_exception);
    }
  }
}

BOOST_AUTO_TEST_CASE(SGGenInstr_All_Dims) {
  // Nested iff the nodes of every dimension are nested w.r.t. the growth function of that dim
  CompleteSGGenInstr genInstr(2, 3);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  BOOST_CHECK(genInstr.areNodesOfAllDimsNested());

  genInstr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 2);
  BOOST_CHECK(!genInstr.areNodesOfAllDimsNested());

  genInstr.setLvl2GPCntFuncForDim(doublingLvl2GPCntFunction, 2);
  genInstr.setNodeGenFuncForDim(firstTypeChebyshevNodeGenFunc, 0);
  BOOST_CHECK(!genInstr.areNodesOfAllDimsNested());

  genInstr.setNodeGenFuncForDim(equidistantNodeGenFunc, 0);
  genInstr.setLvl2GPCntFuncForDim(triplingLvl2GPCntFunction, 1);
  BOOST_CHECK_THROW(genInstr.areNodesOfAllDimsNested(), sgpp::base::not_implemented_exception);

  // Without dimensions, there is nothing that could be non-nested
  BOOST_CHECK(CompleteSGGenInstr(2, 0).areNodesOfAllDimsNested());
}

BOOST_AUTO_TEST_SUITE_END()

/****************
Node index maps
****************/

BOOST_AUTO_TEST_SUITE(NodeGenFunc_node_index_maps)

BOOST_AUTO_TEST_CASE(Handwritten_Values) {
  const NodeIdxLvlMapFunc mapBetweenLvls =
      tools::equidistant_node_gen_function::mapNodeIdxBetweenLvlsForDoublingLvl2GPCntFunction;
  const NodeIdxLvlMapFunc mapFromDeltaLvl = tools::equidistant_node_gen_function::
      mapNodeIdxFromDeltaTGLvlToRegularLvlForDoublingLvl2GPCntFunction;

  // Level 0: {1/2}, level 1: {1/4, 2/4, 3/4}, level 3: {1/16, ..., 15/16} (inner indices)
  BOOST_CHECK_EQUAL(mapBetweenLvls(0, 0, 0), 0u);
  BOOST_CHECK_EQUAL(mapBetweenLvls(0, 0, 1), 1u);
  BOOST_CHECK_EQUAL(mapBetweenLvls(0, 0, 3), 7u);
  BOOST_CHECK_EQUAL(mapBetweenLvls(2, 1, 1), 2u);
  BOOST_CHECK_EQUAL(mapBetweenLvls(2, 1, 3), 11u);

  // Delta level 1: {1/4, 3/4}, delta level 2: {1/8, 3/8, 5/8, 7/8}
  BOOST_CHECK_EQUAL(mapFromDeltaLvl(0, 0, 0), 0u);
  BOOST_CHECK_EQUAL(mapFromDeltaLvl(1, 1, 1), 2u);
  BOOST_CHECK_EQUAL(mapFromDeltaLvl(1, 1, 3), 11u);
  BOOST_CHECK_EQUAL(mapFromDeltaLvl(3, 2, 2), 6u);
  BOOST_CHECK_EQUAL(mapFromDeltaLvl(3, 2, 4), 27u);

  // Both nested node families share the index maps
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes") {
      BOOST_CHECK(nodeGenFunc->getNodeIdxMapBetweenLvls(doublingLvl2GPCntFunction) ==
                  mapBetweenLvls);
      BOOST_CHECK(nodeGenFunc->getNodeIdxMapFromDeltaTGLvlToRegularLvl(
                      doublingLvl2GPCntFunction) == mapFromDeltaLvl);
    }
  }
}

BOOST_AUTO_TEST_CASE(Map_Between_Levels_Preserves_The_Node) {
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;
    const NodeIdxLvlMapFunc map = nodeGenFunc->getNodeIdxMapBetweenLvls(f);

    for (LvlType originLvl = 0; originLvl <= 6; originLvl++) {
      const DataVector originNodes = innerNodes(nodeGenFunc, f, originLvl);

      for (LvlType targetLvl = originLvl; targetLvl <= MAX_LVL; targetLvl++) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, level " << originLvl << " -> "
                                               << targetLvl) {
          const DataVector targetNodes = innerNodes(nodeGenFunc, f, targetLvl);

          for (GPCntType i = 0; i < originNodes.size(); i++) {
            const GPCntType mappedIdx = map(i, originLvl, targetLvl);

            BOOST_TEST_CONTEXT("Node index " << i << " -> " << mappedIdx) {
              BOOST_REQUIRE_LT(mappedIdx, targetNodes.size());
              BOOST_CHECK_SMALL(targetNodes[mappedIdx] - originNodes[i], EPS);
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Map_Between_Levels_Is_Identity_On_The_Same_Level_And_Transitive) {
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;
    const NodeIdxLvlMapFunc map = nodeGenFunc->getNodeIdxMapBetweenLvls(f);

    for (LvlType lvl1 = 0; lvl1 <= 5; lvl1++) {
      for (GPCntType i = 0; i < f(lvl1); i++) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, level " << lvl1 << ", node " << i) {
          BOOST_CHECK_EQUAL(map(i, lvl1, lvl1), i);

          for (LvlType lvl2 = lvl1; lvl2 <= 6; lvl2++) {
            for (LvlType lvl3 = lvl2; lvl3 <= 7; lvl3++) {
              BOOST_CHECK_EQUAL(map(map(i, lvl1, lvl2), lvl2, lvl3), map(i, lvl1, lvl3));
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Map_From_Delta_Level_Preserves_The_Node) {
  for (const NodeGenFunc* const nodeGenFunc : nestedNodeGenFuncs()) {
    const Lvl2GPCntFunc f = doublingLvl2GPCntFunction;
    const NodeIdxLvlMapFunc map = nodeGenFunc->getNodeIdxMapFromDeltaTGLvlToRegularLvl(f);

    for (LvlType deltaLvl = 0; deltaLvl <= 6; deltaLvl++) {
      const DataVector deltaNodes = expectedInnerDeltaNodes(nodeGenFunc, f, deltaLvl);

      for (LvlType targetLvl = deltaLvl; targetLvl <= MAX_LVL; targetLvl++) {
        BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, delta level " << deltaLvl
                                               << " -> level " << targetLvl) {
          const DataVector targetNodes = innerNodes(nodeGenFunc, f, targetLvl);

          for (GPCntType i = 0; i < deltaNodes.size(); i++) {
            const GPCntType mappedIdx = map(i, deltaLvl, targetLvl);

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

BOOST_AUTO_TEST_CASE(Non_Nested_Nodes_Throw) {
  const std::vector<std::pair<NodeGenFunc*, Lvl2GPCntFunc>> nonNestedSettings{
      {equidistantNodeGenFunc, linearLvl2GPCntFunction},
      {clenshawCurtisNodeGenFunc, linearLvl2GPCntFunction},
      {firstTypeChebyshevNodeGenFunc, linearLvl2GPCntFunction},
      {firstTypeChebyshevNodeGenFunc, doublingLvl2GPCntFunction}};

  for (const auto& [nodeGenFunc, f] : nonNestedSettings) {
    BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes, "
                                           << (f == linearLvl2GPCntFunction ? "linear"
                                                                            : "doubling")
                                           << " growth") {
      BOOST_CHECK_THROW(nodeGenFunc->getNodeIdxMapBetweenLvls(f), std::invalid_argument);
      BOOST_CHECK_THROW(nodeGenFunc->getNodeIdxMapFromDeltaTGLvlToRegularLvl(f),
                        std::invalid_argument);
    }
  }
}

BOOST_AUTO_TEST_CASE(Unknown_Growth_Function_Throws) {
  for (const NodeGenFunc* const nodeGenFunc :
       {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc}) {
    BOOST_TEST_CONTEXT(nameOf(nodeGenFunc) << " nodes") {
      BOOST_CHECK_THROW(nodeGenFunc->getNodeIdxMapBetweenLvls(triplingLvl2GPCntFunction),
                        sgpp::base::not_implemented_exception);
      BOOST_CHECK_THROW(
          nodeGenFunc->getNodeIdxMapFromDeltaTGLvlToRegularLvl(triplingLvl2GPCntFunction),
          sgpp::base::not_implemented_exception);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
