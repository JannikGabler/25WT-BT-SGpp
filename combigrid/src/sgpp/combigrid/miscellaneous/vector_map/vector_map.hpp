#ifndef COMBIGRID_MISC_VECTOR_MAP_HPP
#define COMBIGRID_MISC_VECTOR_MAP_HPP

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

/*
Only use for small number of keys.
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
  VecMap() = default;

  explicit VecMap(size_type reserve_n) { data_.reserve(reserve_n); }

  VecMap(std::initializer_list<value_type> init) {
    data_.reserve(init.size());
    for (const auto& v : init) insert(v);
  }

  /****************
  Size and capacity
  ****************/
  bool empty() const noexcept { return data_.empty(); }

  size_type size() const noexcept { return data_.size(); }

  void clear() noexcept { data_.clear(); }

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
  iterator find(const key_type& key) { return find_it(key); }

  const_iterator find(const key_type& key) const { return find_it(key); }

  bool contains(const key_type& key) const { return find_it(key) != data_.end(); }

  mapped_type& at(const key_type& key) {
    auto it = find_it(key);
    if (it == data_.end()) throw std::out_of_range("VecMap::at: key not found");
    return it->second;
  }

  const mapped_type& at(const key_type& key) const {
    auto it = find_it(key);
    if (it == data_.end()) throw std::out_of_range("VecMap::at: key not found");
    return it->second;
  }

  /*************
  Element access
  *************/
  value_type& operator[](const size_t idx) {
    // auto it = find_it(key);
    // if (it != data_.end()) return it->second;

    // data_.emplace_back(key, mapped_type{});
    // return data_.back().second;
    return data_[idx];
  }

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