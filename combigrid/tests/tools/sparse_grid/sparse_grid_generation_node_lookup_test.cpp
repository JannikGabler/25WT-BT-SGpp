/**
 * @file sparse_grid_generation_node_lookup_test.cpp
 * @brief Reserved for tests of the (currently disabled) concurrent
 * node-lookup builder; the body is commented out.
 */

// TODO: Not used at the moment

// // Copyright (C) 2008-today The SG++ project
// // This file is part of the SG++ project. For conditions of distribution and
// // use, please see the copyright notice provided with SG++ or at
// // sgpp.sparsegrids.org
// #define BOOST_TEST_DYN_LINK

// #include <boost/test/tools/old/interface.hpp>
// #include <boost/test/unit_test.hpp>
// #include <boost/test/unit_test_log.hpp>
// #include <boost/test/unit_test_suite.hpp>

// #include <sgpp/base/tools/RandomNumberGenerator.hpp>
// #include <sgpp/combigrid/constants.hpp>
// #include
// <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
// #include
// <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
// #include
// <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
// #include
// <sgpp/combigrid/node_families/getter/second_type_chebyshev_nodes_getter.hpp>
// #include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
// #include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
// #include <sgpp/combigrid/tools/sparse_grid_generation/sparse_grid_generation_node_lookup.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
// #include <vector>

// using namespace sgpp::combigrid;

// static sgpp::base::RandomNumberGenerator& randGen =
//     sgpp::base::RandomNumberGenerator::getInstance();
// static NodeFamily* equidistantNodes = getEquidistantNodes();
// static NodeFamily* firstTypeChebyshevNodes = getFirstTypeChebyshevNodes();
// static NodeFamily* secondTypeChebyshevNodes = getSecondTypeChebyshevNodes();

// BOOST_AUTO_TEST_SUITE(Tools_genSGNodeLookup)

// BOOST_AUTO_TEST_CASE(ZeroDims) {
//   const CompleteSGGenInstr instr(3, 0);
//   const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff =
//   instr.genReducedMIVecWithCoeffs();

//   const SGGenNodeLookup result =
//       tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

//   const SGGenNodeLookup expected{};

//   BOOST_CHECK(result.size() == 0);

//   BOOST_CHECK(result == expected);
// }

// BOOST_AUTO_TEST_CASE(Simple1D) {
//   const CompleteSGGenInstr instr(2, 1);
//   const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff =
//   instr.genReducedMIVecWithCoeffs();

//   const SGGenNodeLookup result =
//       tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

//   const SGGenNodeLookup expected{
//       {{equidistantNodes, 1}, equidistantNodes->genGPs(1, false)},
//       {{equidistantNodes, 3}, equidistantNodes->genGPs(3, false)},
//       {{equidistantNodes, 7}, equidistantNodes->genGPs(7, false)}};

//   BOOST_CHECK(result == expected);
// }

// BOOST_AUTO_TEST_CASE(Simple2D) {
//   const CompleteSGGenInstr instr(3, 2);
//   const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff =
//   instr.genReducedMIVecWithCoeffs();

//   const SGGenNodeLookup result =
//       tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

//   const SGGenNodeLookup expected{
//       {{equidistantNodes, 1}, equidistantNodes->genGPs(1, false)},
//       {{equidistantNodes, 3}, equidistantNodes->genGPs(3, false)},
//       {{equidistantNodes, 7}, equidistantNodes->genGPs(7, false)},
//       {{equidistantNodes, 15}, equidistantNodes->genGPs(15, false)}};

//   BOOST_CHECK(result == expected);
// }

// BOOST_AUTO_TEST_CASE(Complex2D) {
//   CompleteSGGenInstr instr(2, 2);
//   instr.setNodeFamilyForDim(firstTypeChebyshevNodes, 1);
//   instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 1);

//   const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff =
//   instr.genReducedMIVecWithCoeffs();

//   const SGGenNodeLookup result =
//       tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

//   const SGGenNodeLookup expected{
//       {{equidistantNodes, 1}, equidistantNodes->genGPs(1, false)},
//       {{equidistantNodes, 3}, equidistantNodes->genGPs(3, false)},
//       {{equidistantNodes, 7}, equidistantNodes->genGPs(7, false)},
//       {{firstTypeChebyshevNodes, 1}, firstTypeChebyshevNodes->genGPs(1, false)},
//       {{firstTypeChebyshevNodes, 2}, firstTypeChebyshevNodes->genGPs(2, false)},
//       {{firstTypeChebyshevNodes, 3}, firstTypeChebyshevNodes->genGPs(3, false)}};

//   BOOST_CHECK(result == expected);
// }

// BOOST_AUTO_TEST_CASE(Complex3D) {
//   CompleteSGGenInstr instr(4, 4);
//   instr.setNodeFamilyForDim(firstTypeChebyshevNodes, 0);
//   instr.setLvl2GPCntFuncForDim(doublingLvl2GPCntFunction, 0);
//   instr.setNodeFamilyForDim(equidistantNodes, 1);
//   instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 1);
//   instr.setNodeFamilyForDim(firstTypeChebyshevNodes, 2);
//   instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 2);
//   instr.setNodeFamilyForDim(secondTypeChebyshevNodes, 3);
//   instr.setLvl2GPCntFuncForDim(linearLvl2GPCntFunction, 3);

//   const std::pair<LvlMIVec, std::vector<CTCoeffType>> miVecWithCoeff =
//   instr.genReducedMIVecWithCoeffs();

//   const SGGenNodeLookup result =
//       tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

//   const SGGenNodeLookup expected{
//       {{equidistantNodes, 1}, equidistantNodes->genGPs(1, false)},
//       {{equidistantNodes, 2}, equidistantNodes->genGPs(2, false)},
//       {{equidistantNodes, 3}, equidistantNodes->genGPs(3, false)},
//       {{equidistantNodes, 4}, equidistantNodes->genGPs(4, false)},
//       {{equidistantNodes, 5}, equidistantNodes->genGPs(5, false)},
//       {{firstTypeChebyshevNodes, 1}, firstTypeChebyshevNodes->genGPs(1, false)},
//       {{firstTypeChebyshevNodes, 2}, firstTypeChebyshevNodes->genGPs(2, false)},
//       {{firstTypeChebyshevNodes, 3}, firstTypeChebyshevNodes->genGPs(3, false)},
//       {{firstTypeChebyshevNodes, 4}, firstTypeChebyshevNodes->genGPs(4, false)},
//       {{firstTypeChebyshevNodes, 5}, firstTypeChebyshevNodes->genGPs(5, false)},
//       {{firstTypeChebyshevNodes, 7}, firstTypeChebyshevNodes->genGPs(7, false)},
//       {{firstTypeChebyshevNodes, 15}, firstTypeChebyshevNodes->genGPs(15, false)},
//       {{firstTypeChebyshevNodes, 31}, firstTypeChebyshevNodes->genGPs(31, false)},
//       {{secondTypeChebyshevNodes, 1}, secondTypeChebyshevNodes->genGPs(1, false)},
//       {{secondTypeChebyshevNodes, 2}, secondTypeChebyshevNodes->genGPs(2, false)},
//       {{secondTypeChebyshevNodes, 3}, secondTypeChebyshevNodes->genGPs(3, false)},
//       {{secondTypeChebyshevNodes, 4}, secondTypeChebyshevNodes->genGPs(4, false)},
//       {{secondTypeChebyshevNodes, 5}, secondTypeChebyshevNodes->genGPs(5, false)}};

//   BOOST_CHECK(result == expected);
// }

// BOOST_AUTO_TEST_SUITE_END()