// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cmath>
#include "sgpp/combigrid/multiindices/multiindex.hpp"
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <sgpp/globaldef.hpp>
#include <sgpp_base.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(OP_find)

BOOST_AUTO_TEST_CASE(test_find_existing_mi) {
  // 3 Multiindices mit 2 Dimensionen
  const std::vector<sgpp::combigrid::MI> mis = {
      sgpp::combigrid::MI{1, 1}, sgpp::combigrid::MI{2, 1}, sgpp::combigrid::MI{2, 2}};

  const sgpp::combigrid::MIVec miVec(mis);
  const sgpp::combigrid::misc::MIVecLookup lookup(miVec);

  BOOST_TEST(lookup.find(sgpp::combigrid::MI{1, 1}) == 0);
  BOOST_TEST(lookup.find(sgpp::combigrid::MI{2, 1}) == 1);
  BOOST_TEST(lookup.find(sgpp::combigrid::MI{2, 2}) == 2);
}

BOOST_AUTO_TEST_CASE(test_contains) {
  const std::vector<sgpp::combigrid::MI> mis = {
      sgpp::combigrid::MI{0, 0, 0}, sgpp::combigrid::MI{1, 0, 0}, sgpp::combigrid::MI{1, 1, 0}};

  const sgpp::combigrid::MIVec miVec(mis);
  const sgpp::combigrid::misc::MIVecLookup lookup(miVec);

  BOOST_TEST(lookup.contains(sgpp::combigrid::MI{0, 0, 0}));
  BOOST_TEST(lookup.contains(sgpp::combigrid::MI{1, 1, 0}));

  BOOST_TEST(!lookup.contains(sgpp::combigrid::MI{0, 1, 0}));
  BOOST_TEST(!lookup.contains(sgpp::combigrid::MI{2, 0, 0}));
}

BOOST_AUTO_TEST_CASE(test_find_not_existing_returns_nMI) {
  const std::vector<sgpp::combigrid::MI> mis = {sgpp::combigrid::MI{1, 2},
                                                sgpp::combigrid::MI{2, 3}};

  const sgpp::combigrid::MIVec miVec(mis);
  const sgpp::combigrid::misc::MIVecLookup lookup(miVec);

  const size_t notFoundIndex = miVec.nMI();

  BOOST_TEST(lookup.find(sgpp::combigrid::MI{0, 0}) == notFoundIndex);
  BOOST_TEST(lookup.find(sgpp::combigrid::MI{3, 3}) == notFoundIndex);
}

BOOST_AUTO_TEST_CASE(test_empty_mivec) {
  const std::vector<sgpp::combigrid::MI> mis;
  const sgpp::combigrid::MIVec miVec(mis);
  const sgpp::combigrid::misc::MIVecLookup lookup(miVec);

  BOOST_TEST(miVec.nMI() == 0);
  BOOST_TEST(lookup.find(sgpp::combigrid::MI{}) == 0);
  BOOST_TEST(!lookup.contains(sgpp::combigrid::MI{}));
}

BOOST_AUTO_TEST_CASE(test_equal_content_different_object) {
  const std::vector<sgpp::combigrid::MI> mis = {sgpp::combigrid::MI{3, 3},
                                                sgpp::combigrid::MI{4, 4}};

  const sgpp::combigrid::MIVec miVec(mis);
  const sgpp::combigrid::misc::MIVecLookup lookup(miVec);

  const sgpp::combigrid::MI sameContent{3, 3};  // New object

  BOOST_TEST(lookup.contains(sameContent));
  BOOST_TEST(lookup.find(sameContent) == 0);
}

BOOST_AUTO_TEST_CASE(test_wrong_dimension) {
  const std::vector<sgpp::combigrid::MI> mis = {sgpp::combigrid::MI{1, 2}};

  const sgpp::combigrid::MIVec miVec(mis);
  const sgpp::combigrid::misc::MIVecLookup lookup(miVec);

  // Wrong dimension
  BOOST_TEST(!lookup.contains(sgpp::combigrid::MI{1, 2, 3}));
  BOOST_TEST(lookup.find(sgpp::combigrid::MI{1, 2, 3}) == miVec.nMI());
}

BOOST_AUTO_TEST_SUITE_END()
