#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <stdexcept>
#include <vector>

namespace sgpp {
namespace combigrid {

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

namespace {

void check_same_size(const MI& a, const MI& b) {
  if (a.size() != b.size()) {
    throw std::logic_error("Sizes of MI do not match!");
  }
}
void check_same_size(const MI& a, const std::vector<MIType>& b) {
  if (a.size() != b.size()) {
    throw std::logic_error("Sizes of MI do not match!");
  }
}
void check_same_size(const std::vector<MIType>& a, const MI& b) {
  if (a.size() != b.size()) {
    throw std::logic_error("Sizes of MI do not match!");
  }
}
void check_same_size(const std::vector<MIType>& a, const std::vector<MIType>& b) {
  if (a.size() != b.size()) {
    throw std::logic_error("Sizes of MI do not match!");
  }
}

bool cmpVecForEqual(const std::vector<value_type>& a, const std::vector<value_type>& b) {
  check_same_size(a, b);

  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a[dim] != b[dim]) {
      return false;
    }
  }

  return true;
}
bool cmpVecForUnequal(const std::vector<value_type>& a, const std::vector<value_type>& b) {
  check_same_size(a, b);

  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a[dim] != b[dim]) {
      return true;
    }
  }

  return false;
}
bool cmpVecForLess(const std::vector<value_type>& a, const std::vector<value_type>& b) {
  check_same_size(a, b);

  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a[dim] >= b[dim]) {
      return false;
    }
  }

  return true;
}
bool cmpVecForLessEqual(const std::vector<value_type>& a, const std::vector<value_type>& b) {
  check_same_size(a, b);

  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a[dim] > b[dim]) {
      return false;
    }
  }

  return true;
}
bool cmpVecGreater(const std::vector<value_type>& a, const std::vector<value_type>& b) {
  check_same_size(a, b);

  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a[dim] <= b[dim]) {
      return false;
    }
  }

  return true;
}
bool cmpVecGreaterEqual(const std::vector<value_type>& a, const std::vector<value_type>& b) {
  check_same_size(a, b);

  for (size_t dim = 0; dim < a.size(); dim++) {
    if (a[dim] < b[dim]) {
      return false;
    }
  }

  return true;
}

}  // namespace

MI::MI(size_type count) : data_(count) {}

MI::MI(size_type count, value_type value) : data_(count, value) {}

MI::MI(std::initializer_list<value_type> init) : data_(init) {}

MI::MI(std::vector<value_type>& data) : data_(data) {}

template <class InputIt>
MI::MI(InputIt first, InputIt last) : data_(first, last) {}

size_t MI::toLinearIndex() const {
  const size_t nDim = this->size();

  size_t idx = 0;
  for (size_t dim = 0; dim < nDim; dim++) {
    idx *= nDim;
    idx += static_cast<size_t>((*this)[dim]);
  }

  return idx;
}

MIType MI::productofElems() const {
  const size_t nDim = this->size();

  if (nDim == 0) {
    return 0;
  } else {
    MIType product = 1;

    for (size_t dim = 0; dim < nDim; dim++) {
      product *= static_cast<size_t>((*this)[dim]);
    }

    return product;
  }
}

MIType MI::sumOfElems() const {
  MIType sum = 0;
  for (const MIType v : *this) {
    sum += v;
  }

  return sum;
}

void MI::clear() noexcept { data_.clear(); }

void MI::push_back(value_type value) { data_.push_back(value); }

void MI::pop_back() { data_.pop_back(); }

iterator MI::insert(const_iterator pos, value_type value) { return data_.insert(pos, value); }

iterator MI::erase(const_iterator pos) { return data_.erase(pos); }

iterator MI::erase(const_iterator first, const_iterator last) { return data_.erase(first, last); }

void MI::resize(size_type count) { data_.resize(count); }

void MI::resize(size_type count, value_type value) { data_.resize(count, value); }

void MI::swap(MI& other) noexcept { data_.swap(other.data_); }

bool operator==(const MI& a, const MI& b) { return cmpVecForEqual(a.data_, b.data_); }
bool operator==(const MI& a, const std::vector<MIType>& b) { return cmpVecForEqual(a.data_, b); }
bool operator==(const std::vector<MIType>& a, const MI& b) { return cmpVecForEqual(a, b.data_); }

bool operator!=(const MI& a, const MI& b) { return cmpVecForUnequal(a.data_, b.data_); }
bool operator!=(const MI& a, const std::vector<MIType>& b) { return cmpVecForUnequal(a.data_, b); }
bool operator!=(const std::vector<MIType>& a, const MI& b) { return cmpVecForUnequal(a, b.data_); }

bool operator<(const MI& a, const MI& b) { return cmpVecForLess(a.data_, b.data_); }
bool operator<(const MI& a, const std::vector<MIType>& b) { return cmpVecForLess(a.data_, b); }
bool operator<(const std::vector<MIType>& a, const MI& b) { return cmpVecForLess(a, b.data_); }

bool operator<=(const MI& a, const MI& b) { return cmpVecForLessEqual(a.data_, b.data_); }
bool operator<=(const MI& a, const std::vector<MIType>& b) {
  return cmpVecForLessEqual(a.data_, b);
}
bool operator<=(const std::vector<MIType>& a, const MI& b) {
  return cmpVecForLessEqual(a, b.data_);
}

bool operator>(const MI& a, const MI& b) { return cmpVecGreater(a.data_, b.data_); }
bool operator>(const MI& a, const std::vector<MIType>& b) { return cmpVecGreater(a.data_, b); }
bool operator>(const std::vector<MIType>& a, const MI& b) { return cmpVecGreater(a, b.data_); }

bool operator>=(const MI& a, const MI& b) { return cmpVecGreaterEqual(a.data_, b.data_); }
bool operator>=(const MI& a, const std::vector<MIType>& b) {
  return cmpVecGreaterEqual(a.data_, b);
}
bool operator>=(const std::vector<MIType>& a, const MI& b) {
  return cmpVecGreaterEqual(a, b.data_);
}

MI& MI::operator+=(const MI& other) {
  check_same_size(*this, other);
  for (size_type i = 0; i < size(); ++i) {
    data_[i] += other.data_[i];
  }
  return *this;
}

MI& MI::operator+=(const std::vector<MIType>& other) {
  check_same_size(*this, other);
  for (size_type i = 0; i < size(); ++i) {
    data_[i] += other[i];
  }
  return *this;
}

MI& MI::operator-=(const MI& other) {
  check_same_size(*this, other);
  for (size_type i = 0; i < size(); ++i) {
    data_[i] -= other.data_[i];
  }
  return *this;
}

MI& MI::operator-=(const std::vector<MIType>& other) {
  check_same_size(*this, other);
  for (size_type i = 0; i < size(); ++i) {
    data_[i] -= other[i];
  }
  return *this;
}

MI operator+(MI lhs, const MI& rhs) {
  lhs += rhs;
  return lhs;
}

MI operator+(MI lhs, const std::vector<MIType>& rhs) {
  lhs += rhs;
  return lhs;
}

MI operator+(const std::vector<MIType>& lhs, MI rhs) {
  rhs += lhs;
  return rhs;
}

MI operator-(MI lhs, const MI& rhs) {
  lhs -= rhs;
  return lhs;
}

MI operator-(MI lhs, const std::vector<MIType>& rhs) {
  lhs -= rhs;
  return lhs;
}

MI operator-(std::vector<MIType> lhs, const MI& rhs) {
  check_same_size(lhs, rhs);

  MI result(lhs);
  result -= rhs;
  return result;
}

}  // namespace combigrid

}  // namespace sgpp
