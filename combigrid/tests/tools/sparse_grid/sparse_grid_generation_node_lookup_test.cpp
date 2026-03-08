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
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/second_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();
static NodeGenFunc* equidistantNodeGenFunc = getEquidistantNodeGenFunc();
static NodeGenFunc* firstTypeChebyshevNodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
static NodeGenFunc* secondTypeChebyshevNodeGenFunc = getSecondTypeChebyshevNodeGenFunc();

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

  const SGGenNodeLookup expected{
      {{equidistantNodeGenFunc, 1}, equidistantNodeGenFunc->genGPs(1, false)},
      {{equidistantNodeGenFunc, 3}, equidistantNodeGenFunc->genGPs(3, false)},
      {{equidistantNodeGenFunc, 7}, equidistantNodeGenFunc->genGPs(7, false)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Simple2D) {
  const FullSGGenInstr instr(3, 2);
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{
      {{equidistantNodeGenFunc, 1}, equidistantNodeGenFunc->genGPs(1, false)},
      {{equidistantNodeGenFunc, 3}, equidistantNodeGenFunc->genGPs(3, false)},
      {{equidistantNodeGenFunc, 7}, equidistantNodeGenFunc->genGPs(7, false)},
      {{equidistantNodeGenFunc, 15}, equidistantNodeGenFunc->genGPs(15, false)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Complex2D) {
  FullSGGenInstr instr(2, 2);
  instr.setNodeGenFuncForDim(firstTypeChebyshevNodeGenFunc, 1);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 1);

  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{
      {{equidistantNodeGenFunc, 1}, equidistantNodeGenFunc->genGPs(1, false)},
      {{equidistantNodeGenFunc, 3}, equidistantNodeGenFunc->genGPs(3, false)},
      {{equidistantNodeGenFunc, 7}, equidistantNodeGenFunc->genGPs(7, false)},
      {{firstTypeChebyshevNodeGenFunc, 1}, firstTypeChebyshevNodeGenFunc->genGPs(1, false)},
      {{firstTypeChebyshevNodeGenFunc, 2}, firstTypeChebyshevNodeGenFunc->genGPs(2, false)},
      {{firstTypeChebyshevNodeGenFunc, 3}, firstTypeChebyshevNodeGenFunc->genGPs(3, false)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(Complex3D) {
  FullSGGenInstr instr(4, 4);
  instr.setNodeGenFuncForDim(firstTypeChebyshevNodeGenFunc, 0);
  instr.setLvl2GPCntFuncForDim(doublingLvl2GPCntFunction, 0);
  instr.setNodeGenFuncForDim(equidistantNodeGenFunc, 1);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 1);
  instr.setNodeGenFuncForDim(firstTypeChebyshevNodeGenFunc, 2);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 2);
  instr.setNodeGenFuncForDim(secondTypeChebyshevNodeGenFunc, 3);
  instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 3);

  const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff = instr.genMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  const SGGenNodeLookup expected{
      {{equidistantNodeGenFunc, 1}, equidistantNodeGenFunc->genGPs(1, false)},
      {{equidistantNodeGenFunc, 2}, equidistantNodeGenFunc->genGPs(2, false)},
      {{equidistantNodeGenFunc, 3}, equidistantNodeGenFunc->genGPs(3, false)},
      {{equidistantNodeGenFunc, 4}, equidistantNodeGenFunc->genGPs(4, false)},
      {{equidistantNodeGenFunc, 5}, equidistantNodeGenFunc->genGPs(5, false)},
      {{firstTypeChebyshevNodeGenFunc, 1}, firstTypeChebyshevNodeGenFunc->genGPs(1, false)},
      {{firstTypeChebyshevNodeGenFunc, 2}, firstTypeChebyshevNodeGenFunc->genGPs(2, false)},
      {{firstTypeChebyshevNodeGenFunc, 3}, firstTypeChebyshevNodeGenFunc->genGPs(3, false)},
      {{firstTypeChebyshevNodeGenFunc, 4}, firstTypeChebyshevNodeGenFunc->genGPs(4, false)},
      {{firstTypeChebyshevNodeGenFunc, 5}, firstTypeChebyshevNodeGenFunc->genGPs(5, false)},
      {{firstTypeChebyshevNodeGenFunc, 7}, firstTypeChebyshevNodeGenFunc->genGPs(7, false)},
      {{firstTypeChebyshevNodeGenFunc, 15}, firstTypeChebyshevNodeGenFunc->genGPs(15, false)},
      {{firstTypeChebyshevNodeGenFunc, 31}, firstTypeChebyshevNodeGenFunc->genGPs(31, false)},
      {{secondTypeChebyshevNodeGenFunc, 1}, secondTypeChebyshevNodeGenFunc->genGPs(1, false)},
      {{secondTypeChebyshevNodeGenFunc, 2}, secondTypeChebyshevNodeGenFunc->genGPs(2, false)},
      {{secondTypeChebyshevNodeGenFunc, 3}, secondTypeChebyshevNodeGenFunc->genGPs(3, false)},
      {{secondTypeChebyshevNodeGenFunc, 4}, secondTypeChebyshevNodeGenFunc->genGPs(4, false)},
      {{secondTypeChebyshevNodeGenFunc, 5}, secondTypeChebyshevNodeGenFunc->genGPs(5, false)}};

  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_SUITE_END()