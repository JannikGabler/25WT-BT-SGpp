// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file downwards_closedness.hpp
 * @brief Tests and constructions related to the downwards-closedness of
 * multi-index sets.
 *
 * The combination technique requires its level-multi-index set to be
 * downwards closed (DWC): for every @f$\vec{\ell} \in I@f$ all
 * @f$\vec{\ell}'\le\vec{\ell}@f$ must also belong to @f$I@f$.
 */
#pragma once

#include <omp.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  ///< Forward declaration to avoid circular includes.

namespace tools {

/**
 * @brief Tests whether a multi-index set is downwards closed.
 *
 * For every multi-index @f$\vec{\ell}@f$ in @p miVec the function checks
 * that all immediate predecessors @f$\vec{\ell} - \vec e_k@f$ (for the
 * @f$d@f$ unit vectors that yield non-negative values) are also present.
 * If a single predecessor is missing the parallel scan is cancelled and
 * @c false is returned. Parallelized with OpenMP above the threshold
 * @c constants::mi_vec::DWC_MIN_MI_FOR_CONCURRENCY.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set.
 * @return @c true iff @p miVec is downwards closed.
 *
 * @note Naive implementation; the unit-bounding-box iteration cost is
 *       @f$O(2^d)@f$ per multi-index.
 */
template <typename T>
bool isMIVecDownwardsClosed(const MIVec<T>& miVec) {
  const misc::DiscUnitBB<T> offsets(miVec.nDim());
  const std::shared_ptr<misc::MIVecLookup<T>> lookup = miVec.lookup();

  bool closed = true;

  // Separate parallel and for constructs: the loop of a combined 'parallel for' is implicitly
  // nowait and must not be cancelled. Cancellation only takes effect with OMP_CANCELLATION=true,
  // so the shared flag additionally lets all threads skip their remaining iterations early.
#pragma omp parallel shared(closed) if (miVec.nMI() >= \
                                            constants::mi_vec::DWC_MIN_MI_FOR_CONCURRENCY)
  {
    // Per-thread scratch multi-indices, reused for every candidate (no allocation per iteration).
    MI<T> mi(miVec.nDim());
    MI<T> predecessor(miVec.nDim());

#pragma omp for schedule(static)
    for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
      bool stillClosed;
#pragma omp atomic read
      stillClosed = closed;
      if (!stillClosed) {
        continue;
      }

      mi = miVec[miIdx];

      for (const std::vector<T>& offset : offsets) {
        if (!(offset <= mi)) {
          continue;
        }

        predecessor = mi;
        predecessor -= offset;

        if (!lookup->contains(predecessor)) {
#pragma omp atomic write
          closed = false;
#pragma omp cancel for
          break;
        }
      }
#pragma omp cancellation point for
    }
  }

  return closed;
}

/**
 * @brief Appends one row of the closure: the multi-indices
 * @f$(0, \ell_2, \dots, \ell_d), \dots, (m, \ell_2, \dots, \ell_d)@f$ with the entries
 * @f$\ell_2, \dots, \ell_d@f$ of @p cur.
 *
 * @tparam T            Multi-index element type.
 * @param cur           Current multi-index (@c closure.nDim() entries; entry 0 is ignored).
 * @param maxFirstEntry Largest entry @f$m@f$ of dimension 0 in the row.
 * @param closure       Output; grows by @f$m + 1@f$ multi-indices.
 */
template <typename T>
void appendClosureRow(const T* const cur, const T maxFirstEntry, MIVec<T>& closure) {
  const size_t nDim = closure.nDim();
  const size_t nRow = static_cast<size_t>(maxFirstEntry) + 1;
  const size_t first = closure.nMI();

  closure.resize(first + nRow);
  T* out = closure.data() + first * nDim;

  for (size_t entry = 0; entry < nRow; entry++) {
    out[0] = static_cast<T>(entry);
    std::copy(cur + 1, cur + nDim, out + 1);
    out += nDim;
  }
}

/**
 * @brief Appends the part of the downwards closure whose entries in the dimensions above @p dim
 * equal those of @p cur, in colexicographic order.
 *
 * A multi-index belongs to the closure iff a generator dominates it. Given the entries above
 * @p dim, the admissible entries of dimension @p dim are @f$0, \dots, m@f$ with @f$m@f$ the
 * largest entry of dimension @p dim among the generators that dominate the fixed entries. Fixing
 * the entries from the last dimension downwards (dimension 0 fastest) therefore enumerates the
 * closure in colexicographic order without duplicates, the traversal order of its bounding box.
 *
 * @tparam T          Multi-index element type.
 * @param generators  Entries of the generators that dominate the fixed entries above @p dim
 *                    (at least one); not modified.
 * @param nGenerators Number of @p generators.
 * @param dim         Dimension whose entry is fixed next.
 * @param levelLists  Scratch list of every dimension @c k >= 1, with room for all generators
 *                    whose entry of dimension @c k is positive.
 * @param cur         Current multi-index; entries above @p dim are fixed, the others are
 *                    overwritten.
 * @param closure     Output the closure part is appended to.
 */
template <typename T>
void appendClosurePart(const T* const* generators, size_t nGenerators, const size_t dim,
                       const T** const* const levelLists, T* const cur, MIVec<T>& closure) {
  if (dim == 0) {
    T maxEntry = 0;
    for (size_t i = 0; i < nGenerators; i++) {
      maxEntry = std::max(maxEntry, generators[i][0]);
    }
    appendClosureRow(cur, maxEntry, closure);
    return;
  }

  const T** const remaining = levelLists[dim];

  for (T entry = 0;; entry++) {
    cur[dim] = entry;
    appendClosurePart(generators, nGenerators, dim - 1, levelLists, cur, closure);

    // Keep the generators that also dominate entry + 1 (in place after the first pass).
    size_t nRemaining = 0;
    for (size_t i = 0; i < nGenerators; i++) {
      if (generators[i][dim] > entry) {
        remaining[nRemaining++] = generators[i];
      }
    }
    if (nRemaining == 0) {
      return;
    }
    generators = remaining;
    nGenerators = nRemaining;
  }
}

/**
 * @brief Generates the downwards closure of a multi-index set.
 *
 * The closure is the union of the boxes @f$\{\vec{\ell}' : \vec 0 \le \vec{\ell}' \le
 * \vec{\ell}\}@f$ of all @f$\vec{\ell}@f$ in @p miVec. It is enumerated dimension by dimension
 * (see @ref appendClosurePart): every enumeration step filters the input multi-indices that
 * dominate the fixed entries, so the cost depends on the size of the closure and on how many
 * input multi-indices dominate each partial multi-index, not on the bounding box. Scratch memory:
 * one pointer per input multi-index and per positive entry.
 *
 * @tparam T   Multi-index element type.
 * @param miVec Source set (may contain duplicates and dominated multi-indices).
 * @return Downwards closure of @p miVec in colexicographic order (last dimension most
 *         significant, i.e. the traversal order of its bounding box with dimension 0 fastest),
 *         without duplicates; empty if @p miVec is empty or zero-dimensional.
 */
template <typename T>
MIVec<T> genMIVecDownwardsClosure(const MIVec<T>& miVec) {
  const size_t nDim = miVec.nDim();
  const size_t nMI = miVec.nMI();
  MIVec<T> closure(nDim, 0);

  if (nDim == 0 || nMI == 0) {
    return closure;
  }

  // Storage: the generators (all input multi-indices), then the scratch list of each dimension
  // k >= 1, sized to the number of input multi-indices with a positive entry k.
  std::vector<size_t> listOffsets(nDim + 1, 0);
  listOffsets[1] = nMI;
  for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
    for (size_t dim = 1; dim < nDim; dim++) {
      listOffsets[dim + 1] += miVec(miIdx, dim) > 0 ? 1 : 0;
    }
  }
  for (size_t dim = 1; dim < nDim; dim++) {
    listOffsets[dim + 1] += listOffsets[dim];
  }

  std::vector<const T*> storage(listOffsets[nDim]);
  std::vector<const T**> levelLists(nDim);
  for (size_t dim = 0; dim < nDim; dim++) {
    levelLists[dim] = storage.data() + listOffsets[dim];
  }
  for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
    storage[miIdx] = miVec.data() + miIdx * nDim;
  }

  MI<T> cur(nDim, 0);
  appendClosurePart(levelLists[0], nMI, nDim - 1, levelLists.data(), cur.data(), closure);

  return closure;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
