// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_construction_test.cpp
 * @brief Boost.Test cases verifying the construction of @c SparseGrid objects from
 * @c CompleteSGGenInstr and @c MIVecSGGenInstr instructions (number of tensor grids,
 * level multi-indices, combination coefficients, node positions, boundary handling,
 * cached meta values) and the generation instruction stored in the grid.
 *
 * The expected grids are assembled by hand in @c ExpectedSG, a minimal subclass of
 * @c SparseGridBase that makes its protected constructor and @c addTensorGrid public. Expected node lists
 * are built only from @c NodeGenFunc::genNodesWithoutBoundary, independently of the
 * library's sparse-grid generation code.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <functional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
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
#include <string>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();
static NodeGenFunc* equidistantNodeGenFunc = getEquidistantNodeGenFunc();
static NodeGenFunc* clenshawCurtisNodeGenFunc = getClenshawCurtisNodeGenFunc();
static NodeGenFunc* firstTypeChebyshevNodeGenFunc = getFirstTypeChebyshevNodeGenFunc();

namespace {

/// Concrete sparse grid with a public constructor and modifier, used to assemble expected
/// grids by hand.
class ExpectedSG : public SparseGridBase {
 public:
  explicit ExpectedSG(const size_t nDim) : SparseGridBase(nDim) {}

  using SparseGridBase::addTensorGrid;
};

/// The @p nInnerNodes inner nodes of @p nodeGenFunc, framed by 0 and 1 if @p withBoundary.
sgpp::base::DataVector expectedNodes(const NodeGenFunc* nodeGenFunc, const GPCntType nInnerNodes,
                                     const bool withBoundary) {
  const sgpp::base::DataVector innerNodes = nodeGenFunc->genNodesWithoutBoundary(nInnerNodes);

  if (!withBoundary) {
    return innerNodes;
  }

  sgpp::base::DataVector nodes(nInnerNodes + 2);
  nodes[0] = 0.0;
  std::copy(innerNodes.begin(), innerNodes.end(), nodes.begin() + 1);
  nodes[nInnerNodes + 1] = 1.0;

  return nodes;
}

/// Tensor grid with the given per-dimension node lists.
TensorGrid makeTG(const std::vector<sgpp::base::DataVector>& nodesPerDim) {
  GPMI gpCntPerDim(nodesPerDim.size());
  size_t nNodes = 0;

  for (size_t dim = 0; dim < nodesPerDim.size(); dim++) {
    gpCntPerDim[dim] = static_cast<GPCntType>(nodesPerDim[dim].size());
    nNodes += nodesPerDim[dim].size();
  }

  sgpp::base::DataVector concatenatedNodes(nNodes);
  auto out = concatenatedNodes.begin();
  for (const sgpp::base::DataVector& nodes : nodesPerDim) {
    out = std::copy(nodes.begin(), nodes.end(), out);
  }

  return TensorGrid(gpCntPerDim, concatenatedNodes);
}

/**
 * Tensor grid of level @p mi as it should be generated from @p genInstr: in dimension k,
 * Lvl2GPCntFunc_k(l_k) inner nodes, plus the boundary nodes 0 and 1 iff
 * l_k >= boundary level offset.
 */
TensorGrid expectedTG(const LvlMI& mi, const SGGenInstr& genInstr) {
  std::vector<sgpp::base::DataVector> nodesPerDim;

  for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
    nodesPerDim.push_back(expectedNodes(genInstr.getNodeGenFuncForDim(dim),
                                        genInstr.getLvl2GPCntFuncForDim(dim)(mi[dim]),
                                        mi[dim] >= genInstr.getBoundaryLevelOffset()));
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
 * Expected complete sparse grid of level @p maxLvl: every l with |l|_1 <= maxLvl whose
 * closed-form coefficient (-1)^q * binom(d-1, q), q = maxLvl - |l|_1, is non-zero.
 */
ExpectedSG genExpectedCompleteSG(const SGGenInstr& genInstr, const LvlType maxLvl) {
  const size_t nDim = genInstr.nDim();
  ExpectedSG expected(nDim);

  forEachMIInBox(LvlMI(nDim, maxLvl), [&](const LvlMI& mi) {
    const LvlType lvlSum = mi.sumOfElems();
    if (lvlSum > maxLvl) return;

    const CTCoeffType q = static_cast<CTCoeffType>(maxLvl - lvlSum);
    const CTCoeffType coeff =
        (q % 2 == 0 ? 1 : -1) * tools::binomial(static_cast<CTCoeffType>(nDim) - 1, q);

    if (coeff != 0) {
      expected.addTensorGrid({mi, coeff, expectedTG(mi, genInstr)});
    }
  });

  return expected;
}

/**
 * Expected sparse grid of an @c MIVecSGGenInstr with input @p inputMIs, by brute force: the
 * downwards closure I of the input is enumerated in its bounding box and every coefficient is
 * computed by the inclusion-exclusion formula c_l = sum over z in {0,1}^d with l + z in I of
 * (-1)^|z|_1. Tensor grids with coefficient 0 are omitted.
 */
ExpectedSG genExpectedMIVecSG(const std::vector<LvlMI>& inputMIs, const SGGenInstr& genInstr) {
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
    for (size_t dim = 0; dim < nDim; dim++) upper[dim] = std::max(upper[dim], inputMI[dim]);
  }

  ExpectedSG expected(nDim);

  forEachMIInBox(upper, [&](const LvlMI& mi) {
    if (!isInClosure(mi)) return;

    CTCoeffType coeff = 0;
    for (size_t z = 0; z < (size_t{1} << nDim); z++) {
      LvlMI neighbor = mi;
      CTCoeffType sign = 1;

      for (size_t dim = 0; dim < nDim; dim++) {
        if ((z >> dim) & 1) {
          neighbor[dim]++;
          sign = -sign;
        }
      }

      if (isInClosure(neighbor)) coeff += sign;
    }

    if (coeff != 0) {
      expected.addTensorGrid({mi, coeff, expectedTG(mi, genInstr)});
    }
  });

  return expected;
}

/// Assigns random node generation functions, growth functions and a random boundary offset.
void randomizeConfig(SGGenInstr& genInstr) {
  const std::vector<NodeGenFunc*> nodeGenFuncs{equidistantNodeGenFunc, clenshawCurtisNodeGenFunc,
                                               firstTypeChebyshevNodeGenFunc};
  const std::vector<Lvl2GPCntFunc> lvl2GPCntFuncs{linearLvl2GPCntFunction,
                                                  doublingLvl2GPCntFunction};

  for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
    genInstr.setNodeGenFuncForDim(nodeGenFuncs[randGen.getUniformIndexRN(nodeGenFuncs.size())],
                                  dim);
    genInstr.setLvl2GPCntFuncForDim(
        lvl2GPCntFuncs[randGen.getUniformIndexRN(lvl2GPCntFuncs.size())], dim);
  }

  genInstr.setBoundaryLevelOffset(static_cast<LvlType>(randGen.getUniformIndexRN(4)));
}

/// Human-readable form of @p mi for test contexts, e.g. "(1, 0, 2)".
std::string toString(const LvlMI& mi) {
  std::string result = "(";
  for (size_t dim = 0; dim < mi.size(); dim++) {
    result += (dim > 0 ? ", " : "") + std::to_string(mi[dim]);
  }
  return result + ")";
}

CTCoeffType sumOfCoeffs(const SparseGridBase& sg) {
  CTCoeffType sum = 0;
  for (const TensorGridCTData& tgData : sg) sum += tgData.coefficient;
  return sum;
}

void checkSameConfig(const SGGenInstr& result, const SGGenInstr& expected) {
  BOOST_CHECK_EQUAL(result.nDim(), expected.nDim());
  BOOST_CHECK_EQUAL(result.getBoundaryLevelOffset(), expected.getBoundaryLevelOffset());
  BOOST_CHECK(result.getDomain() == expected.getDomain());
  BOOST_CHECK(result.getNodeGenFuncs() == expected.getNodeGenFuncs());
  BOOST_CHECK(result.getLvl2GPCntFuncs() == expected.getLvl2GPCntFuncs());
}

}  // namespace

BOOST_AUTO_TEST_SUITE(SparseGrid_Constructor)

BOOST_AUTO_TEST_CASE(Zero_Dims) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(10));

    const CompleteSGGenInstr genInstr(maxLvl, 0);

    const ExpectedSG expected(0);

    const SparseGrid result(genInstr);

    BOOST_CHECK_EQUAL(result.nDim(), 0u);
    BOOST_CHECK(result == expected);
  }
}

BOOST_AUTO_TEST_CASE(Random_Full_1D_SG) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(10000));

    // Default boundary level offset 0: the boundary is always included.
    CompleteSGGenInstr genInstr(maxLvl, 1);
    genInstr.setNodeGenFuncs({equidistantNodeGenFunc});
    genInstr.setLvl2GPCntFuncs({linearLvl2GPCntFunction});

    // Expected
    ExpectedSG expected(1);
    expected.addTensorGrid(
        {LvlMI{maxLvl}, 1,
         makeTG({expectedNodes(equidistantNodeGenFunc, linearLvl2GPCntFunction(maxLvl), true)})});

    // Result
    const SparseGrid result(genInstr);

    // Comparison
    BOOST_CHECK_EQUAL(result.nDim(), 1u);
    BOOST_CHECK(result == expected);
  }
}

BOOST_AUTO_TEST_CASE(Complex_Full_2D_SG) {
  CompleteSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFuncs({equidistantNodeGenFunc, clenshawCurtisNodeGenFunc});
  genInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  // Expected: dim 1 has 2^(l+1)-1 equidistant inner nodes, dim 2 has l+1 Clenshaw-Curtis inner
  // nodes; the boundary nodes are added for l >= 1. The tensor grid (0, 0) has coefficient 0.
  ExpectedSG expected(2);

  expected.addTensorGrid({{2, 0},
                          1,
                          makeTG({expectedNodes(equidistantNodeGenFunc, 7, true),
                                  expectedNodes(clenshawCurtisNodeGenFunc, 1, false)})});
  expected.addTensorGrid({{1, 1},
                          1,
                          makeTG({expectedNodes(equidistantNodeGenFunc, 3, true),
                                  expectedNodes(clenshawCurtisNodeGenFunc, 2, true)})});
  expected.addTensorGrid({{0, 2},
                          1,
                          makeTG({expectedNodes(equidistantNodeGenFunc, 1, false),
                                  expectedNodes(clenshawCurtisNodeGenFunc, 3, true)})});
  expected.addTensorGrid({{1, 0},
                          -1,
                          makeTG({expectedNodes(equidistantNodeGenFunc, 3, true),
                                  expectedNodes(clenshawCurtisNodeGenFunc, 1, false)})});
  expected.addTensorGrid({{0, 1},
                          -1,
                          makeTG({expectedNodes(equidistantNodeGenFunc, 1, false),
                                  expectedNodes(clenshawCurtisNodeGenFunc, 2, true)})});

  // Explicit grid point counts, as a check of the expectation itself.
  BOOST_REQUIRE(expected.getTensorGrid(0).tensorGrid.getGPCntPerDim() == GPMI({9, 1}));
  BOOST_REQUIRE(expected.getTensorGrid(1).tensorGrid.getGPCntPerDim() == GPMI({5, 4}));
  BOOST_REQUIRE(expected.getTensorGrid(2).tensorGrid.getGPCntPerDim() == GPMI({1, 5}));
  BOOST_REQUIRE(expected.getTensorGrid(3).tensorGrid.getGPCntPerDim() == GPMI({5, 1}));
  BOOST_REQUIRE(expected.getTensorGrid(4).tensorGrid.getGPCntPerDim() == GPMI({1, 4}));

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Simple_Full_3D_Level0) {
  // Generation instruction
  const LvlType maxLvl = 0;
  CompleteSGGenInstr genInstr(maxLvl, 3);

  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, equidistantNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, linearLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  // Expected: the single tensor grid (0, 0, 0) with one node per dimension.
  ExpectedSG expected(3);

  const sgpp::base::DataVector nodes = expectedNodes(equidistantNodeGenFunc, 1, false);
  expected.addTensorGrid({{0, 0, 0}, 1, makeTG({nodes, nodes, nodes})});

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Complex_Full_3D_SG) {
  // Generation instruction
  const LvlType maxLvl = 2;

  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {doublingLvl2GPCntFunction, linearLvl2GPCntFunction, doublingLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  // Expected
  const ExpectedSG expected = genExpectedCompleteSG(genInstr, maxLvl);
  BOOST_REQUIRE_EQUAL(expected.nTG(), 10u);

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

/*
  Mixed_NodeFunctions_3D
  ----------------------
  Deterministic 3D test that uses mixed node-generation strategies
  (Clenshaw-Curtis in some dims, equidistant in others) for maxLvl = 2.
*/
BOOST_AUTO_TEST_CASE(Mixed_NodeFunctions_3D) {
  // Generation instruction
  const LvlType maxLvl = 2;

  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeGenFuncs(
      {clenshawCurtisNodeGenFunc, equidistantNodeGenFunc, clenshawCurtisNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  // Expected
  const ExpectedSG expected = genExpectedCompleteSG(genInstr, maxLvl);

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

/*
Random dimensionality, maximum level, node generation functions, growth functions and
boundary level offset. Tensor grids with coefficient 0 (|l|_1 < maxLvl - (d-1)) must not
be part of the sparse grid.
*/
BOOST_AUTO_TEST_CASE(Random_Complete_SG) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 10; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(6));

      BOOST_TEST_CONTEXT("nDim: " << nDim << ", maxLvl: " << maxLvl) {
        CompleteSGGenInstr genInstr(maxLvl, nDim);
        randomizeConfig(genInstr);

        const SparseGrid result(genInstr);

        BOOST_CHECK_EQUAL(result.nDim(), nDim);
        BOOST_CHECK_EQUAL(sumOfCoeffs(result), 1);
        BOOST_CHECK(result == genExpectedCompleteSG(genInstr, maxLvl));
      }
    }
  }
}

/*
The boundary nodes 0 and 1 are added in dimension k iff l_k >= boundaryLevelOffset:
offset 0 adds them to every tensor grid, a very large offset to none.
*/
BOOST_AUTO_TEST_CASE(BoundaryOffsetBehavior) {
  const LvlType maxLvl = 3;
  const size_t nDim = 3;

  for (const LvlType boundaryLevelOffset : std::vector<LvlType>{0, 1, 2, 100}) {
    BOOST_TEST_CONTEXT("Boundary level offset: " << boundaryLevelOffset) {
      CompleteSGGenInstr genInstr(maxLvl, nDim);
      genInstr.setNodeGenFunc(equidistantNodeGenFunc);
      genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);
      genInstr.setBoundaryLevelOffset(boundaryLevelOffset);

      const SparseGrid sg(genInstr);
      BOOST_REQUIRE_GT(sg.nTG(), 0u);

      for (const TensorGridCTData& tgData : sg) {
        const GPMI& gpCntPerDim = tgData.tensorGrid.getGPCntPerDim();
        const sgpp::base::DataVector& nodes = tgData.tensorGrid.getNodesPerDim();
        size_t firstNodeIdx = 0;

        for (size_t dim = 0; dim < nDim; dim++) {
          BOOST_TEST_CONTEXT("Tensor grid: " << toString(tgData.mi) << ", dim: " << dim) {
            const LvlType lvl = tgData.mi[dim];
            const bool hasBoundary = lvl >= boundaryLevelOffset;

            GPCntType expectedGPCnt = linearLvl2GPCntFunction(lvl);
            if (hasBoundary) expectedGPCnt += 2;

            BOOST_REQUIRE_EQUAL(gpCntPerDim[dim], expectedGPCnt);
            BOOST_CHECK_EQUAL(nodes[firstNodeIdx] == 0.0, hasBoundary);
            BOOST_CHECK_EQUAL(nodes[firstNodeIdx + gpCntPerDim[dim] - 1] == 1.0, hasBoundary);

            firstNodeIdx += gpCntPerDim[dim];
          }
        }
      }
    }
  }
}

/*
All node positions live on the unit domain [0,1]^d; the user domain only affects function
evaluations and the quadrature scaling, never the tensor grids.
*/
BOOST_AUTO_TEST_CASE(Domain_Does_Not_Affect_Tensor_Grids) {
  CompleteSGGenInstr unitGenInstr(3, 2);
  unitGenInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  unitGenInstr.setBoundaryLevelOffset(1);

  CompleteSGGenInstr scaledGenInstr(3, 2);
  scaledGenInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  scaledGenInstr.setBoundaryLevelOffset(1);
  scaledGenInstr.setDomain(HyperCubeArea{{-4.0, 2.0}, {10.0, 10.5}});

  const SparseGrid unitSG(unitGenInstr);
  const SparseGrid scaledSG(scaledGenInstr);

  BOOST_CHECK(scaledSG == unitSG);

  for (const TensorGridCTData& tgData : scaledSG) {
    for (const double node : tgData.tensorGrid.getNodesPerDim()) {
      BOOST_CHECK_GE(node, 0.0);
      BOOST_CHECK_LE(node, 1.0);
    }
  }
}

BOOST_AUTO_TEST_CASE(Meta_Values) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(4);
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(6));

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    randomizeConfig(genInstr);

    const SparseGrid sg(genInstr);

    size_t maxTGGPCnt = 0;
    size_t maxTGSumOverGPCntsPerDim = 0;
    for (const TensorGridCTData& tgData : sg) {
      maxTGGPCnt = std::max(maxTGGPCnt, tgData.tensorGrid.nGP());
      maxTGSumOverGPCntsPerDim = std::max(
          maxTGSumOverGPCntsPerDim, tgData.tensorGrid.getGPCntPerDim().sumOfElems<size_t>());
    }

    BOOST_CHECK_EQUAL(sg.getMaxTGGPCnt(), maxTGGPCnt);
    BOOST_CHECK_EQUAL(sg.getMaxTGSumOverGPCntsPerDim(), maxTGSumOverGPCntsPerDim);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGrid_Constructor_MIVec)

BOOST_AUTO_TEST_CASE(Anisotropic_2D) {
  // Not downwards closed: the closure is (0,0), (1,0), (2,0), (3,0), (0,1).
  const LvlMIVec miVec(std::vector<LvlMI>{{3, 0}, {0, 1}});

  MIVecSGGenInstr genInstr(miVec);
  genInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  // Expected: (1,0) and (2,0) have coefficient 0 and are omitted.
  ExpectedSG expected(2);

  expected.addTensorGrid({{3, 0},
                          1,
                          makeTG({expectedNodes(clenshawCurtisNodeGenFunc, 15, true),
                                  expectedNodes(equidistantNodeGenFunc, 1, false)})});
  expected.addTensorGrid({{0, 1},
                          1,
                          makeTG({expectedNodes(clenshawCurtisNodeGenFunc, 1, false),
                                  expectedNodes(equidistantNodeGenFunc, 2, true)})});
  expected.addTensorGrid({{0, 0},
                          -1,
                          makeTG({expectedNodes(clenshawCurtisNodeGenFunc, 1, false),
                                  expectedNodes(equidistantNodeGenFunc, 1, false)})});

  // Result
  const SparseGrid result(genInstr);

  // Comparison
  BOOST_CHECK(result == expected);
}

/*
A random (in general neither downwards closed nor duplicate-free) multi-index set,
compared with a brute-force downwards closure and inclusion-exclusion coefficients.
*/
BOOST_AUTO_TEST_CASE(Random_MIVec) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 10; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const size_t nMI = 1 + randGen.getUniformIndexRN(5);

      std::vector<LvlMI> inputMIs(nMI, LvlMI(nDim));
      for (LvlMI& mi : inputMIs) {
        for (LvlType& lvl : mi) lvl = static_cast<LvlType>(randGen.getUniformIndexRN(4));
      }

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", nMI: " << nMI) {
        const LvlMIVec miVec(inputMIs);

        MIVecSGGenInstr genInstr(miVec);
        randomizeConfig(genInstr);

        const SparseGrid result(genInstr);

        BOOST_CHECK_EQUAL(result.nDim(), nDim);
        BOOST_CHECK_EQUAL(sumOfCoeffs(result), 1);
        BOOST_CHECK(result == genExpectedMIVecSG(inputMIs, genInstr));
      }
    }
  }
}

/*
The downwards closure of all multi-indices with |l|_1 = maxLvl is the complete sparse grid
of level maxLvl, so both generation instructions must yield the same sparse grid.
*/
BOOST_AUTO_TEST_CASE(Top_Layer_Equals_Complete_SG) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 5; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(5));

      BOOST_TEST_CONTEXT("nDim: " << nDim << ", maxLvl: " << maxLvl) {
        std::vector<LvlMI> topLayer;
        forEachMIInBox(LvlMI(nDim, maxLvl), [&topLayer, maxLvl](const LvlMI& mi) {
          if (mi.sumOfElems() == maxLvl) topLayer.push_back(mi);
        });
        const LvlMIVec miVec(topLayer);

        CompleteSGGenInstr completeGenInstr(maxLvl, nDim);
        randomizeConfig(completeGenInstr);

        MIVecSGGenInstr miVecGenInstr(miVec);
        miVecGenInstr.setNodeGenFuncs(completeGenInstr.getNodeGenFuncs());
        miVecGenInstr.setLvl2GPCntFuncs(completeGenInstr.getLvl2GPCntFuncs());
        miVecGenInstr.setBoundaryLevelOffset(completeGenInstr.getBoundaryLevelOffset());

        const SparseGrid completeSG(completeGenInstr);
        const SparseGrid miVecSG(miVecGenInstr);

        BOOST_CHECK(miVecSG == completeSG);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGrid_GenInstr)

BOOST_AUTO_TEST_CASE(Stores_Copy_Of_Complete_Gen_Instr) {
  const LvlType maxLvl = 3;

  CompleteSGGenInstr genInstr(maxLvl, 2);
  genInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFuncs({linearLvl2GPCntFunction, doublingLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(2);
  genInstr.setDomain(HyperCubeArea{{-1.0, 1.0}, {2.0, 5.0}});

  const SparseGrid sg(genInstr);
  const SGGenInstr& stored = sg.getGenInstr();

  BOOST_CHECK(&stored != &genInstr);
  BOOST_CHECK(dynamic_cast<const CompleteSGGenInstr*>(&stored) != nullptr);
  checkSameConfig(stored, genInstr);

  // The stored instruction reproduces the grid.
  BOOST_CHECK(SparseGrid(stored) == sg);

  // Later changes of the original instruction affect neither the stored copy nor the grid.
  const ExpectedSG expected = genExpectedCompleteSG(genInstr, maxLvl);

  genInstr.setMaxLvl(5);
  genInstr.setNodeGenFunc(firstTypeChebyshevNodeGenFunc);
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);
  genInstr.setDomain({0.0, 1.0});

  BOOST_CHECK_EQUAL(stored.getBoundaryLevelOffset(), 2u);
  BOOST_CHECK(stored.getDomain() == HyperCubeArea({{-1.0, 1.0}, {2.0, 5.0}}));
  BOOST_CHECK(stored.getNodeGenFuncForDim(0) == clenshawCurtisNodeGenFunc);
  BOOST_CHECK(stored.getNodeGenFuncForDim(1) == equidistantNodeGenFunc);
  BOOST_CHECK(stored.getLvl2GPCntFuncForDim(0) == linearLvl2GPCntFunction);
  BOOST_CHECK(stored.getLvl2GPCntFuncForDim(1) == doublingLvl2GPCntFunction);

  BOOST_CHECK(sg == expected);
  BOOST_CHECK(SparseGrid(stored) == expected);
}

BOOST_AUTO_TEST_CASE(Stores_Copy_Of_MIVec_Gen_Instr) {
  const LvlMIVec miVec(std::vector<LvlMI>{{3, 0, 1}, {0, 2, 0}, {1, 1, 1}});

  MIVecSGGenInstr genInstr(miVec);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);
  genInstr.setDomain({-2.0, 2.0});

  const SparseGrid sg(genInstr);
  const SGGenInstr& stored = sg.getGenInstr();

  BOOST_CHECK(&stored != &genInstr);
  BOOST_CHECK(dynamic_cast<const MIVecSGGenInstr*>(&stored) != nullptr);
  checkSameConfig(stored, genInstr);

  BOOST_CHECK(SparseGrid(stored) == sg);
}

BOOST_AUTO_TEST_SUITE_END()
