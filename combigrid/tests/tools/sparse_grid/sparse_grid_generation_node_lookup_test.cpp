// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_functions.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

BOOST_AUTO_TEST_SUITE(Tools_genSGNodeLookup)

BOOST_AUTO_TEST_CASE(ZeroDims) {
  const FullSGGenInstr instr(3, 0);
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{};

  BOOST_CHECK(result.size() == 0);

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Simple1D) {
  const FullSGGenInstr instr(2, 1);
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{{{genEquidistantNodes, 1}, genEquidistantNodes(1)},
                                 {{genEquidistantNodes, 3}, genEquidistantNodes(3)},
                                 {{genEquidistantNodes, 7}, genEquidistantNodes(7)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Simple2D) {
  const FullSGGenInstr instr(3, 2);
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{{{genEquidistantNodes, 1}, genEquidistantNodes(1)},
                                 {{genEquidistantNodes, 3}, genEquidistantNodes(3)},
                                 {{genEquidistantNodes, 7}, genEquidistantNodes(7)},
                                 {{genEquidistantNodes, 15}, genEquidistantNodes(15)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Complex2D) {
  FullSGGenInstr instr(2, 2);
  instr.setNodeGenFuncForDim(genFirstTypeChebyshevNodes, 1);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 1);

  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{{{genEquidistantNodes, 1}, genEquidistantNodes(1)},
                                 {{genEquidistantNodes, 3}, genEquidistantNodes(3)},
                                 {{genEquidistantNodes, 7}, genEquidistantNodes(7)},
                                 {{genFirstTypeChebyshevNodes, 1}, genFirstTypeChebyshevNodes(1)},
                                 {{genFirstTypeChebyshevNodes, 2}, genFirstTypeChebyshevNodes(2)},
                                 {{genFirstTypeChebyshevNodes, 3}, genFirstTypeChebyshevNodes(3)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Complex3D) {
  FullSGGenInstr instr(4, 4);
  instr.setNodeGenFuncForDim(genFirstTypeChebyshevNodes, 0);
  instr.setLvl2GPCntFuncForDim(doublingLvl2GPCntFunction, 0);
  instr.setNodeGenFuncForDim(genEquidistantNodes, 1);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 1);
  instr.setNodeGenFuncForDim(genFirstTypeChebyshevNodes, 2);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 2);
  instr.setNodeGenFuncForDim(genSecondTypeChebyshevNodes, 3);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 3);

  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{
      {{genEquidistantNodes, 1}, genEquidistantNodes(1)},
      {{genEquidistantNodes, 2}, genEquidistantNodes(2)},
      {{genEquidistantNodes, 3}, genEquidistantNodes(3)},
      {{genEquidistantNodes, 4}, genEquidistantNodes(4)},
      {{genEquidistantNodes, 5}, genEquidistantNodes(5)},
      {{genFirstTypeChebyshevNodes, 1}, genFirstTypeChebyshevNodes(1)},
      {{genFirstTypeChebyshevNodes, 2}, genFirstTypeChebyshevNodes(2)},
      {{genFirstTypeChebyshevNodes, 3}, genFirstTypeChebyshevNodes(3)},
      {{genFirstTypeChebyshevNodes, 4}, genFirstTypeChebyshevNodes(4)},
      {{genFirstTypeChebyshevNodes, 5}, genFirstTypeChebyshevNodes(5)},
      {{genFirstTypeChebyshevNodes, 7}, genFirstTypeChebyshevNodes(7)},
      {{genFirstTypeChebyshevNodes, 15}, genFirstTypeChebyshevNodes(15)},
      {{genFirstTypeChebyshevNodes, 31}, genFirstTypeChebyshevNodes(31)},
      {{genSecondTypeChebyshevNodes, 1}, genSecondTypeChebyshevNodes(1)},
      {{genSecondTypeChebyshevNodes, 2}, genSecondTypeChebyshevNodes(2)},
      {{genSecondTypeChebyshevNodes, 3}, genSecondTypeChebyshevNodes(3)},
      {{genSecondTypeChebyshevNodes, 4}, genSecondTypeChebyshevNodes(4)},
      {{genSecondTypeChebyshevNodes, 5}, genSecondTypeChebyshevNodes(5)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_SUITE_END()