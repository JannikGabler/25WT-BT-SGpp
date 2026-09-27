// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_algorithm_test.cpp
 * @brief Boost.Test cases running standard algorithms on @c MIVec.
 *
 * Every algorithm is applied to an @c MIVec and to a @c std::vector<MI>
 * holding the same multi-indices. Both containers have random-access
 * iterators, so the standard library executes the same sequence of
 * operations on them; the results (including returned positions and the
 * order of equivalent elements) must therefore be identical. This verifies
 * that the proxy references of @c MIVec behave like true references.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <random>
#include <ranges>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_lexicographic_less.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;

namespace {

using LvlMIView = MIView<LvlType>;
using RefVec = std::vector<LvlMI>;

sgpp::base::RandomNumberGenerator& randGen = sgpp::base::RandomNumberGenerator::getInstance();

/// (nDim, nMI, maxValue): small value ranges produce many duplicates and ties.
const size_t CONFIGS[][3] = {{1, 0, 3},   {1, 1, 3},  {1, 60, 3},  {2, 2, 1},  {3, 17, 2},
                             {3, 500, 3}, {4, 40, 1}, {7, 300, 1}, {7, 64, 5}, {32, 50, 2}};
const size_t N_SEEDS = 3;

LvlMIVec randomMIVec(const size_t nDim, const size_t nMI, const size_t maxValue) {
  LvlMIVec vec(nDim, nMI);
  for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
    for (size_t dim = 0; dim < nDim; dim++) {
      vec(miIdx, dim) = static_cast<LvlType>(randGen.getUniformIndexRN(maxValue + 1));
    }
  }
  return vec;
}

RefVec toReference(const LvlMIVec& vec) {
  RefVec reference;
  for (const LvlMIView mi : vec) {
    reference.push_back(mi);
  }
  return reference;
}

bool equalsReference(const LvlMIVec& vec, const RefVec& reference) {
  if (vec.nMI() != reference.size()) return false;
  for (size_t i = 0; i < vec.nMI(); i++) {
    if (vec[i] != reference[i]) return false;
  }
  return true;
}

/// Runs @p algorithm on random MIVecs and on std::vector<MI> copies and compares the results.
template <typename Algorithm>
void checkAgainstReference(const Algorithm& algorithm) {
  for (const size_t* config : CONFIGS) {
    for (size_t seed = 0; seed < N_SEEDS; seed++) {
      randGen.setSeed(static_cast<sgpp::base::RandomNumberGenerator::SeedType>(1000 + seed));
      LvlMIVec vec = randomMIVec(config[0], config[1], config[2]);
      RefVec reference = toReference(vec);

      const std::ptrdiff_t vecResult = algorithm(vec);
      const std::ptrdiff_t refResult = algorithm(reference);

      BOOST_TEST_CONTEXT("nDim=" << config[0] << " nMI=" << config[1] << " maxValue=" << config[2]
                                 << " seed=" << seed) {
        BOOST_CHECK(equalsReference(vec, reference));
        BOOST_TEST(vecResult == refResult);
      }
    }
  }
}

/// Predicates and comparators taking MIView accept MI, MIView and MIRef arguments.
struct FirstEntryLess {
  bool operator()(const LvlMIView a, const LvlMIView b) const { return a[0] < b[0]; }
};
struct FirstEntryEqual {
  bool operator()(const LvlMIView a, const LvlMIView b) const { return a[0] == b[0]; }
};
struct SumIsEven {
  bool operator()(const LvlMIView mi) const { return mi.sumOfElems() % 2 == 0; }
};
struct IncrementFirst {
  LvlMI operator()(const LvlMIView mi) const {
    LvlMI result = mi;
    result[0]++;
    return result;
  }
};

/// Distance of @p it from the begin of @p c (for comparing returned iterators).
template <typename C, typename It>
std::ptrdiff_t pos(C& c, const It it) {
  return std::distance(c.begin(), it);
}

/// Combines several results into one value (unsigned arithmetic, so overflow is well-defined).
std::ptrdiff_t mix(const std::ptrdiff_t accumulated, const std::ptrdiff_t value) {
  return static_cast<std::ptrdiff_t>(31u * static_cast<size_t>(accumulated) +
                                     static_cast<size_t>(value));
}

/*********************
Mutating algorithms
*********************/
struct Sort {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::sort(c.begin(), c.end(), MILexLess());
    return std::is_sorted(c.begin(), c.end(), MILexLess());
  }
};
struct StableSort {  // Compares only the first entry, so stability is observable.
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::stable_sort(c.begin(), c.end(), FirstEntryLess());
    return 0;
  }
};
struct Partition {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    return pos(c, std::partition(c.begin(), c.end(), SumIsEven()));
  }
};
struct StablePartition {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    return pos(c, std::stable_partition(c.begin(), c.end(), SumIsEven()));
  }
};
struct RemoveIfErase {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    c.erase(std::remove_if(c.begin(), c.end(), SumIsEven()), c.end());
    return static_cast<std::ptrdiff_t>(c.size());
  }
};
struct SortUniqueErase {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::sort(c.begin(), c.end(), MILexLess());
    c.erase(std::unique(c.begin(), c.end()), c.end());
    return static_cast<std::ptrdiff_t>(c.size());
  }
};
struct UniqueWithPredicate {  // Elements past the returned end are unspecified: erase them.
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    const std::ptrdiff_t newSize = pos(c, std::unique(c.begin(), c.end(), FirstEntryEqual()));
    c.erase(c.begin() + newSize, c.end());
    return newSize;
  }
};
struct Rotate {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::ptrdiff_t result = 0;
    const std::ptrdiff_t n = static_cast<std::ptrdiff_t>(c.size());
    for (std::ptrdiff_t k = 0; k <= n; k += (n / 5) + 1) {
      result += pos(c, std::rotate(c.begin(), c.begin() + k, c.end()));
    }
    return result;
  }
};
struct ReverseAndSwapRanges {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::reverse(c.begin(), c.end());
    const std::ptrdiff_t half = static_cast<std::ptrdiff_t>(c.size()) / 2;
    std::swap_ranges(c.begin(), c.begin() + half, c.begin() + half);
    return 0;
  }
};
struct Shuffle {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::mt19937 generator(4711);
    std::shuffle(c.begin(), c.end(), generator);
    return 0;
  }
};
struct NthElementPartialSort {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    const std::ptrdiff_t n = static_cast<std::ptrdiff_t>(c.size());
    std::nth_element(c.begin(), c.begin() + n / 2, c.end(), MILexLess());
    std::partial_sort(c.begin(), c.begin() + n / 3, c.end(), MILexLess());
    return 0;
  }
};
struct InplaceMerge {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    const std::ptrdiff_t half = static_cast<std::ptrdiff_t>(c.size()) / 2;
    std::sort(c.begin(), c.begin() + half, MILexLess());
    std::sort(c.begin() + half, c.end(), MILexLess());
    std::inplace_merge(c.begin(), c.begin() + half, c.end(), MILexLess());
    return std::is_sorted(c.begin(), c.end(), MILexLess());
  }
};
struct Heap {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::make_heap(c.begin(), c.end(), MILexLess());
    const bool isHeap = std::is_heap(c.begin(), c.end(), MILexLess());
    std::sort_heap(c.begin(), c.end(), MILexLess());
    return isHeap;
  }
};
struct NextPermutation {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::ptrdiff_t result = 0;
    for (size_t i = 0; i < 5; i++) {
      result = 2 * result + std::next_permutation(c.begin(), c.end(), MILexLess());
    }
    return result;
  }
};
struct TransformFillCopyBackward {
  template <typename C>
  std::ptrdiff_t operator()(C& c) const {
    std::transform(c.begin(), c.end(), c.begin(), IncrementFirst());
    if (c.size() >= 2) {
      std::copy_backward(c.begin(), c.end() - 1, c.end());  // Overlapping shift by one.
      const LvlMI filler(c.begin()[0].size(), 7);
      std::fill(c.begin(), c.begin() + 1, filler);
    }
    return 0;
  }
};

/*************************
Non-mutating algorithms
*************************/
struct Searches {
  template <typename C>
  std::ptrdiff_t operator()(C& mutableC) const {
    const C& c = mutableC;  // Exercises the const iterators.
    std::ptrdiff_t result = pos(c, std::find_if(c.begin(), c.end(), SumIsEven()));
    result = mix(result, std::count_if(c.begin(), c.end(), SumIsEven()));
    result = mix(result, std::all_of(c.begin(), c.end(), SumIsEven()));
    result = mix(result, std::any_of(c.begin(), c.end(), SumIsEven()));
    result = mix(result, std::none_of(c.begin(), c.end(), SumIsEven()));
    result = mix(result, pos(c, std::adjacent_find(c.begin(), c.end())));
    result = mix(result, pos(c, std::min_element(c.begin(), c.end(), MILexLess())));
    result = mix(result, pos(c, std::max_element(c.begin(), c.end(), MILexLess())));
    result = mix(result, std::is_partitioned(c.begin(), c.end(), SumIsEven()));
    if (!c.empty()) {
      const LvlMI needle = c.begin()[static_cast<std::ptrdiff_t>(c.size()) / 2];
      result = mix(result, pos(c, std::find(c.begin(), c.end(), needle)));
      result = mix(result, std::count(c.begin(), c.end(), needle));
    }
    return result;
  }
};
struct SortedSearches {
  template <typename C>
  std::ptrdiff_t operator()(C& mutableC) const {
    std::sort(mutableC.begin(), mutableC.end(), MILexLess());
    const C& c = mutableC;
    std::ptrdiff_t result = 0;
    const size_t nDim = c.empty() ? 1 : c.begin()[0].size();

    for (size_t probe = 0; probe < 20; probe++) {
      LvlMI value(nDim);
      for (size_t dim = 0; dim < nDim; dim++) {
        value[dim] = static_cast<LvlType>((probe * (dim + 3)) % 4);
      }
      result = mix(result, pos(c, std::lower_bound(c.begin(), c.end(), value, MILexLess())));
      result = mix(result, pos(c, std::upper_bound(c.begin(), c.end(), value, MILexLess())));
      result = mix(result, std::binary_search(c.begin(), c.end(), value, MILexLess()));
      result = mix(result, pos(c, std::equal_range(c.begin(), c.end(), value, MILexLess()).second));
    }
    return result;
  }
};

}  // namespace

BOOST_AUTO_TEST_SUITE(MIVec_algorithms)

BOOST_AUTO_TEST_CASE(sort) { checkAgainstReference(Sort()); }
BOOST_AUTO_TEST_CASE(stable_sort) { checkAgainstReference(StableSort()); }
BOOST_AUTO_TEST_CASE(partition) { checkAgainstReference(Partition()); }
BOOST_AUTO_TEST_CASE(stable_partition) { checkAgainstReference(StablePartition()); }
BOOST_AUTO_TEST_CASE(remove_if_erase) { checkAgainstReference(RemoveIfErase()); }
BOOST_AUTO_TEST_CASE(unique_erase) { checkAgainstReference(SortUniqueErase()); }
BOOST_AUTO_TEST_CASE(unique_with_predicate) { checkAgainstReference(UniqueWithPredicate()); }
BOOST_AUTO_TEST_CASE(rotate) { checkAgainstReference(Rotate()); }
BOOST_AUTO_TEST_CASE(reverse_swap_ranges) { checkAgainstReference(ReverseAndSwapRanges()); }
BOOST_AUTO_TEST_CASE(shuffle) { checkAgainstReference(Shuffle()); }
BOOST_AUTO_TEST_CASE(nth_element_partial_sort) { checkAgainstReference(NthElementPartialSort()); }
BOOST_AUTO_TEST_CASE(inplace_merge) { checkAgainstReference(InplaceMerge()); }
BOOST_AUTO_TEST_CASE(heap) { checkAgainstReference(Heap()); }
BOOST_AUTO_TEST_CASE(next_permutation) { checkAgainstReference(NextPermutation()); }
BOOST_AUTO_TEST_CASE(transform_fill_copy_backward) {
  checkAgainstReference(TransformFillCopyBackward());
}
BOOST_AUTO_TEST_CASE(searches) { checkAgainstReference(Searches()); }
BOOST_AUTO_TEST_CASE(sorted_searches) { checkAgainstReference(SortedSearches()); }

BOOST_AUTO_TEST_CASE(copy_between_container_types) {
  randGen.setSeed(5);
  const LvlMIVec source = randomMIVec(4, 100, 9);
  const RefVec reference = toReference(source);

  LvlMIVec toMIVec(4, source.nMI());
  std::copy(source.begin(), source.end(), toMIVec.begin());  // MIView -> MIRef
  BOOST_CHECK(equalsReference(toMIVec, reference));

  RefVec toStdVector(source.nMI(), LvlMI(4));
  std::copy(source.begin(), source.end(), toStdVector.begin());  // MIView -> MI&
  BOOST_CHECK(toStdVector == reference);

  LvlMIVec fromStdVector(4, reference.size());
  std::copy(reference.begin(), reference.end(), fromStdVector.begin());  // MI& -> MIRef
  BOOST_CHECK(equalsReference(fromStdVector, reference));

  LvlMIVec moved(4, source.nMI());
  std::move(toMIVec.begin(), toMIVec.end(), moved.begin());
  BOOST_CHECK(equalsReference(moved, reference));
  BOOST_CHECK(std::equal(moved.begin(), moved.end(), reference.begin()));
  BOOST_CHECK(std::mismatch(moved.begin(), moved.end(), reference.begin()).first == moved.end());
}

BOOST_AUTO_TEST_CASE(hand_written_cases) {
  const LvlMIVec input{{2, 1}, {0, 3}, {1, 1}, {0, 0}};

  LvlMIVec sorted(input);
  std::sort(sorted.begin(), sorted.end(), MILexLess());
  BOOST_CHECK(equalsReference(sorted, RefVec{LvlMI{0, 0}, LvlMI{0, 3}, LvlMI{1, 1}, LvlMI{2, 1}}));

  LvlMIVec removed(input);
  removed.erase(
      std::remove_if(removed.begin(), removed.end(), [](const LvlMIView mi) { return mi[0] == 0; }),
      removed.end());
  BOOST_CHECK(equalsReference(removed, RefVec{LvlMI{2, 1}, LvlMI{1, 1}}));

  LvlMIVec rotated(input);
  LvlMIVec::iterator newFirst = std::rotate(rotated.begin(), rotated.begin() + 1, rotated.end());
  BOOST_TEST(newFirst.index() == 3);
  BOOST_CHECK(equalsReference(rotated, RefVec{LvlMI{0, 3}, LvlMI{1, 1}, LvlMI{0, 0}, LvlMI{2, 1}}));

  LvlMIVec duplicates{{1, 1}, {1, 1}, {0, 2}, {0, 2}, {1, 1}};
  duplicates.erase(std::unique(duplicates.begin(), duplicates.end()), duplicates.end());
  BOOST_CHECK(equalsReference(duplicates, RefVec{LvlMI{1, 1}, LvlMI{0, 2}, LvlMI{1, 1}}));

  LvlMIVec partitioned(input);
  LvlMIVec::iterator partitionPoint = std::stable_partition(
      partitioned.begin(), partitioned.end(), [](const LvlMIView mi) { return mi[0] % 2 == 0; });
  BOOST_TEST(partitionPoint.index() == 3);
  BOOST_CHECK(
      equalsReference(partitioned, RefVec{LvlMI{2, 1}, LvlMI{0, 3}, LvlMI{0, 0}, LvlMI{1, 1}}));

  const LvlMIVec& constSorted = sorted;
  BOOST_TEST(std::binary_search(constSorted.begin(), constSorted.end(), LvlMI{1, 1}, MILexLess()));
  BOOST_TEST(!std::binary_search(constSorted.begin(), constSorted.end(), LvlMI{1, 2}, MILexLess()));
  BOOST_TEST(
      std::lower_bound(constSorted.begin(), constSorted.end(), LvlMI{0, 2}, MILexLess()).index() ==
      1);
  BOOST_TEST(std::count_if(constSorted.begin(), constSorted.end(),
                           [](const LvlMIView mi) { return mi <= LvlMI{1, 3}; }) == 3);
}

BOOST_AUTO_TEST_SUITE_END()

#if __cplusplus >= 202002L && defined(__cpp_lib_ranges)
BOOST_AUTO_TEST_SUITE(MIVec_ranges_algorithms)

BOOST_AUTO_TEST_CASE(ranges_mutating) {
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    std::ranges::sort(c, MILexLess{});
    return std::ranges::is_sorted(c, MILexLess{});
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    std::ranges::stable_sort(c, FirstEntryLess{});
    return 0;
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {  // Projection onto the first entry.
    std::ranges::stable_sort(c, std::ranges::greater{}, [](const LvlMIView mi) { return mi[0]; });
    return 0;
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    return pos(c, std::ranges::partition(c, SumIsEven{}).begin());
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    return pos(c, std::ranges::stable_partition(c, SumIsEven{}).begin());
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    const auto removed = std::ranges::remove_if(c, SumIsEven{});
    c.erase(removed.begin(), removed.end());
    return std::ranges::ssize(c);
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    std::ranges::sort(c, MILexLess{});
    const auto duplicates = std::ranges::unique(c);
    c.erase(duplicates.begin(), duplicates.end());
    return std::ranges::ssize(c);
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    const std::ptrdiff_t k = std::ranges::ssize(c) / 3;
    return pos(c, std::ranges::rotate(c, c.begin() + k).begin());
  });
  checkAgainstReference([](auto& c) -> std::ptrdiff_t {
    std::ranges::reverse(c);
    std::mt19937 generator(17);
    std::ranges::shuffle(c, generator);
    return 0;
  });
}

BOOST_AUTO_TEST_CASE(ranges_non_mutating) {
  checkAgainstReference([](auto& mutableC) -> std::ptrdiff_t {
    std::ranges::sort(mutableC, MILexLess{});
    const auto& c = mutableC;
    std::ptrdiff_t result = pos(c, std::ranges::find_if(c, SumIsEven{}));
    result = mix(result, std::ranges::count_if(c, SumIsEven{}));
    result = mix(result, std::ranges::all_of(c, SumIsEven{}));
    result = mix(result, pos(c, std::ranges::min_element(c, MILexLess{})));
    if (!c.empty()) {
      const LvlMI value = c.begin()[std::ranges::ssize(c) / 2];
      result = mix(result, pos(c, std::ranges::lower_bound(c, value, MILexLess{})));
      result = mix(result, std::ranges::binary_search(c, value, MILexLess{}));
      result = mix(result, pos(c, std::ranges::find(c, value)));
    }
    return result;
  });
}

BOOST_AUTO_TEST_CASE(ranges_views) {
  const LvlMIVec vec{{2, 1}, {0, 3}, {1, 1}, {0, 0}, {4, 4}};

  size_t sumOfFirstEntries = 0;
  for (const LvlMIView mi : vec | std::views::filter(SumIsEven{}) | std::views::reverse) {
    sumOfFirstEntries = 10 * sumOfFirstEntries + mi[0];
  }
  BOOST_TEST(sumOfFirstEntries == 401u);  // {4,4}, {0,0}, {1,1}

  const auto sums = vec | std::views::take(3) |
                    std::views::transform([](const LvlMIView mi) { return mi.sumOfElems(); });
  BOOST_CHECK(std::ranges::equal(sums, std::vector<LvlType>{3, 3, 2}));

  const RefVec reference = toReference(vec);
  BOOST_CHECK(std::ranges::equal(vec, reference));
}

BOOST_AUTO_TEST_SUITE_END()
#endif
