/**
 * @file kway_unique_merge_sorted_collections.hpp
 * @brief K-way merge of sorted ranges that removes duplicates on the fly.
 */
#include <functional>
#include <iterator>
#include <queue>
#include <vector>

namespace sgpp {

namespace combigrid {

namespace tools {

/**
 * @brief Merges @c k sorted iterator ranges into a single sorted output,
 * de-duplicating equal consecutive values.
 *
 * Implemented with a binary min-heap (priority queue) of size @c k, so
 * the total work is @f$O(N \log k)@f$ where @c N is the combined size of
 * the input ranges.
 *
 * @tparam Iterator Forward-iterator type of the input ranges.
 * @tparam Compare  Strict weak ordering matching the sorted input
 *                  (defaults to @c std::less).
 * @param ranges    Pairs of @c (begin, end) iterators describing each
 *                  sorted input collection.
 * @param comp      Comparator instance.
 * @return Newly allocated vector with the merged, de-duplicated values.
 */
template <typename Iterator,
          typename Compare = std::less<typename std::iterator_traits<Iterator>::value_type>>
std::vector<typename std::iterator_traits<Iterator>::value_type>
kway_unique_merge_sorted_collections(const std::vector<std::pair<Iterator, Iterator>>& ranges,
                                     Compare comp = Compare{}) {
  using T = typename std::iterator_traits<Iterator>::value_type;

  struct Node {
    T value;
    std::size_t range;
    Iterator it;
  };

  struct HeapCmp {
    Compare comp;
    bool operator()(Node const& a, Node const& b) const {
      return comp(b.value, a.value);  // min-heap
    }
  };

  std::priority_queue<Node, std::vector<Node>, HeapCmp> heap{HeapCmp{comp}};

  // Initialisierung
  for (std::size_t i = 0; i < ranges.size(); ++i) {
    if (ranges[i].first != ranges[i].second) {
      heap.push(Node{*ranges[i].first, i, ranges[i].first});
    }
  }

  std::vector<T> out;
  bool have_last = false;
  T last{};

  while (!heap.empty()) {
    Node n = heap.top();
    heap.pop();

    if (!have_last || comp(last, n.value) || comp(n.value, last)) {
      out.push_back(n.value);
      last = n.value;
      have_last = true;
    }

    ++n.it;
    if (n.it != ranges[n.range].second) {
      heap.push(Node{*n.it, n.range, n.it});
    }
  }

  return out;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp