// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file vector_values.hpp
 * @brief Value type "m values per grid point" (@c VectorSourceFunc) of the operator kernels (see
 * scalar_values.hpp for the interface).
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <span>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief Value type of the operator kernels for a vector-valued source function (m values per
 * grid point, m known at run time).
 *
 * The values and accumulators live in a buffer of the caller (per thread, sized with
 * @ref bufferSize, allocated once): the m values at the current point, followed by the slots of
 * the accumulators (m doubles each). The kernels perform, per output, the operations of the
 * scalar kernel (@c ScalarValues).
 */
class VectorValues {
 public:
  /// @brief Accumulator: the m accumulated values in a slot of the buffer.
  using Acc = double*;

  /**
   * @brief Size of the buffer of a view.
   * @param nOutputs Number of outputs m.
   * @param nAccs    Number of accumulator slots the kernel uses.
   * @return Number of doubles.
   */
  static constexpr size_t bufferSize(const size_t nOutputs, const size_t nAccs) {
    return (nAccs + 1) * nOutputs;
  }

  /**
   * @brief Creates the view.
   * @param sourceFunc Source function (must outlive the view).
   * @param buf        Buffer of @ref bufferSize(sourceFunc.nOutputs(), nAccs) doubles, where
   *                   @c nAccs is the number of accumulator slots of the kernel.
   */
  VectorValues(const VectorSourceFunc& sourceFunc, const std::span<double> buf)
      : sourceFunc(&sourceFunc), nOutputs_(sourceFunc.nOutputs()), buffer(buf.data()) {
    assert(buf.size() >= bufferSize(nOutputs_, 0));
  }

  /// @brief Number of outputs m.
  size_t nOutputs() const { return nOutputs_; }

  /**
   * @brief Returns the accumulator in slot @p slot, set to zero.
   * @param slot Accumulator slot (less than the @c nAccs the buffer was sized for).
   */
  Acc zeroAcc(const size_t slot) const {
    const Acc acc = buffer + (slot + 1) * nOutputs_;
    std::fill_n(acc, nOutputs_, 0.0);
    return acc;
  }

  /**
   * @brief Evaluates the source function.
   * @param point Point in problem coordinates.
   * @return The m function values (valid until the next evaluation).
   */
  const double* evaluate(const base::DataVector& point) const {
    sourceFunc->evaluate(point, std::span<double>(buffer, nOutputs_));
    return buffer;
  }

  /// @brief @p acc += @p weight * @p values (m values, output by output).
  void addScaled(const Acc acc, const double weight, const double* const values) const {
    for (size_t k = 0; k < nOutputs_; k++) {
      acc[k] += weight * values[k];
    }
  }

  /// @brief Writes the m accumulated values to @p out.
  void store(const Acc acc, double* const out) const { std::copy_n(acc, nOutputs_, out); }

 private:
  const VectorSourceFunc* sourceFunc;  ///< Viewed source function.
  size_t nOutputs_;                    ///< Number of outputs m.
  double* buffer;                      ///< Values at the current point, then the accumulators.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
