// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <cmath>
#include <cstddef>
#include <memory>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/tools/paretoMaxima.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <sgpp_base.hpp>
#include <vector>

using namespace sgpp::combigrid;

BOOST_AUTO_TEST_SUITE(OP_find)

BOOST_AUTO_TEST_CASE(test_find_existing_mi) {
  // 3 Multiindices mit 2 Dimensionen
  const std::vector<LvlMI> mis = {LvlMI{1, 1}, LvlMI{2, 1}, LvlMI{2, 2}};

  const LvlMIVec miVec(mis);
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();

  BOOST_TEST(lookup->find(LvlMI{1, 1}) == 0);
  BOOST_TEST(lookup->find(LvlMI{2, 1}) == 1);
  BOOST_TEST(lookup->find(LvlMI{2, 2}) == 2);
}

BOOST_AUTO_TEST_CASE(test_contains) {
  const std::vector<LvlMI> mis = {LvlMI{0, 0, 0}, LvlMI{1, 0, 0}, LvlMI{1, 1, 0}};

  const LvlMIVec miVec(mis);
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();

  BOOST_TEST(lookup->contains(LvlMI{0, 0, 0}));
  BOOST_TEST(lookup->contains(LvlMI{1, 1, 0}));

  BOOST_TEST(!lookup->contains(LvlMI{0, 1, 0}));
  BOOST_TEST(!lookup->contains(LvlMI{2, 0, 0}));
}

BOOST_AUTO_TEST_CASE(test_find_not_existing_returns_nMI) {
  const std::vector<LvlMI> mis = {LvlMI{1, 2}, LvlMI{2, 3}};

  const LvlMIVec miVec(mis);
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();

  const size_t notFoundIndex = miVec.nMI();

  BOOST_TEST(lookup->find(LvlMI{0, 0}) == notFoundIndex);
  BOOST_TEST(lookup->find(LvlMI{3, 3}) == notFoundIndex);
}

BOOST_AUTO_TEST_CASE(test_empty_mivec) {
  const std::vector<LvlMI> mis;

  const LvlMIVec miVec(mis);
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();

  BOOST_TEST(miVec.nMI() == 0);
  BOOST_TEST(lookup->find(LvlMI{}) == 0);
  BOOST_TEST(!lookup->contains(LvlMI{}));
}

BOOST_AUTO_TEST_CASE(test_equal_content_different_object) {
  const std::vector<LvlMI> mis = {LvlMI{3, 3}, LvlMI{4, 4}};

  const LvlMIVec miVec(mis);
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();

  const LvlMI sameContent{3, 3};  // New object

  BOOST_TEST(lookup->contains(sameContent));
  BOOST_TEST(lookup->find(sameContent) == 0);
}

BOOST_AUTO_TEST_CASE(test_wrong_dimension) {
  const std::vector<LvlMI> mis = {LvlMI{1, 2}};

  const LvlMIVec miVec(mis);
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();

  // Wrong dimension
  BOOST_TEST(!lookup->contains(LvlMI{1, 2, 3}));
  BOOST_TEST(lookup->find(LvlMI{1, 2, 3}) == miVec.nMI());
}

BOOST_AUTO_TEST_SUITE_END()
