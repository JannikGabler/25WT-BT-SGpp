/**
 * @file vector_map.hpp
 * @brief Linear-search associative container backed by @c std::vector.
 */
#ifndef COMBIGRID_MISC_VECTOR_MAP_HPP
#define COMBIGRID_MISC_VECTOR_MAP_HPP

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Tiny associative container backed by a @c std::vector.
 *
 * Provides a subset of the @c std::map interface (lookup, insert,
 * erase, iteration). All lookups are linear scans, so this map is only
 * suitable for very few keys but offers cache-friendly iteration and
 * negligible per-element overhead.
 *
 * @tparam KeyType   Key type (must be equality-comparable).
 * @tparam ValueType Mapped type.
 * @tparam Allocator Optional allocator for the underlying vector.
 */
template <typename KeyType, typename ValueType,
          class Allocator = std::allocator<std::pair<KeyType, ValueType>>>
class VecMap {
 public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<key_type, mapped_type>;
  using container_type = std::vector<value_type, Allocator>;
  using size_type = typename container_type::size_type;

  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;

  /***********
  Constructors
  ***********/
  /// @brief Constructs an empty map.
  VecMap() = default;

  /**
   * @brief Constructs an empty map with capacity reserved for
   * @p reserve_n entries.
   */
  explicit VecMap(size_type reserve_n) { data_.reserve(reserve_n); }

  /**
   * @brief Constructs a map from a brace-enclosed list of @c (key, value)
   * pairs. Duplicate keys are ignored (first occurrence wins).
   */
  VecMap(std::initializer_list<value_type> init) {
    data_.reserve(init.size());
    for (const auto& v : init) insert(v);
  }

  /****************
  Size and capacity
  ****************/
  /// @brief Returns @c true iff the map is empty.
  bool empty() const noexcept { return data_.empty(); }

  /// @brief Returns the number of stored key-value pairs.
  size_type size() const noexcept { return data_.size(); }

  /// @brief Removes all stored entries.
  void clear() noexcept { data_.clear(); }

  /// @brief Reserves capacity for at least @p n entries.
  void reserve(size_type n) { data_.reserve(n); }

  /*******
  Iterators
  ********/
  iterator begin() noexcept { return data_.begin(); }
  iterator end() noexcept { return data_.end(); }

  const_iterator begin() const noexcept { return data_.begin(); }
  const_iterator end() const noexcept { return data_.end(); }

  const_iterator cbegin() const noexcept { return data_.cbegin(); }
  const_iterator cend() const noexcept { return data_.cend(); }

  /*****
  Lookup
  *****/
  /// @brief Finds @p key (linear search) and returns an iterator to it,
  /// or @ref end() if not found.
  iterator find(const key_type& key) { return find_it(key); }

  /// @copydoc find(const key_type&)
  const_iterator find(const key_type& key) const { return find_it(key); }

  /// @brief Returns @c true iff @p key is present.
  bool contains(const key_type& key) const { return find_it(key) != data_.end(); }

  /**
   * @brief Returns a reference to the mapped value of @p key.
   * @throws std::out_of_range if @p key is not present.
   */
  mapped_type& at(const key_type& key) {
    auto it = find_it(key);
    if (it == data_.end()) throw std::out_of_range("VecMap::at: key not found");
    return it->second;
  }

  /// @copydoc at(const key_type&)
  const mapped_type& at(const key_type& key) const {
    auto it = find_it(key);
    if (it == data_.end()) throw std::out_of_range("VecMap::at: key not found");
    return it->second;
  }

  /*************
  Element access
  *************/
  /**
   * @brief Positional access to the underlying storage.
   * @param idx Position into the underlying vector
   * (insertion order).
   */
  value_type& operator[](const size_t idx) {
    // auto it = find_it(key);
    // if (it != data_.end()) return it->second;

    // data_.emplace_back(key, mapped_type{});
    // return data_.back().second;
    return data_[idx];
  }

  /// @copydoc operator[](const size_t)
  const value_type& operator[](const size_t idx) const { return data_[idx]; }

  // mapped_type& operator[](key_type&& key) {
  //   auto it = find_it(key);
  //   if (it != data_.end()) return it->second;

  //   data_.emplace_back(std::move(key), mapped_type{});
  //   return data_.back().second;
  // }

  /********
  Modifiers
  ********/
  /**
   * @brief Inserts @p value if its key is not already present.
   * @return Iterator to the (possibly existing) entry and a flag that
   * is @c true iff a new entry was created.
   */
  std::pair<iterator, bool> insert(const value_type& value) {
    auto it = find_it(value.first);
    if (it != data_.end()) return {it, false};

    data_.push_back(value);
    return {std::prev(data_.end()), true};
  }

  std::pair<iterator, bool> insert(value_type&& value) {
    auto it = find_it(value.first);
    if (it != data_.end()) return {it, false};

    data_.push_back(std::move(value));
    return {std::prev(data_.end()), true};
  }

  template <class M>
  std::pair<iterator, bool> insert_or_assign(const key_type& key, M&& obj) {
    auto it = find_it(key);
    if (it != data_.end()) {
      it->second = std::forward<M>(obj);
      return {it, false};
    }
    data_.emplace_back(key, std::forward<M>(obj));
    return {std::prev(data_.end()), true};
  }

  size_type erase(const key_type& key) {
    auto it = find_it(key);
    if (it == data_.end()) return 0;

    data_.erase(it);
    return 1;
  }

 private:
  container_type data_;

  iterator find_it(const key_type& key) {
    return std::find_if(data_.begin(), data_.end(),
                        [&](const value_type& p) { return p.first == key; });
  }

  const_iterator find_it(const key_type& key) const {
    return std::find_if(data_.begin(), data_.end(),
                        [&](const value_type& p) { return p.first == key; });
  }
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif