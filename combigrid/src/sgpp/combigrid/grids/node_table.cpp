// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdint>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <span>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

NodeTable::NodeTable(std::vector<size_t> firstRowOfDim, std::vector<LvlType> maxLvlOfDim,
                     const size_t nRows)
    : firstRowOfDim_(std::move(firstRowOfDim)),
      maxLvlOfDim_(std::move(maxLvlOfDim)),
      rows_(nRows),
      owners_(nRows) {
  assert(firstRowOfDim_.size() == maxLvlOfDim_.size());

  for (size_t dim = 0; dim < firstRowOfDim_.size(); dim++) {
    assert(firstRowOfDim_[dim] + maxLvlOfDim_[dim] < nRows);
  }
}

size_t NodeTable::nStoredNodes() const noexcept {
  size_t result = 0;

  for (const std::span<const double> row : rows_) {
    result += row.size();
  }

  return result;
}

void NodeTable::setRow(const size_t row, std::shared_ptr<const base::DataVector> owner,
                       const std::span<const double> nodes) noexcept {
  assert(row < nRows() && owner != nullptr);
  assert(nodes.empty() || (nodes.data() >= owner->data() &&
                           nodes.data() + nodes.size() <= owner->data() + owner->size()));

  rows_[row] = nodes;
  owners_[row] = std::move(owner);
}

void NodeTable::setRow(const size_t row, std::shared_ptr<const base::DataVector> owner) noexcept {
  const std::span<const double> nodes(owner->data(), owner->size());
  setRow(row, std::move(owner), nodes);
}

bool NodeTable::operator==(const NodeTable& other) const {
  // Bitwise comparison of the nodes (operator== of double would equate 0.0 and -0.0)
  const auto bitwiseEqual = [](const double a, const double b) {
    return std::bit_cast<uint64_t>(a) == std::bit_cast<uint64_t>(b);
  };

  return firstRowOfDim_ == other.firstRowOfDim_ && maxLvlOfDim_ == other.maxLvlOfDim_ &&
         std::ranges::equal(rows_, other.rows_,
                            [&](const std::span<const double> a, const std::span<const double> b) {
                              return std::ranges::equal(a, b, bitwiseEqual);
                            });
}

}  // namespace combigrid
}  // namespace sgpp
