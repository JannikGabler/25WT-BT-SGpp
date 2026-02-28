#ifndef COMBIGRID_MULTIINDEX_HPP
#define COMBIGRID_MULTIINDEX_HPP

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MI {
 public:
  using value_type = T;
  using allocator_type = std::allocator<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;
  using reverse_iterator = typename std::vector<T>::reverse_iterator;
  using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

  /* =========================
     Konstruktoren
     ========================= */
  MI() = default;

  explicit MI(size_type count) : data_(count) {}

  MI(size_type count, value_type value) : data_(count, value) {}

  MI(std::initializer_list<value_type> init) : data_(init) {}

  MI(std::vector<value_type>& data) : data_(data) {}

  template <class InputIt>
  MI(InputIt first, InputIt last) : data_(first, last) {}

  /* =========================
   Implicit convertion
   ========================= */
  operator std::vector<value_type>&() noexcept { return data_; }

  operator const std::vector<value_type>&() const noexcept { return data_; }

  /* =========================
   Additional methods
   ========================= */
  size_t toLinearIndex() const {
    const size_t nDim = this->size();
    size_t idx = 0;
    for (size_t dim = 0; dim < nDim; dim++) {
      idx *= nDim;
      idx += static_cast<size_t>((*this)[dim]);
    }
    return idx;
  }

  size_t toLinearIndex(const MI<T> nPerDim) const {
    if (nDim() == 0) {
      return 0;
    }

    size_t idx = static_cast<size_t>(data_[0]);

    for (size_t dim = 1; dim < nDim(); dim++) {
      idx *= nPerDim[dim - 1];
      idx += static_cast<size_t>((*this)[dim]);
    }
    return idx;
  }

  value_type productofElems() const {
    const size_t nDim = this->size();
    if (nDim == 0) {
      return 0;
    } else {
      value_type product = 1;
      for (size_t dim = 0; dim < nDim; dim++) {
        product *= static_cast<value_type>((*this)[dim]);
      }
      return product;
    }
  }

  value_type sumOfElems() const {
    value_type sum = 0;
    for (const value_type v : *this) {
      sum += v;
    }
    return sum;
  }

  size_t nDim() const { return this->size(); }

  /* =========================
     Elementzugriff
     ========================= */
  reference operator[](size_type pos) { return data_[pos]; }
  const_reference operator[](size_type pos) const { return data_[pos]; }

  reference at(size_type pos) { return data_.at(pos); }
  const_reference at(size_type pos) const { return data_.at(pos); }

  reference front() { return data_.front(); }
  const_reference front() const { return data_.front(); }

  reference back() { return data_.back(); }
  const_reference back() const { return data_.back(); }

  pointer data() noexcept { return data_.data(); }
  const_pointer data() const noexcept { return data_.data(); }

  /* =========================
     Iteratoren
     ========================= */
  iterator begin() noexcept { return data_.begin(); }
  const_iterator begin() const noexcept { return data_.begin(); }
  const_iterator cbegin() const noexcept { return data_.cbegin(); }

  iterator end() noexcept { return data_.end(); }
  const_iterator end() const noexcept { return data_.end(); }
  const_iterator cend() const noexcept { return data_.cend(); }

  reverse_iterator rbegin() noexcept { return data_.rbegin(); }
  const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }

  reverse_iterator rend() noexcept { return data_.rend(); }
  const_reverse_iterator rend() const noexcept { return data_.rend(); }

  /* =========================
     Kapazität
     ========================= */
  bool empty() const noexcept { return data_.empty(); }
  size_type size() const noexcept { return data_.size(); }
  size_type capacity() const noexcept { return data_.capacity(); }

  void reserve(size_type new_cap) { data_.reserve(new_cap); }
  void shrink_to_fit() { data_.shrink_to_fit(); }

  /* =========================
     Modifikatoren
     ========================= */
  void clear() noexcept { data_.clear(); }

  void push_back(value_type value) { data_.push_back(value); }

  void pop_back() { data_.pop_back(); }

  iterator insert(const_iterator pos, value_type value) { return data_.insert(pos, value); }

  iterator erase(const_iterator pos) { return data_.erase(pos); }

  iterator erase(const_iterator first, const_iterator last) { return data_.erase(first, last); }

  void resize(size_type count) { data_.resize(count); }

  void resize(size_type count, value_type value) { data_.resize(count, value); }

  void swap(MI& other) noexcept { data_.swap(other.data_); }

  /* =========================
     Vergleichsoperatoren
     ========================= */
  friend bool operator==(const MI& a, const MI& b) { return cmpVecForEqual(a.data_, b.data_); }
  friend bool operator==(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForEqual(a.data_, b);
  }
  friend bool operator==(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForEqual(a, b.data_);
  }

  friend bool operator!=(const MI& a, const MI& b) { return cmpVecForUnequal(a.data_, b.data_); }
  friend bool operator!=(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForUnequal(a.data_, b);
  }
  friend bool operator!=(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForUnequal(a, b.data_);
  }

  friend bool operator<(const MI& a, const MI& b) { return cmpVecForLess(a.data_, b.data_); }
  friend bool operator<(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForLess(a.data_, b);
  }
  friend bool operator<(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForLess(a, b.data_);
  }

  friend bool operator<=(const MI& a, const MI& b) { return cmpVecForLessEqual(a.data_, b.data_); }
  friend bool operator<=(const MI& a, const std::vector<value_type>& b) {
    return cmpVecForLessEqual(a.data_, b);
  }
  friend bool operator<=(const std::vector<value_type>& a, const MI& b) {
    return cmpVecForLessEqual(a, b.data_);
  }

  friend bool operator>(const MI& a, const MI& b) { return cmpVecGreater(a.data_, b.data_); }
  friend bool operator>(const MI& a, const std::vector<value_type>& b) {
    return cmpVecGreater(a.data_, b);
  }
  friend bool operator>(const std::vector<value_type>& a, const MI& b) {
    return cmpVecGreater(a, b.data_);
  }

  friend bool operator>=(const MI& a, const MI& b) { return cmpVecGreaterEqual(a.data_, b.data_); }
  friend bool operator>=(const MI& a, const std::vector<value_type>& b) {
    return cmpVecGreaterEqual(a.data_, b);
  }
  friend bool operator>=(const std::vector<value_type>& a, const MI& b) {
    return cmpVecGreaterEqual(a, b.data_);
  }

  /* =========================
     Arithmetische Operatoren
     ========================= */
  MI& operator+=(const MI& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] += other.data_[i];
    }
    return *this;
  }

  MI& operator+=(const std::vector<value_type>& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] += other[i];
    }
    return *this;
  }

  MI& operator-=(const MI& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] -= other.data_[i];
    }
    return *this;
  }

  MI& operator-=(const std::vector<value_type>& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] -= other[i];
    }
    return *this;
  }

  friend MI operator+(MI lhs, const MI& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend MI operator+(MI lhs, const std::vector<value_type>& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend MI operator+(const std::vector<value_type>& lhs, MI rhs) {
    rhs += lhs;
    return rhs;
  }

  friend MI operator-(MI lhs, const MI& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend MI operator-(MI lhs, const std::vector<value_type>& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend MI operator-(std::vector<value_type> lhs, const MI& rhs) {
    check_same_size(lhs, rhs);
    MI result(lhs);
    result -= rhs;
    return result;
  }

 private:
  std::vector<value_type> data_;

  static void check_same_size(const MI& a, const MI& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }
  static void check_same_size(const MI& a, const std::vector<value_type>& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }
  static void check_same_size(const std::vector<value_type>& a, const MI& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }
  static void check_same_size(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }

  static bool cmpVecForEqual(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] != b[dim]) return false;
    }
    return true;
  }

  static bool cmpVecForUnequal(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] != b[dim]) return true;
    }
    return false;
  }

  static bool cmpVecForLess(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] >= b[dim]) return false;
    }
    return true;
  }

  static bool cmpVecForLessEqual(const std::vector<value_type>& a,
                                 const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] > b[dim]) return false;
    }
    return true;
  }

  static bool cmpVecGreater(const std::vector<value_type>& a, const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] <= b[dim]) return false;
    }
    return true;
  }

  static bool cmpVecGreaterEqual(const std::vector<value_type>& a,
                                 const std::vector<value_type>& b) {
    check_same_size(a, b);
    for (size_t dim = 0; dim < a.size(); dim++) {
      if (a[dim] < b[dim]) return false;
    }
    return true;
  }
};

}  // namespace combigrid
}  // namespace sgpp

#endif