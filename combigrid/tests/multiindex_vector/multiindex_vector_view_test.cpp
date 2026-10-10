// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_view_test.cpp
 * @brief Boost.Test cases for @c MIVecView, the read-only view of a range of
 * the multi-indices of an @c MIVec: creation through @c MIVec::getView(),
 * element access, slicing, iterators (including their interoperability with
 * the iterators of the @c MIVec) and use with standard algorithms.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <ranges>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_lexicographic_less.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector_view.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

namespace {

using LvlMIView = MIView<LvlType>;
using Diff = LvlMIVec::difference_type;

/// MIVec whose entries encode their position: entry (miIdx, dim) is 10 * miIdx + dim.
LvlMIVec enumeratedMIVec(const size_t nDim, const size_t nMI) {
  LvlMIVec vec(nDim, nMI);
  for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
    for (size_t dim = 0; dim < nDim; dim++) {
      vec(miIdx, dim) = static_cast<LvlType>(10 * miIdx + dim);
    }
  }
  return vec;
}

/// Checks that the view shows exactly the count multi-indices of vec starting at offset,
/// without copying them, and that its iterators are positions within vec.
void checkShowsRange(const LvlMIVecView view, const LvlMIVec& vec, const size_t offset,
                     const size_t count) {
  BOOST_TEST(view.nDim() == vec.nDim());
  BOOST_TEST(view.nMI() == count);
  BOOST_TEST(view.size() == count);
  BOOST_TEST(view.empty() == (count == 0));
  BOOST_CHECK(view.data() == vec.data() + offset * vec.nDim());

  BOOST_CHECK(view.begin() == vec.begin() + static_cast<Diff>(offset));
  BOOST_CHECK(view.end() == vec.begin() + static_cast<Diff>(offset + count));
  BOOST_TEST(view.begin().index() == static_cast<Diff>(offset));
  BOOST_TEST((view.end() - view.begin()) == static_cast<Diff>(count));

  for (size_t miIdx = 0; miIdx < count; miIdx++) {
    BOOST_CHECK(view[miIdx] == vec[offset + miIdx]);
    BOOST_CHECK(view[miIdx].data() == vec[offset + miIdx].data());
    for (size_t dim = 0; dim < vec.nDim(); dim++) {
      BOOST_TEST(view(miIdx, dim) == vec(offset + miIdx, dim));
    }
  }
  if (count > 0) {
    BOOST_CHECK(view.front().data() == vec[offset].data());
    BOOST_CHECK(view.back().data() == vec[offset + count - 1].data());
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(MIVecView_construction)

BOOST_AUTO_TEST_CASE(default_constructed_is_empty) {
  const LvlMIVecView view;

  BOOST_TEST(view.empty());
  BOOST_TEST(view.nMI() == 0u);
  BOOST_TEST(view.nDim() == 0u);
  BOOST_CHECK(view.data() == nullptr);
  BOOST_CHECK(view.begin() == view.end());
  BOOST_TEST(view.first(0).empty());
  BOOST_TEST(view.last(0).empty());
  BOOST_TEST(view.subview(0).empty());
}

BOOST_AUTO_TEST_CASE(view_of_whole_vector) {
  for (const size_t nDim : {1u, 2u, 5u, 40u}) {
    for (const size_t nMI : {0u, 1u, 7u}) {
      const LvlMIVec vec = enumeratedMIVec(nDim, nMI);
      checkShowsRange(vec.getView(), vec, 0, nMI);
    }
  }
}

BOOST_AUTO_TEST_CASE(view_of_raw_storage) {
  const LvlMIVec vec = enumeratedMIVec(3, 4);
  const LvlMIVecView view(vec.data() + vec.nDim(), vec.nDim(), 2);

  BOOST_TEST(view.size() == 2u);
  BOOST_CHECK(view[0] == vec[1]);
  BOOST_CHECK(view[1] == vec[2]);
  BOOST_TEST((view.end() - view.begin()) == 2);
}

BOOST_AUTO_TEST_CASE(view_does_not_copy) {
  LvlMIVec vec{{1, 5}, {4, 2}, {0, 0}};
  const LvlMIVecView view = vec.getView().last(2);

  vec[1] = LvlMI{7, 9};
  vec(2, 1) = 3;

  BOOST_CHECK(view[0] == (LvlMI{7, 9}));
  BOOST_CHECK(view[1] == (LvlMI{0, 3}));

  const LvlMI copy = view[0];
  vec[1] = LvlMI{0, 0};
  BOOST_CHECK(copy == (LvlMI{7, 9}));
}

BOOST_AUTO_TEST_CASE(get_view_keeps_cached_values) {
  LvlMIVec vec{{1, 5}, {4, 2}};
  const LvlMIVec& constVec = vec;
  const std::shared_ptr<LvlMI> max = constVec.componentWiseMax();

  // getView() is const: called on a non-const MIVec it must not drop the cache either.
  const LvlMIVecView view = vec.getView();
  static_cast<void>(view.first(1).begin());
  BOOST_CHECK(view[1] == (LvlMI{4, 2}));

  BOOST_CHECK(constVec.componentWiseMax() == max);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MIVecView_subviews)

BOOST_AUTO_TEST_CASE(all_ranges) {
  for (const size_t nDim : {1u, 3u}) {
    const size_t nMI = 6;
    const LvlMIVec vec = enumeratedMIVec(nDim, nMI);
    const LvlMIVecView view = vec.getView();

    for (size_t offset = 0; offset <= nMI; offset++) {
      checkShowsRange(view.subview(offset), vec, offset, nMI - offset);
      for (size_t count = 0; count <= nMI - offset; count++) {
        checkShowsRange(view.subview(offset, count), vec, offset, count);
      }
    }
    for (size_t count = 0; count <= nMI; count++) {
      checkShowsRange(view.first(count), vec, 0, count);
      checkShowsRange(view.last(count), vec, nMI - count, count);
    }
  }
}

BOOST_AUTO_TEST_CASE(nested_subviews) {
  const LvlMIVec vec = enumeratedMIVec(2, 10);
  const LvlMIVecView view = vec.getView();

  checkShowsRange(view.subview(1, 8).last(5).first(2), vec, 4, 2);
  checkShowsRange(view.last(7).subview(2).subview(1, 3), vec, 6, 3);
  checkShowsRange(view.first(4).last(4).subview(0), vec, 0, 4);
  checkShowsRange(view.last(3).last(0), vec, 10, 0);
}

BOOST_AUTO_TEST_CASE(zero_dimensions) {
  // All multi-indices share one address; only the index-based iterators tell them apart.
  const LvlMIVec vec(0, 5);
  const LvlMIVecView view = vec.getView();

  BOOST_TEST(view.size() == 5u);
  BOOST_TEST(view.nDim() == 0u);
  BOOST_TEST(view.last(2).size() == 2u);
  BOOST_TEST(view.last(2).begin().index() == 3);
  BOOST_TEST((view.subview(1, 3).end() - view.begin()) == 4);

  size_t visited = 0;
  for (const LvlMIView mi : view.subview(1, 3)) {
    BOOST_TEST(mi.empty());
    visited++;
  }
  BOOST_TEST(visited == 3u);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MIVecView_iterators)

BOOST_AUTO_TEST_CASE(traversal) {
  const LvlMIVec vec = enumeratedMIVec(2, 5);
  const LvlMIVecView view = vec.getView().subview(1, 3);

  std::vector<LvlType> forward;
  for (LvlMIVecView::const_iterator it = view.cbegin(); it != view.cend(); ++it) {
    forward.push_back((*it)[0]);
  }
  BOOST_CHECK(forward == (std::vector<LvlType>{10, 20, 30}));

  std::vector<LvlType> backward;
  for (LvlMIVecView::const_reverse_iterator it = view.crbegin(); it != view.crend(); ++it) {
    backward.push_back((*it)[0]);
  }
  BOOST_CHECK(backward == (std::vector<LvlType>{30, 20, 10}));
  BOOST_CHECK(view.rbegin() == view.crbegin());
  BOOST_CHECK(view.rend() == view.crend());

  BOOST_CHECK(view.begin()[2] == vec[3]);
  BOOST_CHECK(*(view.end() - 1) == view.back());
  BOOST_CHECK(view.begin() < view.end());
}

BOOST_AUTO_TEST_CASE(interchangeable_with_container_iterators) {
  LvlMIVec vec = enumeratedMIVec(3, 8);
  const LvlMIVec& constVec = vec;
  const LvlMIVecView sub = constVec.getView().subview(2, 3);

  BOOST_CHECK(sub.begin() == constVec.cbegin() + 2);
  BOOST_CHECK(sub.end() == constVec.cbegin() + 5);
  BOOST_TEST((sub.begin() - constVec.cbegin()) == 2);
  BOOST_TEST((constVec.cend() - sub.end()) == 3);
  BOOST_TEST(std::distance(constVec.getView().first(1).end(), sub.begin()) == 1);
  BOOST_CHECK(sub.end() == constVec.getView().last(3).begin());

  // The iterators of the view are accepted wherever the MIVec expects its own.
  vec.erase(sub.begin(), sub.end());

  BOOST_TEST(vec.nMI() == 5u);
  for (const size_t miIdx : {0u, 1u, 2u, 3u, 4u}) {
    BOOST_TEST(vec(miIdx, 0) == (miIdx < 2 ? 10 * miIdx : 10 * (miIdx + 3)));
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MIVecView_algorithms)

BOOST_AUTO_TEST_CASE(classic_algorithms) {
  const LvlMIVec vec{{0, 0}, {0, 3}, {1, 1}, {2, 0}, {2, 1}, {4, 4}};
  const LvlMIVecView view = vec.getView().subview(1, 4);

  const LvlMIVecView::iterator found = std::find(view.begin(), view.end(), LvlMI{2, 0});
  BOOST_CHECK(found == view.begin() + 2);
  BOOST_TEST(found.index() == 3);
  BOOST_CHECK(std::find(view.begin(), view.end(), LvlMI{4, 4}) == view.end());
  BOOST_CHECK(std::find(view.begin(), view.end(), LvlMI{0, 0}) == view.end());

  BOOST_TEST(std::count_if(view.begin(), view.end(),
                           [](const LvlMIView mi) { return mi[0] == 2; }) == 2);
  BOOST_TEST(std::is_sorted(view.begin(), view.end(), MILexLess{}));
  BOOST_CHECK(std::lower_bound(view.begin(), view.end(), LvlMI{2, 0}, MILexLess{}) == found);

  const std::vector<LvlMI> copies(view.begin(), view.end());
  BOOST_CHECK(copies == (std::vector<LvlMI>{{0, 3}, {1, 1}, {2, 0}, {2, 1}}));
}

#if __cplusplus >= 202002L && defined(__cpp_lib_ranges)
BOOST_AUTO_TEST_CASE(ranges_algorithms) {
  const LvlMIVec vec{{0, 0}, {0, 3}, {1, 1}, {2, 0}, {2, 1}, {4, 4}};
  const LvlMIVecView view = vec.getView();

  LvlType sum = 0;
  for (const LvlMIView mi : view.first(3)) {
    sum += mi.sumOfElems();
  }
  BOOST_TEST(sum == 5u);

  BOOST_CHECK(std::ranges::equal(view, vec));
  BOOST_CHECK(std::ranges::equal(view.last(2), std::vector<LvlMI>{{2, 1}, {4, 4}}));
  BOOST_TEST(std::ranges::is_sorted(view, MILexLess{}));

  // Borrowed range: the iterator found in a temporary view stays valid and is a position in vec.
  const auto found = std::ranges::find(vec.getView().last(4), LvlMI{2, 1});
  static_assert(std::same_as<decltype(found), const LvlMIVec::const_iterator>);
  BOOST_TEST(found.index() == 4);
  BOOST_CHECK(*found == vec[4]);

  // Adaptors compose with the view without wrapping it in a reference.
  const auto firstEntries = view.subview(2, 3) | std::views::reverse |
                            std::views::transform([](const LvlMIView mi) { return mi[0]; });
  BOOST_CHECK(std::ranges::equal(firstEntries, std::vector<LvlType>{2, 2, 1}));
  static_assert(std::same_as<std::views::all_t<LvlMIVecView>, LvlMIVecView>);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
