#ifndef COMBIGRID_MULTIINDEX_TEST_HPP
#define COMBIGRID_MULTIINDEX_TEST_HPP

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MITest {
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
  MITest<T>() = default;

  explicit MITest<T>(const size_type count) : data_(count) {}

  MITest<T>(const size_type count, const value_type value) : data_(count, value) {}

  MITest<T>(const std::initializer_list<value_type>& init) : data_(init) {}

  explicit MITest<T>(const std::vector<value_type>& data) : data_(data) {}

  template <class InputIt>
  MITest<T>(InputIt first, InputIt last) : data_(first, last) {}

  /* =========================
     Implizite Konvertierung
     ========================= */
  operator std::vector<T>&() noexcept { return data_; }
  operator const std::vector<T>&() const noexcept { return data_; }

  /* =========================
     Additional methods
     ========================= */
  size_t toLinearIndex() const {
    const size_t nDim = size();
    size_t idx = 0;

    for (size_t dim = 0; dim < nDim; ++dim) {
      idx *= nDim;
      idx += static_cast<size_t>(data_[dim]);
    }
    return idx;
  }

  T productofElems() const {
    if (data_.empty()) {
      return T{};
    }

    T product = static_cast<T>(1);
    for (const auto& v : data_) {
      product *= v;
    }
    return product;
  }

  T sumOfElems() const {
    T sum{};
    for (const auto& v : data_) {
      sum += v;
    }
    return sum;
  }

  size_t nDim() const { return size(); }

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

  void swap(MITest& other) noexcept { data_.swap(other.data_); }

  /* =========================
     Vergleichsoperatoren
     ========================= */
  friend bool operator==(const MITest& a, const MITest& b) {
    check_same_size(a, b);
    return a.data_ == b.data_;
  }

  friend bool operator!=(const MITest& a, const MITest& b) { return !(a == b); }

  friend bool operator<(const MITest& a, const MITest& b) {
    check_same_size(a, b);
    for (size_type i = 0; i < a.size(); ++i) {
      if (a.data_[i] >= b.data_[i]) return false;
    }
    return true;
  }

  friend bool operator<=(const MITest& a, const MITest& b) {
    check_same_size(a, b);
    for (size_type i = 0; i < a.size(); ++i) {
      if (a.data_[i] > b.data_[i]) return false;
    }
    return true;
  }

  friend bool operator>(const MITest& a, const MITest& b) { return b < a; }

  friend bool operator>=(const MITest& a, const MITest& b) { return b <= a; }

  /* =========================
     Arithmetische Operatoren
     ========================= */
  MITest<T>& operator+=(const MITest<T>& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] += other.data_[i];
    }
    return *this;
  }

  MITest<T>& operator-=(const MITest<T>& other) {
    check_same_size(*this, other);
    for (size_type i = 0; i < size(); ++i) {
      data_[i] -= other.data_[i];
    }
    return *this;
  }

  friend MITest<T> operator+(MITest<T> lhs, const MITest<T>& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend MITest<T> operator-(MITest<T> lhs, const MITest<T>& rhs) {
    lhs -= rhs;
    return lhs;
  }

 private:
  static void check_same_size(const MITest<T>& a, const MITest<T>& b) {
    if (a.size() != b.size()) {
      throw std::logic_error("Sizes of MI do not match!");
    }
  }

  std::vector<T> data_;
};

}  // namespace combigrid
}  // namespace sgpp

#endif