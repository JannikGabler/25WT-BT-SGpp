// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_container_test.cpp
 * @brief Boost.Test cases for the container interface of @c MIVec: construction,
 * element access through @c MIView / @c MIRef, modification, iterators,
 * comparisons in all combinations with @c MI, and cache invalidation.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <iterator>
#include <memory>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_lexicographic_less.hpp>
#include <sgpp/combigrid/multiindices/multiindex_reference.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;

namespace {

using LvlMIView = MIView<LvlType>;
using LvlMIRef = MIRef<LvlType>;

sgpp::base::RandomNumberGenerator& randGen = sgpp::base::RandomNumberGenerator::getInstance();

/// Returns true iff the view and the multi-index have the same entries (checked element-wise).
bool sameEntries(const LvlMIView view, const LvlMI& mi) {
  if (view.size() != mi.size()) return false;
  for (size_t dim = 0; dim < mi.size(); dim++) {
    if (view[dim] != mi[dim]) return false;
  }
  return true;
}

LvlMI randomMI(const size_t nDim, const size_t maxValue) {
  LvlMI mi(nDim);
  for (size_t dim = 0; dim < nDim; dim++) {
    mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(maxValue + 1));
  }
  return mi;
}

/// Results of all six comparison operators, in the order ==, !=, <, <=, >, >=.
template <typename A, typename B>
std::vector<bool> compareAll(const A& a, const B& b) {
  std::vector<bool> result;
  result.push_back(a == b);
  result.push_back(a != b);
  result.push_back(a < b);
  result.push_back(a <= b);
  result.push_back(a > b);
  result.push_back(a >= b);
  return result;
}

/// Checks that all six comparison operators throw std::logic_error.
template <typename A, typename B>
void checkAllThrow(const A& a, const B& b) {
  BOOST_CHECK_THROW(static_cast<void>(a == b), std::logic_error);
  BOOST_CHECK_THROW(static_cast<void>(a != b), std::logic_error);
  BOOST_CHECK_THROW(static_cast<void>(a < b), std::logic_error);
  BOOST_CHECK_THROW(static_cast<void>(a <= b), std::logic_error);
  BOOST_CHECK_THROW(static_cast<void>(a > b), std::logic_error);
  BOOST_CHECK_THROW(static_cast<void>(a >= b), std::logic_error);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(MIVec_construction)

BOOST_AUTO_TEST_CASE(from_dimensions) {
  const LvlMIVec vec(3, 4);

  BOOST_TEST(vec.nDim() == 3u);
  BOOST_TEST(vec.nMI() == 4u);
  BOOST_TEST(vec.size() == 4u);
  BOOST_TEST(!vec.empty());
  for (size_t i = 0; i < vec.nMI(); i++) {
    BOOST_CHECK(vec[i] == LvlMI(3, 0));
  }
}

BOOST_AUTO_TEST_CASE(from_vector_of_mi_vectors_and_initializer_list) {
  const std::vector<LvlMI> mis = {LvlMI{1, 2, 3}, LvlMI{4, 5, 6}};
  const std::vector<std::vector<LvlType>> raw = {{1, 2, 3}, {4, 5, 6}};
  const LvlMIVec fromMIs(mis);
  const LvlMIVec fromRaw(raw);
  const LvlMIVec fromList{{1, 2, 3}, {4, 5, 6}};

  for (const LvlMIVec* vec : {&fromMIs, &fromRaw, &fromList}) {
    BOOST_TEST(vec->nDim() == 3u);
    BOOST_TEST(vec->nMI() == 2u);
    BOOST_CHECK(sameEntries((*vec)[0], mis[0]));
    BOOST_CHECK(sameEntries((*vec)[1], mis[1]));
    BOOST_TEST((*vec)(1, 2) == 6u);
  }
}

BOOST_AUTO_TEST_CASE(empty_container) {
  LvlMIVec vec(3, 0);
  const LvlMIVec& constVec = vec;

  BOOST_TEST(vec.empty());
  BOOST_TEST(vec.size() == 0u);
  BOOST_CHECK(vec.begin() == vec.end());
  BOOST_CHECK(constVec.begin() == constVec.end());
  BOOST_CHECK(vec.rbegin() == vec.rend());
  BOOST_TEST(std::distance(constVec.begin(), constVec.end()) == 0);

  size_t visited = 0;
  for (const LvlMIView mi : constVec) {
    static_cast<void>(mi);
    visited++;
  }
  BOOST_TEST(visited == 0u);

  vec.push_back(LvlMI{1, 2, 3});
  BOOST_TEST(vec.nMI() == 1u);
  BOOST_CHECK(vec[0] == (LvlMI{1, 2, 3}));
  vec.pop_back();
  BOOST_TEST(vec.empty());
  BOOST_TEST(vec.nDim() == 3u);
}

BOOST_AUTO_TEST_CASE(dimension_one) {
  LvlMIVec vec{{3}, {1}, {2}};
  const LvlMIVec& constVec = vec;

  BOOST_TEST(vec.nDim() == 1u);
  BOOST_CHECK(constVec[0] == LvlMI{3});
  BOOST_CHECK(constVec[1] < constVec[0]);
  BOOST_CHECK(constVec[2] >= constVec[1]);

  vec[0] = constVec[1];
  BOOST_TEST(vec(0, 0) == 1u);
  BOOST_TEST(constVec[2].sumOfElems() == 2u);
}

BOOST_AUTO_TEST_CASE(large_dimension) {
  const size_t nDim = 256;
  randGen.setSeed(42);

  std::vector<LvlMI> mis;
  for (size_t i = 0; i < 20; i++) {
    mis.push_back(randomMI(nDim, 1000));
  }
  LvlMIVec vec(mis);
  const LvlMIVec& constVec = vec;

  BOOST_TEST(vec.nDim() == nDim);
  for (size_t i = 0; i < mis.size(); i++) {
    BOOST_CHECK(constVec[i] == mis[i]);
    BOOST_CHECK(sameEntries(constVec[i], mis[i]));
    BOOST_TEST(constVec[i].sumOfElems<size_t>() == mis[i].sumOfElems<size_t>());
  }

  vec[3] = mis[7];
  BOOST_CHECK(constVec[3] == constVec[7]);
}

BOOST_AUTO_TEST_CASE(zero_dimensions) {
  LvlMIVec vec(0, 3);
  const LvlMIVec& constVec = vec;

  BOOST_TEST(vec.nMI() == 3u);
  BOOST_TEST(std::distance(constVec.begin(), constVec.end()) == 3);
  BOOST_TEST(constVec[1].empty());
  // Zero-dimensional multi-indices compare vacuously, exactly like MI.
  BOOST_CHECK(constVec[0] == constVec[2]);
  BOOST_CHECK(constVec[0] < constVec[2]);
  BOOST_CHECK(constVec[0] == LvlMI());

  vec.erase(vec.begin() + 1);
  BOOST_TEST(vec.nMI() == 2u);
  vec.push_back(LvlMI());
  BOOST_TEST(vec.nMI() == 3u);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MIVec_element_access)

BOOST_AUTO_TEST_CASE(views_refer_to_storage) {
  LvlMIVec vec{{1, 2}, {3, 4}, {5, 6}};
  const LvlMIVec& constVec = vec;

  // No MI is created: the view points directly into the flat storage.
  BOOST_CHECK(constVec[1].data() == constVec.data() + 2);
  BOOST_CHECK(vec[2].data() == constVec.data() + 4);
  BOOST_CHECK((*(constVec.begin() + 2)).data() == constVec.data() + 4);
  BOOST_TEST(constVec[1].nDim() == 2u);
  BOOST_TEST(constVec[1].front() == 3u);
  BOOST_TEST(constVec[1].back() == 4u);
  BOOST_TEST(constVec.front()[0] == 1u);
  BOOST_TEST(constVec.back()[1] == 6u);
}

BOOST_AUTO_TEST_CASE(conversion_to_mi_is_an_independent_copy) {
  LvlMIVec vec{{1, 2}, {3, 4}};

  const LvlMI copy = vec[1];
  const LvlMI copy2 = vec[1].toMI();
  vec(1, 0) = 9;

  BOOST_CHECK(copy == (LvlMI{3, 4}));
  BOOST_CHECK(copy2 == (LvlMI{3, 4}));
  BOOST_CHECK(vec[1] == (LvlMI{9, 4}));
}

BOOST_AUTO_TEST_CASE(mi_helpers_match_mi) {
  randGen.setSeed(7);
  for (size_t nDim = 0; nDim < 6; nDim++) {
    const LvlMI mi = randomMI(nDim, 5);
    const LvlMIVec vec(std::vector<LvlMI>{mi});
    const LvlMIView view = static_cast<const LvlMIVec&>(vec)[0];

    BOOST_TEST(view.sumOfElems() == mi.sumOfElems());
    BOOST_TEST(view.sumOfElems<size_t>() == mi.sumOfElems<size_t>());
    BOOST_TEST(view.productofElems() == mi.productofElems());
    BOOST_TEST(view.productofElems<size_t>() == mi.productofElems<size_t>());
    BOOST_TEST(view.nDim() == mi.nDim());
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MIVec_modification)

BOOST_AUTO_TEST_CASE(assignment_through_proxy_copies_contents) {
  LvlMIVec vec{{1, 1}, {2, 2}, {3, 3}};
  const LvlMIVec other{{7, 8}};

  vec[0] = LvlMI{5, 6};  // From MI.
  BOOST_CHECK(vec[0] == (LvlMI{5, 6}));

  vec[1] = other[0];  // From a view of another container.
  BOOST_CHECK(vec[1] == (LvlMI{7, 8}));

  vec[2] = vec[0];  // From a proxy of the same container.
  BOOST_CHECK(vec[2] == (LvlMI{5, 6}));

  vec[1] = vec[1];  // Self-assignment.
  BOOST_CHECK(vec[1] == (LvlMI{7, 8}));

  vec.setMI(0, other[0]);
  BOOST_CHECK(vec[0] == (LvlMI{7, 8}));
}

BOOST_AUTO_TEST_CASE(proxy_is_never_rebound) {
  LvlMIVec vec{{1, 1}, {2, 2}};

  LvlMIRef first = vec[0];  // Copy construction: alias of vec[0].
  first[1] = 5;
  BOOST_CHECK(vec[0] == (LvlMI{1, 5}));

  first = vec[1];  // Assignment copies contents; 'first' still refers to vec[0].
  first[0] = 9;
  BOOST_CHECK(vec[0] == (LvlMI{9, 2}));
  BOOST_CHECK(vec[1] == (LvlMI{2, 2}));
}

BOOST_AUTO_TEST_CASE(swap_proxies_and_iterators) {
  LvlMIVec vec{{1, 1}, {2, 2}, {3, 3}};

  swap(vec[0], vec[2]);
  BOOST_CHECK(vec[0] == (LvlMI{3, 3}));
  BOOST_CHECK(vec[2] == (LvlMI{1, 1}));

  std::iter_swap(vec.begin(), vec.begin() + 1);
  BOOST_CHECK(vec[0] == (LvlMI{2, 2}));
  BOOST_CHECK(vec[1] == (LvlMI{3, 3}));

  iter_swap(vec.begin() + 1, vec.begin() + 2);
  BOOST_CHECK(vec[1] == (LvlMI{1, 1}));
  BOOST_CHECK(vec[2] == (LvlMI{3, 3}));
}

BOOST_AUTO_TEST_CASE(push_back_aliasing_own_element) {
  LvlMIVec vec{{1, 2, 3}};

  // Every push_back may reallocate while the argument refers into the storage.
  for (size_t i = 0; i < 100; i++) {
    vec.push_back(vec[i]);
    vec(vec.nMI() - 1, 0)++;
  }
  BOOST_TEST(vec.nMI() == 101u);
  for (size_t i = 0; i < vec.nMI(); i++) {
    BOOST_CHECK(vec[i] == (LvlMI{static_cast<LvlType>(1 + i), 2, 3}));
  }
}

BOOST_AUTO_TEST_CASE(erase_clear_resize_reserve) {
  LvlMIVec vec{{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};

  LvlMIVec::iterator it = vec.erase(vec.begin() + 1);
  BOOST_TEST(it.index() == 1);
  BOOST_CHECK(*it == (LvlMI{2, 2}));
  BOOST_TEST(vec.nMI() == 4u);

  it = vec.erase(vec.begin() + 1, vec.begin() + 3);
  BOOST_CHECK(*it == (LvlMI{4, 4}));
  BOOST_TEST(vec.nMI() == 2u);
  BOOST_CHECK(vec[0] == (LvlMI{0, 0}));
  BOOST_CHECK(vec[1] == (LvlMI{4, 4}));

  vec.reserve(100);
  BOOST_CHECK(vec[1] == (LvlMI{4, 4}));
  vec.resize(3);
  BOOST_CHECK(vec[2] == (LvlMI{0, 0}));
  vec.shrink_to_fit();
  BOOST_TEST(vec.nMI() == 3u);

  vec.clear();
  BOOST_TEST(vec.empty());
  BOOST_TEST(vec.nDim() == 2u);
}

BOOST_AUTO_TEST_CASE(copy_move_and_swap_containers) {
  LvlMIVec original{{1, 2}, {3, 4}};

  LvlMIVec copy(original);
  copy[0] = LvlMI{9, 9};
  BOOST_CHECK(original[0] == (LvlMI{1, 2}));

  LvlMIVec moved(std::move(copy));
  BOOST_TEST(moved.nMI() == 2u);
  BOOST_CHECK(moved[0] == (LvlMI{9, 9}));
  BOOST_TEST(copy.nMI() == 0u);  // Documented moved-from state.
  BOOST_TEST(copy.nDim() == 2u);

  LvlMIVec assigned(3, 0);
  assigned = original;
  BOOST_TEST(assigned.nDim() == 2u);
  BOOST_CHECK(assigned[1] == (LvlMI{3, 4}));
  assigned = std::move(moved);
  BOOST_CHECK(assigned[0] == (LvlMI{9, 9}));

  swap(assigned, original);
  BOOST_CHECK(assigned[0] == (LvlMI{1, 2}));
  BOOST_CHECK(original[0] == (LvlMI{9, 9}));
}

BOOST_AUTO_TEST_CASE(iterator_arithmetic) {
  LvlMIVec vec{{0}, {1}, {2}, {3}, {4}};
  const LvlMIVec& constVec = vec;

  LvlMIVec::iterator it = vec.begin();
  BOOST_TEST((*(it + 3))[0] == 3u);
  BOOST_TEST((*(2 + it))[0] == 2u);
  BOOST_TEST(it[4][0] == 4u);
  BOOST_TEST((vec.end() - it) == 5);
  BOOST_CHECK(it < it + 1);
  BOOST_CHECK(it + 1 > it);
  BOOST_CHECK(it <= it);
  BOOST_CHECK(it >= it);
  BOOST_CHECK(++it == vec.begin() + 1);
  BOOST_CHECK(it++ == vec.begin() + 1);
  BOOST_CHECK(--it == vec.begin() + 1);
  BOOST_CHECK(it-- == vec.begin() + 1);
  BOOST_CHECK((vec.end() - 1)[0] == LvlMI{4});

  // Mutable iterators convert to const iterators and compare with them.
  const LvlMIVec::const_iterator cit = vec.begin() + 2;
  BOOST_CHECK(cit == vec.begin() + 2);
  BOOST_CHECK(cit != constVec.begin());
  BOOST_TEST((*cit)[0] == 2u);

  BOOST_TEST((*constVec.rbegin())[0] == 4u);
  BOOST_TEST((*vec.rbegin())[0] == 4u);
  BOOST_TEST(std::distance(constVec.crbegin(), constVec.crend()) == 5);

  size_t expected = 0;
  for (LvlMIRef mi : vec) {  // Range-for over a non-const MIVec yields writable proxies.
    BOOST_TEST(mi[0] == expected++);
    mi[0] *= 10;
  }
  BOOST_CHECK(constVec[3] == LvlMI{30});
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MIVec_comparison)

BOOST_AUTO_TEST_CASE(all_combinations_match_mi) {
  randGen.setSeed(1234);

  for (size_t nDim = 0; nDim <= 4; nDim++) {
    for (size_t trial = 0; trial < 200; trial++) {
      const LvlMI a = randomMI(nDim, 2);
      const LvlMI b = randomMI(nDim, 2);
      LvlMIVec vec(std::vector<LvlMI>{a, b});
      const LvlMIVec& constVec = vec;
      const std::vector<bool> expected = compareAll(a, b);

      BOOST_CHECK(compareAll(constVec[0], constVec[1]) == expected);  // view - view
      BOOST_CHECK(compareAll(constVec[0], b) == expected);            // view - MI
      BOOST_CHECK(compareAll(a, constVec[1]) == expected);            // MI - view
      BOOST_CHECK(compareAll(vec[0], vec[1]) == expected);            // ref - ref
      BOOST_CHECK(compareAll(vec[0], b) == expected);                 // ref - MI
      BOOST_CHECK(compareAll(a, vec[1]) == expected);                 // MI - ref
      BOOST_CHECK(compareAll(vec[0], constVec[1]) == expected);       // ref - view
      BOOST_CHECK(compareAll(constVec[0], vec[1]) == expected);       // view - ref
    }
  }
}

BOOST_AUTO_TEST_CASE(partial_order_semantics) {
  const LvlMIVec vec{{1, 2}, {2, 3}, {2, 1}, {1, 2}};

  BOOST_CHECK(vec[0] < vec[1]);
  BOOST_CHECK(vec[0] <= vec[1]);
  BOOST_CHECK(!(vec[0] < vec[3]));
  BOOST_CHECK(vec[0] <= vec[3]);
  BOOST_CHECK(vec[0] == vec[3]);
  // Incomparable: neither is <=, >= or == the other.
  BOOST_CHECK(!(vec[0] <= vec[2]) && !(vec[0] >= vec[2]) && vec[0] != vec[2]);
}

BOOST_AUTO_TEST_CASE(size_mismatch_throws) {
  LvlMIVec vec2{{1, 2}};
  LvlMIVec vec3{{1, 2, 3}};
  const LvlMIVec& constVec2 = vec2;
  const LvlMIVec& constVec3 = vec3;
  const LvlMI mi3{1, 2, 3};

  checkAllThrow(constVec2[0], constVec3[0]);
  checkAllThrow(constVec2[0], mi3);
  checkAllThrow(mi3, constVec2[0]);
  checkAllThrow(vec2[0], vec3[0]);
  checkAllThrow(vec2[0], mi3);
  checkAllThrow(mi3, vec2[0]);
  checkAllThrow(vec2[0], constVec3[0]);
  checkAllThrow(constVec3[0], vec2[0]);
}

BOOST_AUTO_TEST_CASE(lexicographic_comparator) {
  randGen.setSeed(99);
  const MILexLess lexLess;

  for (size_t trial = 0; trial < 500; trial++) {
    const LvlMI a = randomMI(3, 2);
    const LvlMI b = randomMI(3, 2);
    const LvlMIVec vec(std::vector<LvlMI>{a, b});
    const std::vector<LvlType> rawA(a.begin(), a.end());
    const std::vector<LvlType> rawB(b.begin(), b.end());
    const bool expected = rawA < rawB;

    BOOST_TEST(lexLess(a, b) == expected);
    BOOST_TEST(lexLess(vec[0], vec[1]) == expected);
    BOOST_TEST(lexLess(vec[0], b) == expected);
    BOOST_TEST(lexLess(a, vec[1]) == expected);
  }

  BOOST_TEST(lexLess(LvlMI{1, 2}, LvlMI{1, 2, 0}));  // Proper prefix is smaller.
  BOOST_TEST(!lexLess(LvlMI{1, 2}, LvlMI{1, 2}));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MIVec_cached_values)

BOOST_AUTO_TEST_CASE(copy_does_not_share_lookup) {
  // Regression test: the cached lookup stores pointers into the storage of the MIVec it was
  // built for. A copy used to share it and read freed memory after the original died.
  std::unique_ptr<LvlMIVec> original(new LvlMIVec{{0, 0}, {1, 0}});
  original->lookup();
  const LvlMIVec copy(*original);
  original.reset();

  BOOST_TEST(copy.lookup()->find(LvlMI{1, 0}) == 1u);
  BOOST_TEST(copy.isDownwardsClosed());
}

BOOST_AUTO_TEST_CASE(generated_mivec_has_valid_lookup) {
  // Regression test: without compaction, genMIVecWithCoeff used to return a copy of a local
  // MIVec whose cached lookup referred to the destroyed local.
  const LvlMIVec input{{0, 0}};
  const MIVecSGGenInstr genInstr(input);
  const std::pair<LvlMIVec, std::vector<CTCoeffType>> generated = genInstr.genMIVecWithCoeff();

  BOOST_TEST(generated.first.nMI() == 1u);
  BOOST_TEST(generated.first.isDownwardsClosed());
}

BOOST_AUTO_TEST_CASE(mutable_access_clears_cache) {
  LvlMIVec vec{{1, 5}, {4, 2}};
  const LvlMIVec& constVec = vec;

  const std::shared_ptr<LvlMI> max1 = constVec.componentWiseMax();
  BOOST_CHECK(*max1 == (LvlMI{4, 5}));

  // Read-only access keeps the cached value.
  BOOST_CHECK(constVec[0] == (LvlMI{1, 5}));
  static_cast<void>(constVec.begin());
  BOOST_CHECK(constVec.componentWiseMax() == max1);

  // Writing through a proxy obtained afterwards drops it.
  vec[0] = LvlMI{7, 1};
  BOOST_CHECK(*constVec.componentWiseMax() == (LvlMI{7, 2}));

  // Writing through a mutable iterator drops it.
  *vec.begin() = LvlMI{0, 9};
  BOOST_CHECK(*constVec.componentWiseMax() == (LvlMI{4, 9}));

  // Element-wise writes and resizing drop it.
  vec(1, 0) = 8;
  BOOST_CHECK(*constVec.componentWiseMax() == (LvlMI{8, 9}));
  vec.resize(1);
  BOOST_CHECK(*constVec.componentWiseMax() == (LvlMI{0, 9}));
}

BOOST_AUTO_TEST_CASE(lookup_accepts_views_and_proxies) {
  LvlMIVec vec{{0, 0}, {1, 0}, {0, 1}};
  const LvlMIVec other{{0, 1}, {5, 5}};
  const std::shared_ptr<LvlMIVecLookup> lookup = static_cast<const LvlMIVec&>(vec).lookup();

  BOOST_TEST(lookup->find(other[0]) == 2u);
  BOOST_TEST(!lookup->contains(other[1]));
  BOOST_TEST(lookup->find(LvlMI{1, 0}) == 1u);
  BOOST_TEST(lookup->find(LvlMI{1, 0, 0}) == vec.nMI());
}

BOOST_AUTO_TEST_SUITE_END()
