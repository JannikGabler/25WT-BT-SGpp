// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#include "sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp"
#include <vector>
#include "sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp"
#include "sgpp/combigrid/node_generation_functions/node_generation_functions.hpp"
#include "sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp"
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients.hpp>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

BOOST_AUTO_TEST_SUITE(Tools_genSGNodeLookup)

BOOST_AUTO_TEST_CASE(Simple1D) {
  const FullSGGenInstr instr(3, 1);
  const std::pair<MIVec, std::vector<CTCoeffType>> miVecWithCoeff =
      instr.genCompleteMIVecWithCoeff();

  const SGGenNodeLookup result =
      tools::genSGNodeLookup(instr, miVecWithCoeff.first, miVecWithCoeff.second);

  BOOST_REQUIRE(result.size() == 1);

  const auto entry = *result.begin();
  BOOST_CHECK(entry.first.first == genEquidistantNodes);
  BOOST_CHECK(entry.first.second == 7);
  BOOST_CHECK_EQUAL_COLLECTIONS(entry.second.begin(), entry.second.end(),
                                genEquidistantNodes(7).begin(), genEquidistantNodes(7).end());
}

BOOST_AUTO_TEST_SUITE_END()