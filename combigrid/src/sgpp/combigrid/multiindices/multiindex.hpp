#ifndef COMBIGRID_MULTIINDEX_HPP
#define COMBIGRID_MULTIINDEX_HPP

#include <cstddef>
#include <initializer_list>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class MI {
 public:
  using value_type = MIType;
  using allocator_type = std::allocator<MIType>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = std::vector<MIType>::iterator;
  using const_iterator = std::vector<MIType>::const_iterator;
  using reverse_iterator = std::vector<MIType>::reverse_iterator;
  using const_reverse_iterator = std::vector<MIType>::const_reverse_iterator;

  /* =========================
     Konstruktoren
     ========================= */
  MI() = default;

  explicit MI(size_type count);

  MI(size_type count, value_type value);

  MI(std::initializer_list<value_type> init);

  MI(std::vector<value_type>& data);

  template <class InputIt>
  MI(InputIt first, InputIt last);

  /* =========================
   Implicit convertion
   ========================= */
  operator std::vector<MIType>&() noexcept { return data_; }

  operator const std::vector<MIType>&() const noexcept { return data_; }

  /* =========================
   Additional methods
   ========================= */
  size_t toLinearIndex() const;

  size_t productofElems() const;

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
  void clear() noexcept;

  void push_back(value_type value);

  void pop_back();

  iterator insert(const_iterator pos, value_type value);

  iterator erase(const_iterator pos);

  iterator erase(const_iterator first, const_iterator last);

  void resize(size_type count);

  void resize(size_type count, value_type value);

  void swap(MI& other) noexcept;

  /* =========================
     Vergleichsoperatoren
     ========================= */
  friend bool operator==(const MI& a, const MI& b);
  friend bool operator==(const MI& a, const std::vector<MIType>& b);
  friend bool operator==(const std::vector<MIType>& a, const MI& b);

  friend bool operator!=(const MI& a, const MI& b);
  friend bool operator!=(const MI& a, const std::vector<MIType>& b);
  friend bool operator!=(const std::vector<MIType>& a, const MI& b);

  friend bool operator<(const MI& a, const MI& b);
  friend bool operator<(const MI& a, const std::vector<MIType>& b);
  friend bool operator<(const std::vector<MIType>& a, const MI& b);

  friend bool operator<=(const MI& a, const MI& b);
  friend bool operator<=(const MI& a, const std::vector<MIType>& b);
  friend bool operator<=(const std::vector<MIType>& a, const MI& b);

  friend bool operator>(const MI& a, const MI& b);
  friend bool operator>(const MI& a, const std::vector<MIType>& b);
  friend bool operator>(const std::vector<MIType>& a, const MI& b);

  friend bool operator>=(const MI& a, const MI& b);
  friend bool operator>=(const MI& a, const std::vector<MIType>& b);
  friend bool operator>=(const std::vector<MIType>& a, const MI& b);

  /* =========================
     Arithmetische Operatoren
     (elementweise)
     ========================= */

  MI& operator+=(const MI& other);
  MI& operator+=(const std::vector<MIType>& other);

  MI& operator-=(const MI& other);
  MI& operator-=(const std::vector<MIType>& other);

  friend MI operator+(MI lhs, const MI& rhs);
  friend MI operator+(MI lhs, const std::vector<MIType>& rhs);
  friend MI operator+(std::vector<MIType> lhs, const MI& rhs);

  friend MI operator-(MI lhs, const MI& rhs);
  friend MI operator-(MI lhs, const std::vector<MIType>& rhs);
  friend MI operator-(std::vector<MIType> lhs, const MI& rhs);

  /* =========================
     Wrapper <-> std::vector
     ========================= */
  friend bool operator==(const MI& a, const std::vector<MIType>& b);

  friend bool operator==(const std::vector<MIType>& a, const MI& b);

 private:
  std::vector<MIType> data_;
};

}  // namespace combigrid
}  // namespace sgpp

#endif