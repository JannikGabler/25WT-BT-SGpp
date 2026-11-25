// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file tensor_grid_processing_order.hpp
 * @brief Largest-first order of the tensor grids of a sparse grid, in which the operators hand
 * out their per-tensor-grid work to the threads (load balance of the tensor-grid loops).
 *
 * The cost of a tensor grid in the quadrature and the global interpolation is about proportional
 * to its number of grid points, and the sizes differ by orders of magnitude. Handed out in their
 * stored order, a few large tensor grids can start last and keep one thread busy while the others
 * idle; handed out largest first (longest processing time first), the small ones fill the gaps at
 * the end. The order only needs to be approximately sorted: the tensor grids are sorted by a
 * size class (@ref sizeClassOf: the binary exponent and 3 mantissa bits of the point count, i.e.
 * sizes within a factor of 1.125 share a class) with a counting sort in O(nTG), stable within a
 * class (neighbouring tensor grids, which often share node rows, stay together). In a simulation
 * of the tensor-grid loops of 40 benchmark grids at 16 threads (cost = points, greedy hand-out),
 * this order reached at least 98.7 % of the efficiency of an exact sort by size; one size class
 * per binary exponent (no mantissa bits) only 87-91 % on 2D grids with 25-41 tensor grids. The
 * smallest tensor grids are not sorted at all (all classes below a floor count as the floor, see
 * @c constants::sparse_grid_storage::SMALL_TG_POINT_SHARE_DIVISOR), so a grid of many small
 * tensor grids keeps its index order.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace sparse_grid {

/// Type of a size class (@ref sizeClassOf).
using SizeClassType = std::uint16_t;

/// Number of mantissa bits of a size class below the leading one of the point count.
constexpr unsigned SIZE_CLASS_MANTISSA_BITS = 3;

/// Number of size classes (0 for no points, then 2^3 classes per binary exponent 0, ..., 63).
constexpr size_t N_SIZE_CLASSES = (64 + 1) << SIZE_CLASS_MANTISSA_BITS;

/**
 * @brief Returns the size class of a tensor grid with @p nGP grid points: non-decreasing in
 * @p nGP; 0 for no points, else (bit width of @p nGP) * 8 + (the 3 bits after the leading one).
 *
 * @param nGP Number of grid points.
 * @return Size class in @c [0, N_SIZE_CLASSES).
 */
SizeClassType sizeClassOf(size_t nGP) noexcept;

/**
 * @brief Returns the indices @c 0, ..., n-1 sorted by decreasing @p sizeClasses, where every
 * class below @p floorClass counts as @p floorClass; ascending within a (floored) size class
 * (stable counting sort, O(n + N_SIZE_CLASSES)).
 *
 * @param sizeClasses Size class of every index (each < @c N_SIZE_CLASSES).
 * @param floorClass  Classes below it are not distinguished (0: all classes are sorted).
 * @return Permutation of @c 0, ..., n-1 (n = @c sizeClasses.size()).
 */
std::vector<size_t> orderBySizeClassDescending(std::span<const SizeClassType> sizeClasses,
                                               SizeClassType floorClass);

/**
 * @brief Returns the processing order of tensor grids with the given size classes: by decreasing
 * size class (@ref orderBySizeClassDescending), where the tensor grids with fewer than
 * @p totalGPCnt / @c constants::sparse_grid_storage::SMALL_TG_POINT_SHARE_DIVISOR points share
 * the last class (in index order).
 *
 * @param sizeClasses Size class (@ref sizeClassOf) of every tensor grid.
 * @param totalGPCnt  Total number of grid points of the tensor grids.
 * @return Permutation of the tensor-grid indices.
 */
std::vector<size_t> largestFirstOrder(std::span<const SizeClassType> sizeClasses,
                                      size_t totalGPCnt);

}  // namespace sparse_grid
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
