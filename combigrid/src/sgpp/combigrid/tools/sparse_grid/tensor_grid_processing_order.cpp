// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/tools/sparse_grid/tensor_grid_processing_order.hpp>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace sparse_grid {

SizeClassType sizeClassOf(const size_t nGP) noexcept {
  if (nGP == 0) {
    return 0;
  }

  const unsigned exponent = static_cast<unsigned>(std::bit_width(nGP)) - 1;  // Leading one
  // The leading one and the SIZE_CLASS_MANTISSA_BITS bits below it, as the lowest bits
  const size_t leadingBits = (exponent >= SIZE_CLASS_MANTISSA_BITS)
                                 ? nGP >> (exponent - SIZE_CLASS_MANTISSA_BITS)
                                 : nGP << (SIZE_CLASS_MANTISSA_BITS - exponent);
  const size_t mantissa = leadingBits & ((size_t{1} << SIZE_CLASS_MANTISSA_BITS) - 1);

  return static_cast<SizeClassType>(((exponent + 1) << SIZE_CLASS_MANTISSA_BITS) | mantissa);
}

std::vector<size_t> orderBySizeClassDescending(const std::span<const SizeClassType> sizeClasses,
                                               const SizeClassType floorClass) {
  assert(floorClass < N_SIZE_CLASSES);

  const auto floored = [floorClass](const SizeClassType sizeClass) {
    return std::max(sizeClass, floorClass);
  };

  // Start of every size class in the result: the largest class first
  std::array<size_t, N_SIZE_CLASSES> starts{};
  for (const SizeClassType sizeClass : sizeClasses) {
    assert(sizeClass < N_SIZE_CLASSES);
    starts[floored(sizeClass)]++;
  }

  size_t start = 0;
  for (size_t sizeClass = N_SIZE_CLASSES; sizeClass-- > 0;) {  // N_SIZE_CLASSES - 1 .. 0
    const size_t count = starts[sizeClass];
    starts[sizeClass] = start;
    start += count;
  }

  std::vector<size_t> order(sizeClasses.size());
  for (size_t idx = 0; idx < sizeClasses.size(); idx++) {
    order[starts[floored(sizeClasses[idx])]++] = idx;
  }

  return order;
}

std::vector<size_t> largestFirstOrder(const std::span<const SizeClassType> sizeClasses,
                                      const size_t totalGPCnt) {
  return orderBySizeClassDescending(
      sizeClasses,
      sizeClassOf(totalGPCnt / constants::sparse_grid_storage::SMALL_TG_POINT_SHARE_DIVISOR));
}

}  // namespace sparse_grid
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
