// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_construction_test.cpp
 * @brief Boost.Test cases verifying the construction of @c SparseGrid objects from
 * @c CompleteSGGenInstr and @c MIVecSGGenInstr instructions (level multi-indices, combination
 * coefficients, node positions, boundary handling, cached meta values) and the generation
 * instruction stored in the grid.
 *
 * The expected grids are assembled by hand in @c ExpectedSG, a minimal subclass of
 * @c SparseGridBase with a public constructor and @c addTensorGrid. Their multi-indices and
 * coefficients are computed by closed formulas or brute force, and their node lists only from
 * @c NodeGenFunc::genNodesWithoutBoundary, independently of the library's sparse grid generation
 * code.
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
static NodeGenFunc* const equidistantNodeGenFunc = getEquidistantNodeGenFunc();
static NodeGenFunc* const clenshawCurtisNodeGenFunc = getClenshawCurtisNodeGenFunc();
static NodeGenFunc* const firstTypeChebyshevNodeGenFunc = getFirstTypeChebyshevNodeGenFunc();

namespace {

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
 * Lvl2GPCntFunc_k(l_k) inner nodes of NodeGenFunc_k, plus the boundary nodes 0 and 1 iff
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
 * Expected complete sparse grid of level @p maxLvl (with at least one dimension): every l with
 * |l|_1 <= maxLvl whose closed-form coefficient (-1)^q * binom(d - 1, q), q = maxLvl - |l|_1,
 * is non-zero.
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
      expected.addTensorGrid(TensorGridCTData{mi, coeff, expectedTG(mi, genInstr)});
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

    if (coeff != 0) {
      expected.addTensorGrid(TensorGridCTData{mi, coeff, expectedTG(mi, genInstr)});
    }
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

/// Assigns random node generation and growth functions per dimension and a random boundary
/// level offset in {0, 1, 2}.
void setRandomSettings(SGGenInstr& genInstr) {
  NodeGenFunc* const nodeGenFuncs[] = {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc,
                                       firstTypeChebyshevNodeGenFunc};
  const Lvl2GPCntFunc lvl2GPCntFuncs[] = {linearLvl2GPCntFunction, doublingLvl2GPCntFunction};

  for (size_t dim = 0; dim < genInstr.nDim(); dim++) {
    genInstr.setNodeGenFuncForDim(nodeGenFuncs[randGen.getUniformIndexRN(3)], dim);
    genInstr.setLvl2GPCntFuncForDim(lvl2GPCntFuncs[randGen.getUniformIndexRN(2)], dim);
  }

  genInstr.setBoundaryLevelOffset(static_cast<LvlType>(randGen.getUniformIndexRN(3)));
}

/// Copies domain, node generation functions, growth functions and boundary level offset.
void copySettings(const SGGenInstr& from, SGGenInstr& to) {
  to.setDomain(from.getDomain());
  to.setNodeGenFuncs(from.getNodeGenFuncs());
  to.setLvl2GPCntFuncs(from.getLvl2GPCntFuncs());
  to.setBoundaryLevelOffset(from.getBoundaryLevelOffset());
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

}  // namespace

BOOST_AUTO_TEST_SUITE(SparseGrid_complete_construction)

BOOST_AUTO_TEST_CASE(Zero_Dims) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(10));

    const CompleteSGGenInstr genInstr(maxLvl, 0);
    const SparseGrid result(genInstr);

    checkSG(result, ExpectedSG(0));
    BOOST_CHECK_EQUAL(result.getMaxTGGPCnt(), size_t{0});
    BOOST_CHECK_EQUAL(result.getMaxTGSumOverGPCntsPerDim(), size_t{0});
  }
}

BOOST_AUTO_TEST_CASE(Random_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(10000));

    CompleteSGGenInstr genInstr(maxLvl, 1);
    genInstr.setNodeGenFunc(equidistantNodeGenFunc);
    genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);
    genInstr.setBoundaryLevelOffset(0);

    // A single full grid of level maxLvl: maxLvl + 1 inner nodes and the boundary
    ExpectedSG expected(1);
    expected.addTensorGrid(TensorGridCTData{
        {maxLvl}, 1, makeTG({expectedNodes(equidistantNodeGenFunc, maxLvl + 1, true)})});

    const SparseGrid result(genInstr);

    checkSG(result, expected);
    const size_t nNodes = static_cast<size_t>(maxLvl) + 3;
    BOOST_CHECK_EQUAL(result.getMaxTGGPCnt(), nNodes);
    BOOST_CHECK_EQUAL(result.getMaxTGSumOverGPCntsPerDim(), nNodes);
  }
}

BOOST_AUTO_TEST_CASE(Complex_2D) {
  CompleteSGGenInstr genInstr(2, 2);
  genInstr.setNodeGenFuncs({equidistantNodeGenFunc, clenshawCurtisNodeGenFunc});
  genInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  // Dimension 0: 2^(l+1) - 1 equidistant inner nodes, dimension 1: l + 1 Clenshaw-Curtis inner
  // nodes. Boundary nodes for every level >= 1. The tensor grid (0, 0) has coefficient 0.
  const NodeGenFunc* const eq = equidistantNodeGenFunc;
  const NodeGenFunc* const cc = clenshawCurtisNodeGenFunc;

  ExpectedSG expected(2);
  expected.addTensorGrid(TensorGridCTData{
      {2, 0}, 1, makeTG({expectedNodes(eq, 7, true), expectedNodes(cc, 1, false)})});  // 9 x 1
  expected.addTensorGrid(TensorGridCTData{
      {1, 1}, 1, makeTG({expectedNodes(eq, 3, true), expectedNodes(cc, 2, true)})});  // 5 x 4
  expected.addTensorGrid(TensorGridCTData{
      {0, 2}, 1, makeTG({expectedNodes(eq, 1, false), expectedNodes(cc, 3, true)})});  // 1 x 5
  expected.addTensorGrid(TensorGridCTData{
      {1, 0}, -1, makeTG({expectedNodes(eq, 3, true), expectedNodes(cc, 1, false)})});  // 5 x 1
  expected.addTensorGrid(TensorGridCTData{
      {0, 1}, -1, makeTG({expectedNodes(eq, 1, false), expectedNodes(cc, 2, true)})});  // 1 x 4

  const SparseGrid result(genInstr);

  checkSG(result, expected);
  // The two maxima stem from different tensor grids: (1, 1) and (2, 0)
  BOOST_CHECK_EQUAL(result.getMaxTGGPCnt(), size_t{20});
  BOOST_CHECK_EQUAL(result.getMaxTGSumOverGPCntsPerDim(), size_t{10});
}

BOOST_AUTO_TEST_CASE(Level0_3D) {
  CompleteSGGenInstr genInstr(0, 3);
  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);

  // A single tensor grid consisting of the midpoint of the unit cube
  ExpectedSG expected(3);
  expected.addTensorGrid(TensorGridCTData{
      {0, 0, 0}, 1, TensorGrid(GPMI{1, 1, 1}, sgpp::base::DataVector{0.5, 0.5, 0.5})});

  checkSG(SparseGrid(genInstr), expected);
}

BOOST_AUTO_TEST_CASE(Mixed_Settings_3D) {
  const LvlType maxLvl = 2;

  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {doublingLvl2GPCntFunction, linearLvl2GPCntFunction, doublingLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  const SparseGrid result(genInstr);

  // All 10 multi-indices with |l|_1 <= 2 have a non-zero coefficient in 3D
  BOOST_CHECK_EQUAL(result.nTG(), size_t{10});
  checkSG(result, genExpectedCompleteSG(genInstr, maxLvl));
}

BOOST_AUTO_TEST_CASE(Random_Settings) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(5));

      CompleteSGGenInstr genInstr(maxLvl, nDim);
      setRandomSettings(genInstr);

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", maxLvl: " << maxLvl) {
        const SparseGrid result(genInstr);

        checkSG(result, genExpectedCompleteSG(genInstr, maxLvl));
        checkMetaValues(result);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Omits_Zero_Coefficients) {
  const LvlType maxLvl = 4;

  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);

  const SparseGrid result(genInstr);

  // In 3D only the 6 + 10 + 15 multi-indices with 2 <= |l|_1 <= 4 have a non-zero coefficient
  BOOST_CHECK_EQUAL(result.nTG(), size_t{31});

  for (const TensorGridCTData& tg : result) {
    BOOST_TEST_CONTEXT("Tensor grid " << toString(tg.mi)) {
      BOOST_CHECK_NE(tg.coefficient, 0);
      BOOST_CHECK_GE(tg.mi.sumOfElems(), maxLvl - 2);
      BOOST_CHECK_LE(tg.mi.sumOfElems(), maxLvl);
    }
  }

  BOOST_CHECK(result.getTensorGrid(LvlMI{0, 0, 0}) == result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{1, 0, 0}) == result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{0, 0, 5}) == result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{1, 1, 0}) != result.end());
  BOOST_CHECK(result.getTensorGrid(LvlMI{0, 0, 4}) != result.end());
}

BOOST_AUTO_TEST_CASE(Boundary_Level_Offset) {
  const LvlType maxLvl = 3;

  for (const LvlType boundaryLevelOffset : {0u, 1u, 2u, 1000u}) {
    BOOST_TEST_CONTEXT("Boundary level offset: " << boundaryLevelOffset) {
      CompleteSGGenInstr genInstr(maxLvl, 3);
      genInstr.setNodeGenFuncs(
          {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc});
      genInstr.setLvl2GPCntFuncs(
          {linearLvl2GPCntFunction, doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
      genInstr.setBoundaryLevelOffset(boundaryLevelOffset);

      const SparseGrid result(genInstr);

      checkSG(result, genExpectedCompleteSG(genInstr, maxLvl));

      // Dimension k contains the boundary nodes 0 and 1 (as first and last node) iff
      // l_k >= boundary level offset
      for (const TensorGridCTData& tg : result) {
        const GPMI& gpCntPerDim = tg.tensorGrid.getGPCntPerDim();
        const sgpp::base::DataVector& nodesPerDim = tg.tensorGrid.getNodesPerDim();
        size_t firstNodeIdx = 0;

        for (size_t dim = 0; dim < 3; dim++) {
          BOOST_TEST_CONTEXT("Tensor grid " << toString(tg.mi) << ", dimension " << dim) {
            const bool withBoundary = tg.mi[dim] >= boundaryLevelOffset;
            const GPCntType nInnerNodes = genInstr.getLvl2GPCntFuncForDim(dim)(tg.mi[dim]);
            const size_t lastNodeIdx = firstNodeIdx + gpCntPerDim[dim] - 1;

            BOOST_CHECK_EQUAL(gpCntPerDim[dim], withBoundary ? nInnerNodes + 2 : nInnerNodes);
            BOOST_CHECK_EQUAL(nodesPerDim[firstNodeIdx] == 0.0 && nodesPerDim[lastNodeIdx] == 1.0,
                              withBoundary);
          }

          firstNodeIdx += gpCntPerDim[dim];
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Nodes_Independent_Of_Domain) {
  // Nodes are always generated in the unit domain, the domain is only applied on evaluation
  CompleteSGGenInstr unitGenInstr(3, 2);
  unitGenInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  unitGenInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  unitGenInstr.setBoundaryLevelOffset(1);

  CompleteSGGenInstr scaledGenInstr = unitGenInstr;
  const HyperCubeArea domain{{-2.0, 3.0}, {10.0, 10.5}};
  scaledGenInstr.setDomain(domain);

  const SparseGrid scaledSG(scaledGenInstr);

  checkSG(scaledSG, SparseGrid(unitGenInstr));
  BOOST_CHECK(scaledSG.getGenInstr().getDomain() == domain);
}

BOOST_AUTO_TEST_CASE(Coefficients_Sum_To_One) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(6);
      const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(7));

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", maxLvl: " << maxLvl) {
        CompleteSGGenInstr genInstr(maxLvl, nDim);
        genInstr.setNodeGenFunc(equidistantNodeGenFunc);
        genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);

        BOOST_CHECK_EQUAL(sumOfCoeffs(SparseGrid(genInstr)), 1);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGrid_mivec_construction)

BOOST_AUTO_TEST_CASE(Downwards_Closed_2D) {
  const LvlMIVec mis{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}};

  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, equidistantNodeGenFunc});
  genInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  // Inclusion-exclusion yields c(2, 0) = c(1, 1) = 1, c(1, 0) = -1 and c(0, 0) = c(0, 1) = 0
  const NodeGenFunc* const cc = clenshawCurtisNodeGenFunc;
  const NodeGenFunc* const eq = equidistantNodeGenFunc;

  ExpectedSG expected(2);
  expected.addTensorGrid(TensorGridCTData{
      {2, 0}, 1, makeTG({expectedNodes(cc, 7, true), expectedNodes(eq, 1, false)})});
  expected.addTensorGrid(TensorGridCTData{
      {1, 1}, 1, makeTG({expectedNodes(cc, 3, true), expectedNodes(eq, 2, true)})});
  expected.addTensorGrid(TensorGridCTData{
      {1, 0}, -1, makeTG({expectedNodes(cc, 3, true), expectedNodes(eq, 1, false)})});

  const SparseGrid result(genInstr);

  checkSG(result, expected);
  checkMetaValues(result);
}

BOOST_AUTO_TEST_CASE(Non_Downwards_Closed_Input) {
  const LvlMIVec closedMIs{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}};
  const LvlMIVec paretoMaxima{{2, 0}, {1, 1}};
  const LvlMIVec partialMIs{{1, 1}, {0, 1}, {2, 0}};

  MIVecSGGenInstr closedGenInstr(closedMIs);
  closedGenInstr.setNodeGenFuncs({equidistantNodeGenFunc, firstTypeChebyshevNodeGenFunc});
  closedGenInstr.setBoundaryLevelOffset(1);

  MIVecSGGenInstr paretoGenInstr(paretoMaxima);
  copySettings(closedGenInstr, paretoGenInstr);
  MIVecSGGenInstr partialGenInstr(partialMIs);
  copySettings(closedGenInstr, partialGenInstr);

  // The input is completed to its downwards closure
  const SparseGrid expected(closedGenInstr);

  checkSG(SparseGrid(paretoGenInstr), expected);
  checkSG(SparseGrid(partialGenInstr), expected);
}

BOOST_AUTO_TEST_CASE(Single_MI_Gives_Full_Grid) {
  // The downwards closure of a single multi-index l is the box [0, l]; only l itself has a
  // non-zero coefficient
  const LvlMI mi{2, 0, 3};
  const LvlMIVec mis{mi};

  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeGenFuncs(
      {clenshawCurtisNodeGenFunc, equidistantNodeGenFunc, firstTypeChebyshevNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, doublingLvl2GPCntFunction, doublingLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);

  ExpectedSG expected(3);
  expected.addTensorGrid(TensorGridCTData{mi, 1, expectedTG(mi, genInstr)});

  checkSG(SparseGrid(genInstr), expected);
}

BOOST_AUTO_TEST_CASE(Matches_Complete_SG) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(5));

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", maxLvl: " << maxLvl) {
        CompleteSGGenInstr completeGenInstr(maxLvl, nDim);
        setRandomSettings(completeGenInstr);

        // The multi-indices with |l|_1 = maxLvl span the complete sparse grid
        std::vector<LvlMI> paretoMaxima;
        forEachMIInBox(LvlMI(nDim, maxLvl), [&](const LvlMI& mi) {
          if (mi.sumOfElems() == maxLvl) paretoMaxima.push_back(mi);
        });

        const LvlMIVec mis(paretoMaxima);
        MIVecSGGenInstr miVecGenInstr(mis);
        copySettings(completeGenInstr, miVecGenInstr);

        checkSG(SparseGrid(miVecGenInstr), SparseGrid(completeGenInstr));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Random_Input) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);
      const size_t nInputMIs = 1 + randGen.getUniformIndexRN(5);

      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < nInputMIs; i++) {
        inputMIs.push_back(randomMI(nDim, 3));
      }

      BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim << ", nInputMIs: " << nInputMIs) {
        const LvlMIVec mis(inputMIs);
        MIVecSGGenInstr genInstr(mis);
        setRandomSettings(genInstr);

        const SparseGrid result(genInstr);

        checkSG(result, genExpectedMIVecSG(inputMIs, genInstr));
        checkMetaValues(result);
        BOOST_CHECK_EQUAL(sumOfCoeffs(result), 1);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SparseGrid_gen_instr)

BOOST_AUTO_TEST_CASE(Consistent_With_Gen_Instr) {
  // The tensor grids are exactly the multi-indices and coefficients generated by the instruction
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t run = 0; run < 20; run++) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(4);

      std::vector<LvlMI> inputMIs;
      for (size_t i = 0; i < 3; i++) {
        inputMIs.push_back(randomMI(nDim, 3));
      }
      const LvlMIVec mis(inputMIs);

      CompleteSGGenInstr completeGenInstr(static_cast<LvlType>(randGen.getUniformIndexRN(5)),
                                          nDim);
      setRandomSettings(completeGenInstr);
      MIVecSGGenInstr miVecGenInstr(mis);
      setRandomSettings(miVecGenInstr);

      for (const SGGenInstr* const genInstr :
           {static_cast<const SGGenInstr*>(&completeGenInstr),
            static_cast<const SGGenInstr*>(&miVecGenInstr)}) {
        BOOST_TEST_CONTEXT("Run: " << run << ", nDim: " << nDim
                                   << (genInstr == &completeGenInstr ? ", complete" : ", MIVec")) {
          const SparseGrid result(*genInstr);
          const auto miVecWithCoeffs = genInstr->genReducedMIVecWithCoeffs();
          const LvlMIVec& miVec = miVecWithCoeffs.first;
          const std::vector<CTCoeffType>& coeffs = miVecWithCoeffs.second;

          BOOST_REQUIRE_EQUAL(result.nTG(), miVec.nMI());

          for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
            const LvlMI mi = miVec[miIdx];
            const auto iter = result.getTensorGrid(mi);

            BOOST_TEST_CONTEXT("Tensor grid " << toString(mi)) {
              BOOST_REQUIRE(iter != result.end());
              BOOST_CHECK_EQUAL(iter->coefficient, coeffs[miIdx]);
              BOOST_CHECK(iter->tensorGrid == expectedTG(mi, *genInstr));
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Stores_Copy_Of_Complete_SG_Gen_Instr) {
  const LvlType maxLvl = 2;

  CompleteSGGenInstr genInstr(maxLvl, 3);
  genInstr.setNodeGenFuncs(
      {equidistantNodeGenFunc, clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc});
  genInstr.setLvl2GPCntFuncs(
      {linearLvl2GPCntFunction, doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(1);
  genInstr.setDomain(HyperCubeArea{{0.0, 1.0}, {-1.0, 1.0}, {2.0, 5.0}});

  const CompleteSGGenInstr originalGenInstr = genInstr;
  const SparseGrid sg(genInstr);
  const SGGenInstr& storedGenInstr = sg.getGenInstr();

  BOOST_CHECK(&storedGenInstr != &genInstr);
  BOOST_CHECK(dynamic_cast<const CompleteSGGenInstr*>(&storedGenInstr) != nullptr);
  checkSameSettings(storedGenInstr, originalGenInstr);

  // Later changes of the original instruction affect neither the stored instruction nor the grid
  genInstr.setMaxLvl(4);
  genInstr.setNodeGenFunc(clenshawCurtisNodeGenFunc);
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);
  genInstr.setDomain({-5.0, 5.0});

  checkSameSettings(storedGenInstr, originalGenInstr);
  checkSG(SparseGrid(storedGenInstr), SparseGrid(originalGenInstr));
  checkSG(sg, genExpectedCompleteSG(originalGenInstr, maxLvl));
}

BOOST_AUTO_TEST_CASE(Stores_Copy_Of_MIVec_SG_Gen_Instr) {
  const LvlMIVec mis{{3, 0}, {1, 2}};

  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeGenFuncs({clenshawCurtisNodeGenFunc, firstTypeChebyshevNodeGenFunc});
  genInstr.setLvl2GPCntFuncs({doublingLvl2GPCntFunction, linearLvl2GPCntFunction});
  genInstr.setBoundaryLevelOffset(2);

  const MIVecSGGenInstr originalGenInstr = genInstr;
  const SparseGrid sg(genInstr);
  const SGGenInstr& storedGenInstr = sg.getGenInstr();

  BOOST_CHECK(&storedGenInstr != &genInstr);
  BOOST_CHECK(dynamic_cast<const MIVecSGGenInstr*>(&storedGenInstr) != nullptr);
  checkSameSettings(storedGenInstr, originalGenInstr);

  // Later changes of the original instruction affect neither the stored instruction nor the grid
  genInstr.setNodeGenFunc(equidistantNodeGenFunc);
  genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);

  checkSameSettings(storedGenInstr, originalGenInstr);
  checkSG(SparseGrid(storedGenInstr), SparseGrid(originalGenInstr));
  checkSG(sg, genExpectedMIVecSG({{3, 0}, {1, 2}}, originalGenInstr));
}

BOOST_AUTO_TEST_SUITE_END()
