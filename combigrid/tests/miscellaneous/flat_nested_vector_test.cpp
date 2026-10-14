// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file flat_nested_vector_test.cpp
 * @brief Boost.Test cases for @c FlatNestedVec and @c FlatNestedVecView:
 * conversion from and to nested @c std::vector, chained @c operator[] access,
 * views and their iterators, the flat @c std::vector interface, incremental
 * building (fuzzed against a nested @c std::vector) and value semantics.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector.hpp>
#include <sgpp/combigrid/miscellaneous/flat_nested_vector/flat_nested_vector_view.hpp>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

using sgpp::combigrid::misc::FlatNestedVec;
using sgpp::combigrid::misc::FlatNestedVecView;
using sgpp::combigrid::misc::NestedStdVec;

namespace {

/*************************************************************
Types returned by the nested access.
*************************************************************/
using FNV3 = FlatNestedVec<double, 3>;
static_assert(std::is_same_v<decltype(std::declval<FNV3&>()[0]),
                             FlatNestedVecView<double, 2, std::size_t>>);
static_assert(std::is_same_v<decltype(std::declval<FNV3&>()[0][0]), std::span<double>>);
static_assert(std::is_same_v<decltype(std::declval<FNV3&>()[0][0][0]), double&>);
static_assert(std::is_same_v<decltype(std::declval<const FNV3&>()[0]),
                             FlatNestedVecView<const double, 2, std::size_t>>);
static_assert(std::is_same_v<decltype(std::declval<const FNV3&>()[0][0]), std::span<const double>>);
static_assert(std::is_same_v<decltype(std::declval<const FNV3&>()[0][0][0]), const double&>);
static_assert(std::is_same_v<decltype(std::declval<FNV3&>().nested()),
                             FlatNestedVecView<double, 3, std::size_t>>);
static_assert(std::is_same_v<decltype(std::declval<FlatNestedVec<int, 1>&>()[0]), int&>);
static_assert(std::is_same_v<decltype(std::declval<FlatNestedVec<int, 2>&>()[0]), std::span<int>>);
static_assert(std::is_convertible_v<FlatNestedVecView<double, 2, std::size_t>,
                                    FlatNestedVecView<const double, 2, std::size_t>>);
static_assert(!std::is_convertible_v<FlatNestedVecView<const double, 2, std::size_t>,
                                     FlatNestedVecView<double, 2, std::size_t>>);

/*************************************************************
Helpers
*************************************************************/

/// Depth-first concatenation of all elements of a nested vector.
template <typename T>
void appendFlattened(const std::vector<T>& vec, std::vector<T>& out) {
  out.insert(out.end(), vec.begin(), vec.end());
}

template <typename T>
void appendFlattened(const std::vector<std::vector<T>>& vec, auto& out) {
  for (const auto& sub : vec) appendFlattened(sub, out);
}

/// Checks that the entry @p flat (a view, span or element) equals @p nested recursively,
/// both through operator[] and through iteration.
template <typename Entry, typename Nested>
void checkEqual(const Entry& flat, const Nested& nested) {
  if constexpr (std::is_arithmetic_v<Nested>) {
    BOOST_TEST(flat == nested);
  } else {
    BOOST_TEST_REQUIRE(flat.size() == nested.size());
    BOOST_TEST(flat.empty() == nested.empty());
    for (std::size_t i = 0; i < nested.size(); i++) checkEqual(flat[i], nested[i]);
    std::size_t i = 0;
    for (auto&& child : flat) checkEqual(child, nested[i++]);
    BOOST_TEST(i == nested.size());
  }
}

/// Checks @p fv against the nested vector @p nested it is supposed to represent.
template <typename T, std::size_t D, typename Index>
void checkRepresents(const FlatNestedVec<T, D, Index>& fv, const NestedStdVec<T, D>& nested) {
  BOOST_CHECK(fv.toNested() == nested);
  BOOST_CHECK((fv == FlatNestedVec<T, D, Index>(nested)));

  std::vector<T> flattened;
  appendFlattened(nested, flattened);
  BOOST_TEST_REQUIRE(fv.size() == flattened.size());
  BOOST_CHECK(std::ranges::equal(fv, flattened));
  if constexpr (D > 1) BOOST_CHECK(std::ranges::equal(fv.nested().flat(), flattened));
  BOOST_TEST(fv.nEntries(D) == flattened.size());
  BOOST_TEST(fv.nEntries(1) == nested.size());

  checkEqual(fv.nested(), nested);
  for (std::size_t i = 0; i < nested.size(); i++) checkEqual(fv[i], nested[i]);
}

/// A ragged depth-3 vector with empty sub-vectors at both depths.
NestedStdVec<double, 3> raggedDepth3() {
  return {{{1, 2, 3}, {}, {4}}, {}, {{5, 6}}, {{}, {}}, {{7}, {8, 9, 10, 11}}};
}

/// A ragged depth-4 vector with empty sub-vectors at every depth.
NestedStdVec<int, 4> raggedDepth4() {
  return {{{{1, 2}, {3}}, {}, {{}, {4, 5, 6}}},
          {},
          {{{7}}},
          {{{}}, {{}, {}}},
          {{{8, 9}, {10}}, {{11}}}};
}

/// Pointer to the last element in depth-first order, or nullptr.
double* lastElement(NestedStdVec<double, 3>& nested) {
  for (auto it1 = nested.rbegin(); it1 != nested.rend(); ++it1) {
    for (auto it2 = it1->rbegin(); it2 != it1->rend(); ++it2) {
      if (!it2->empty()) return &it2->back();
    }
  }
  return nullptr;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(testFlatNestedVec)

/*************************************************************
Construction and conversion
*************************************************************/

BOOST_AUTO_TEST_CASE(testDefaultConstructedIsEmpty) {
  const FlatNestedVec<double, 3> fv;
  BOOST_TEST(fv.empty());
  BOOST_TEST(fv.size() == 0U);
  BOOST_TEST(fv.capacity() == 0U);
  BOOST_TEST(fv.nEntries(1) == 0U);
  BOOST_TEST(fv.nEntries(2) == 0U);
  BOOST_TEST(fv.nested().empty());
  BOOST_TEST(fv.nested().flat().empty());
  BOOST_CHECK(fv.begin() == fv.end());
  BOOST_CHECK(fv.toNested().empty());
  BOOST_CHECK((fv == FlatNestedVec<double, 3>(NestedStdVec<double, 3>{})));
}

BOOST_AUTO_TEST_CASE(testDepth1BehavesLikeVector) {
  const std::vector<int> nested{4, 5, 6};
  FlatNestedVec<int, 1> fv(nested);
  checkRepresents(fv, nested);
  BOOST_TEST(fv[1] == 5);
  fv[1] = 7;
  fv.push_back(8);
  BOOST_CHECK(fv.toNested() == (std::vector<int>{4, 7, 6, 8}));
  fv.pop_back();
  BOOST_CHECK(std::ranges::equal(fv.nested(), std::vector<int>{4, 7, 6}));
}

BOOST_AUTO_TEST_CASE(testDepth2RoundTrip) {
  const NestedStdVec<int, 2> nested{{1, 2}, {}, {3}, {4, 5, 6}, {}};
  const FlatNestedVec<int, 2> fv(nested);
  checkRepresents(fv, nested);
  BOOST_TEST(fv.nEntries(1) == 5U);
  BOOST_TEST(fv[3][2] == 6);
  BOOST_CHECK(fv[3].data() == fv.data() + 3);
}

BOOST_AUTO_TEST_CASE(testDepth3RoundTrip) {
  const auto nested = raggedDepth3();
  const FlatNestedVec<double, 3> fv(nested);
  checkRepresents(fv, nested);
  BOOST_TEST(fv.nEntries(1) == 5U);
  BOOST_TEST(fv.nEntries(2) == 8U);
  BOOST_TEST(fv.nEntries(3) == 11U);
  BOOST_TEST(fv[4][1][3] == 11.0);
  BOOST_TEST(fv[4][1].size() == 4U);
  BOOST_TEST(fv[1].size() == 0U);
  BOOST_TEST(fv[3][1].size() == 0U);
}

BOOST_AUTO_TEST_CASE(testDepth4RoundTrip) {
  const auto nested = raggedDepth4();
  const FlatNestedVec<int, 4> fv(nested);
  checkRepresents(fv, nested);
  BOOST_TEST(fv[0][2][1][2] == 6);
  BOOST_TEST(fv[4][1][0][0] == 11);
}

BOOST_AUTO_TEST_CASE(testNarrowIndexTypes) {
  const auto nested = raggedDepth4();
  checkRepresents(FlatNestedVec<int, 4, std::uint32_t>(nested), nested);
  checkRepresents(FlatNestedVec<int, 4, std::uint16_t>(nested), nested);
  checkRepresents(FlatNestedVec<int, 4, std::uint8_t>(nested), nested);
}

/*************************************************************
Nested access
*************************************************************/

BOOST_AUTO_TEST_CASE(testWritesThroughNestedAccess) {
  FlatNestedVec<double, 3> fv(raggedDepth3());
  fv[0][2][0] = -4.0;
  auto view = fv[4];
  view[1][0] = -8.0;
  std::span<double> span = view[1];
  span[3] = -11.0;
  BOOST_TEST(fv.data()[3] == -4.0);
  BOOST_TEST(fv.back() == -11.0);
  BOOST_TEST(fv.flat()[7] == -8.0);
}

BOOST_AUTO_TEST_CASE(testViewFlatSpans) {
  const FlatNestedVec<double, 3> fv(raggedDepth3());
  BOOST_CHECK(std::ranges::equal(fv[0].flat(), std::vector<double>{1, 2, 3, 4}));
  BOOST_CHECK(fv[0].flat().data() == fv.data());
  BOOST_TEST(fv[1].flat().empty());
  BOOST_TEST(fv[3].flat().empty());
  BOOST_CHECK(std::ranges::equal(fv[4].flat(), std::vector<double>{7, 8, 9, 10, 11}));
  BOOST_CHECK(fv.nested().flat().data() == fv.data());
  BOOST_TEST(fv.nested().flat().size() == fv.size());

  // Subtrees without any entries at intermediate depths must not dereference offset arrays.
  const FlatNestedVec<int, 4> noDeeperEntries(NestedStdVec<int, 4>{{}, {}});
  BOOST_TEST(noDeeperEntries.nested().flat().empty());
  const FlatNestedVec<int, 4> noElements(NestedStdVec<int, 4>{{{}, {}}, {{}}});
  BOOST_TEST(noElements.nested().flat().empty());
  BOOST_TEST(noElements[0].flat().empty());
}

BOOST_AUTO_TEST_CASE(testViewIterators) {
  FlatNestedVec<int, 4> fv(raggedDepth4());
  const auto view = fv.nested();
  using Diff = std::ptrdiff_t;

  BOOST_TEST((view.end() - view.begin()) == static_cast<Diff>(view.size()));
  BOOST_TEST(std::ranges::distance(view) == static_cast<Diff>(view.size()));
  auto it = view.begin();
  it += 4;
  BOOST_CHECK(it[0].flat().data() == view[4].flat().data());
  BOOST_CHECK((*(it - 2)).flat().data() == view[2].flat().data());
  BOOST_CHECK(view.begin() < it && it < view.end() && it + 1 == view.end());
  BOOST_CHECK(it-- == view.begin() + 4);
  BOOST_CHECK(--it == 2 + view.begin());

  // Reverse traversal and range adaptors.
  std::vector<std::size_t> sizes;
  for (const auto child : view | std::views::reverse) sizes.push_back(child.size());
  BOOST_CHECK(sizes == (std::vector<std::size_t>{2, 2, 1, 0, 3}));
  const auto nonEmpty = std::ranges::count_if(view, [](auto child) { return !child.empty(); });
  BOOST_TEST(nonEmpty == 4);

  // A mutable view converts into a read-only view of the same sub-vector.
  const FlatNestedVecView<const int, 3, std::size_t> constView = fv[0];
  BOOST_CHECK(constView.flat().data() == fv.data());
  BOOST_TEST(constView[2][1][0] == 4);
}

BOOST_AUTO_TEST_CASE(testFlatInterfaceWithAlgorithms) {
  FlatNestedVec<double, 3> fv(raggedDepth3());
  BOOST_TEST(std::accumulate(fv.begin(), fv.end(), 0.0) == 66.0);
  BOOST_TEST(fv.front() == 1.0);
  BOOST_TEST(fv.back() == 11.0);
  BOOST_TEST(*fv.rbegin() == 11.0);
  std::ranges::transform(fv, fv.begin(), [](const double x) { return 2 * x; });
  BOOST_TEST(fv[4][1][3] == 22.0);
  std::ranges::fill(fv[4].flat(), 0.0);
  BOOST_TEST(std::accumulate(fv.cbegin(), fv.cend(), 0.0) == 2 * (1 + 2 + 3 + 4 + 5 + 6));
}

/*************************************************************
Incremental building
*************************************************************/

BOOST_AUTO_TEST_CASE(testIncrementalBuilding) {
  FlatNestedVec<double, 3> fv;
  NestedStdVec<double, 3> nested;

  fv.appendSubVec(1);
  nested.emplace_back();
  fv.appendSubVec(2);
  nested.back().emplace_back();
  fv.push_back(1.5);
  nested.back().back().push_back(1.5);
  fv.appendSubVec(2, 4);
  nested.back().emplace_back(4);
  fv.appendSubVec(1, 3);
  nested.emplace_back(3);
  const std::vector<double> range{2.5, 3.5};
  fv.append_range(range);
  nested.back().back().insert(nested.back().back().end(), range.begin(), range.end());
  fv.append_range(std::views::iota(1, 4));  // not a sized common range of double
  nested.back().back().insert(nested.back().back().end(), {1.0, 2.0, 3.0});
  BOOST_TEST(fv.emplace_back(4.5) == 4.5);
  nested.back().back().emplace_back(4.5);
  fv.appendSubVec(1);
  nested.emplace_back();

  checkRepresents(fv, nested);
}

BOOST_AUTO_TEST_CASE(testPopBackKeepsTrailingEmptySubVecs) {
  NestedStdVec<double, 3> nested{{{1, 2}, {3}, {}}, {{}}, {}};
  FlatNestedVec<double, 3> fv(nested);
  fv.pop_back();
  nested[0][1].pop_back();
  checkRepresents(fv, nested);
  fv.pop_back();
  nested[0][0].pop_back();
  checkRepresents(fv, nested);
}

BOOST_AUTO_TEST_CASE(testIncrementalBuildingFuzz) {
  std::mt19937 rng(42);
  for (int run = 0; run < 50; run++) {
    FlatNestedVec<double, 3, std::uint32_t> fv;
    NestedStdVec<double, 3> nested;
    for (int op = 0; op < 200; op++) {
      const auto count = static_cast<std::size_t>(rng() % 4);
      const double value = static_cast<double>(rng() % 1000);
      switch (rng() % 6) {
        case 0:
          fv.appendSubVec(1, count);
          nested.emplace_back(count);
          break;
        case 1:
          if (nested.empty()) break;
          fv.appendSubVec(2, count);
          nested.back().emplace_back(count);
          break;
        case 2:
          if (nested.empty() || nested.back().empty()) break;
          fv.push_back(value);
          nested.back().back().push_back(value);
          break;
        case 3: {
          if (nested.empty() || nested.back().empty()) break;
          const std::vector<double> range(count, value);
          fv.append_range(range);
          nested.back().back().insert(nested.back().back().end(), range.begin(), range.end());
          break;
        }
        case 4: {
          double* last = lastElement(nested);
          if (last == nullptr) break;
          fv.pop_back();
          for (auto& sub1 : nested) {
            for (auto& sub2 : sub1) {
              if (!sub2.empty() && &sub2.back() == last) sub2.pop_back();
            }
          }
          break;
        }
        default:
          if (!fv.empty()) fv[rng() % fv.nEntries(1)].flat();  // must not crash
          break;
      }
    }
    checkRepresents(fv, nested);
  }
}

/*************************************************************
Value semantics
*************************************************************/

BOOST_AUTO_TEST_CASE(testCopyMoveSwapClear) {
  const auto nested = raggedDepth3();
  FlatNestedVec<double, 3> a(nested);

  FlatNestedVec<double, 3> copy(a);
  BOOST_CHECK(copy == a);
  copy[0][0][0] = -1.0;
  BOOST_CHECK(copy != a);
  BOOST_TEST(a[0][0][0] == 1.0);

  FlatNestedVec<double, 3> moved(std::move(a));
  checkRepresents(moved, nested);
  // A moved-from container is empty and usable.
  BOOST_TEST(a.empty());
  BOOST_TEST(a.nEntries(1) == 0U);
  a.appendSubVec(1, 1);
  a.push_back(3.0);
  BOOST_CHECK(a.toNested() == (NestedStdVec<double, 3>{{{3.0}}}));

  swap(a, moved);
  checkRepresents(a, nested);
  BOOST_CHECK(moved.toNested() == (NestedStdVec<double, 3>{{{3.0}}}));

  a.clear();
  BOOST_TEST(a.empty());
  BOOST_TEST(a.nEntries(1) == 0U);
  BOOST_CHECK((a == FlatNestedVec<double, 3>()));
}

BOOST_AUTO_TEST_CASE(testEqualityComparesNesting) {
  // Same elements, different nesting.
  const FlatNestedVec<int, 2> a(NestedStdVec<int, 2>{{1, 2}, {3}});
  const FlatNestedVec<int, 2> b(NestedStdVec<int, 2>{{1}, {2, 3}});
  BOOST_CHECK(a != b);
  // Same nesting without elements, built differently.
  FlatNestedVec<int, 3> c;
  c.appendSubVec(1);
  c.appendSubVec(1);
  BOOST_CHECK((c == FlatNestedVec<int, 3>(NestedStdVec<int, 3>{{}, {}})));
  BOOST_CHECK((c != FlatNestedVec<int, 3>(NestedStdVec<int, 3>{{}, {{}}})));
}

BOOST_AUTO_TEST_SUITE_END()
