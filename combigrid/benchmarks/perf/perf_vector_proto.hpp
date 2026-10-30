// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_vector_proto.hpp
 * @brief WP-24 prototypes of vector-valued source functions @f$f: \Omega \to \mathbb{R}^m@f$
 * (comparison benchmark, not production code).
 *
 * All variants live in this translation unit, so that they are compiled alike: the scalar
 * kernels are copies of today's production kernels (SG quadrature row loop, DSG row loop via the
 * public @c forEachGPWithGlobalWeight template, per-tensor-grid global interpolation), the
 * vector kernels are the candidate designs. The source-function wrappers are header-inline,
 * like the production @c SourceFunc::evaluate after LTO inlining (per point only the
 * @c std::function call remains).
 *
 * Candidate designs (see combigrid/perf/summaries/WP-24-vector-valued-exploration.md):
 * - @c ScalarFunc + scalar kernels: today.
 * - @c VecFunc (runtime m, values written to a @c std::span<double>) + kernels with runtime m
 *   (@c M = 0) or compile-time m (@c M > 0); a scalar function is the case m = 1, wrapped either
 *   around the @c std::function (@c wrapNested) or around the callable itself (@c wrapDirect).
 * - Row-buffered kernel: the values of a chunk of a row are evaluated into a buffer first and
 *   summed afterwards (the shape of a batched evaluation, WP-17).
 * - Interpolation: m values per grid point innermost (one barycentric factor per node for all m
 *   outputs) or outermost (the scalar 1D formula once per output).
 *
 * The vector kernels perform, per output, the same floating-point operations in the same order
 * as the scalar kernels, so component k of a vector run is bitwise equal to the scalar run of
 * component k (checked in the setup of every case: counter @c bitwise_equal).
 */
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "perf_harness.hpp"
#include "perf_problems.hpp"

namespace sgpp {
namespace combigrid {
namespace perf {
namespace vec_proto {

/*******************************************************************************
Source functions
*******************************************************************************/

/// @brief Today's representation: one @c std::function returning the scalar value.
class ScalarFunc {
 public:
  explicit ScalarFunc(std::function<double(const DataVector&)> func) : func_(std::move(func)) {}

  double evaluate(const DataVector& point) const { return func_(point); }

 private:
  std::function<double(const DataVector&)> func_;
};

/**
 * @brief Candidate: runtime number of outputs m, values written to an out-parameter.
 *
 * Holds two type-erased forms of the same callable: per point (@ref evaluate) and per row chunk
 * (@ref evaluateRow, the shape of a batched evaluation, WP-17: one @c std::function call per
 * chunk, the user callable is called (or inlined) in the loop inside it).
 */
class VecFunc {
 public:
  using Func = std::function<void(const DataVector&, std::span<double>)>;
  using RowFunc =
      std::function<void(DataVector&, size_t, std::span<const double>, std::span<double>)>;

  VecFunc(Func func, RowFunc rowFunc, const size_t nOutputs)
      : func_(std::move(func)), rowFunc_(std::move(rowFunc)), nOutputs_(nOutputs) {}

  /// @brief m = 1 around today's @c std::function (as a constructor taking it would do):
  /// two nested @c std::function calls per point.
  static VecFunc wrapNested(const std::function<double(const DataVector&)>& func) {
    return wrapDirect(func);
  }

  /// @brief m = 1 around the callable itself (as a template constructor would do): one
  /// @c std::function call per point, like today.
  template <typename F>
  static VecFunc wrapDirect(F func) {
    return VecFunc(
        [func](const DataVector& point, std::span<double> out) { out[0] = func(point); },
        [func](DataVector& point, const size_t dim, std::span<const double> coords,
               std::span<double> out) {
          for (size_t i = 0; i < coords.size(); i++) {
            point[dim] = coords[i];
            out[i] = func(point);
          }
        },
        1);
  }

  size_t nOutputs() const { return nOutputs_; }

  void evaluate(const DataVector& point, std::span<double> out) const { func_(point, out); }

  /**
   * @brief Evaluates the points @p point with @c point[dim] = @p coords[i] for all @c i into
   * @c out[i*m], ..., @c out[i*m+m-1] (leaves @c point[dim] = @p coords.back()).
   */
  void evaluateRow(DataVector& point, const size_t dim, std::span<const double> coords,
                   std::span<double> out) const {
    rowFunc_(point, dim, coords, out);
  }

 private:
  Func func_;
  RowFunc rowFunc_;
  size_t nOutputs_;
};

/// @brief Output @p k of the vector integrands: @f$\exp(\frac12 \sum_i s_k x_i)@f$ with
/// @f$s_k = 1 + k/64@f$ (output 0 is perf::expSum). The factor inside the sum keeps the compiler
/// from sharing work between outputs: an m-output evaluation costs m scalar evaluations.
inline double expSumK(const DataVector& x, const size_t k) {
  const double scale = 1.0 + static_cast<double>(k) / 64.0;
  double sum = 0.0;
  for (size_t i = 0; i < x.size(); i++) sum += x[i] * scale;
  return std::exp(0.5 * sum);
}

/// @brief Output @p k of the vector interpolation targets: perf::cosProd shifted by 0.01 k.
inline double cosProdK(const DataVector& x, const size_t k) {
  const double shift = 0.01 * static_cast<double>(k);
  double result = 1.0;
  for (size_t i = 0; i < x.size(); i++) {
    result *= std::cos(1.5 * x[i] + 0.2 * static_cast<double>(i) + shift);
  }
  return result;
}

/// @brief The m scalar functions @p fk(x, 0), ..., @p fk(x, m-1).
template <typename FK>
std::vector<ScalarFunc> scalarFuncs(FK fk, const size_t m) {
  std::vector<ScalarFunc> funcs;
  for (size_t k = 0; k < m; k++) {
    funcs.emplace_back([fk, k](const DataVector& x) { return fk(x, k); });
  }
  return funcs;
}

/// @brief One function with the m outputs @p fk(x, 0), ..., @p fk(x, m-1).
template <typename FK>
VecFunc vecFunc(FK fk, const size_t m) {
  return VecFunc(
      [fk](const DataVector& x, std::span<double> out) {
        for (size_t k = 0; k < out.size(); k++) out[k] = fk(x, k);
      },
      [fk, m](DataVector& x, const size_t dim, std::span<const double> coords,
              std::span<double> out) {
        for (size_t i = 0; i < coords.size(); i++) {
          x[dim] = coords[i];
          for (size_t k = 0; k < m; k++) out[i * m + k] = fk(x, k);
        }
      },
      m);
}

/*******************************************************************************
SG quadrature: per-tensor-grid preparation (copy of the TU-local production helpers)
*******************************************************************************/

/// @brief Copy of quadrature.cpp's findRowDim.
inline size_t findRowDim(const GPMI& nGPPerDim) {
  size_t rowDim = 0;
  while (rowDim + 1 < nGPPerDim.size() && nGPPerDim[rowDim] == 1) rowDim++;
  return rowDim;
}

/// @brief Copy of quadrature.cpp's getWeights + mapNodesOntoDomain + setUpIterDims.
inline void prepareTG(const misc::QuadratureContext& context, const TensorGrid& tg,
                      misc::QuadratureScratch& scratch) {
  quadrature_operator::getWeights(context, tg, scratch);

  const size_t nDim = tg.nDim();
  const GPMI& nGPPerDim = tg.getGPCntPerDim();
  const base::DataVector& nodesPerDim = tg.getNodesPerDim();
  size_t offset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = context.domain[dim].first;
    const double width = context.domain[dim].second - context.domain[dim].first;
    double* const nodes = scratch.nodesPerDim.data() + offset;
    for (GPCntType i = 0; i < nGPPerDim[dim]; i++) {
      nodes[i] = nodesPerDim[offset + i] * width + lower;
    }
    scratch.gp[dim] = nodes[0];
    offset += nGPPerDim[dim];
  }

  const size_t rowDim = findRowDim(nGPPerDim);
  size_t nIterDims = 1;
  double constWeight = 1.0;
  offset = 0;

  for (size_t dim = 0; dim < nDim; dim++) {
    const GPCntType nNodes = nGPPerDim[dim];
    const double* const nodes = scratch.nodesPerDim.data() + offset;
    const double* const weights = scratch.weights1D.data() + offset;
    offset += nNodes;
    if (dim != rowDim && nNodes == 1) {
      constWeight *= weights[0];
      continue;
    }
    const size_t j = (dim == rowDim) ? 0 : nIterDims++;
    scratch.iterDims[j] = dim;
    scratch.nGPOfIterDim[j] = nNodes;
    scratch.nodesOfIterDim[j] = nodes;
    scratch.weightsOfIterDim[j] = weights;
    scratch.gpMI[j] = 0;
  }

  scratch.nIterDims = nIterDims;
  scratch.weightProducts[nIterDims] = constWeight;
  for (size_t j = nIterDims - 1; j >= 1; j--) {
    scratch.weightProducts[j] =
        scratch.weightsOfIterDim[j][scratch.gpMI[j]] * scratch.weightProducts[j + 1];
  }
}

/// @brief Per-thread buffers of the vector kernels (sizes independent of the grid points).
struct VecBuffers {
  static constexpr size_t ROW_CHUNK = 128;  ///< Points per chunk of the row-buffered kernel.

  explicit VecBuffers(const size_t m) : vals(ROW_CHUNK * m), rowSum(m), result(m), tgResult(m) {}

  std::vector<double> vals;      ///< Values of the current point (chunk: point-major).
  std::vector<double> rowSum;    ///< Row sums (runtime m).
  std::vector<double> result;    ///< Result accumulators of a tensor grid (runtime m).
  std::vector<double> tgResult;  ///< Result of a tensor grid (output of the kernels).
};

/// @brief State of the row loop copied into locals (as quadrature.cpp's sumOverRows does).
struct RowLoop {
  explicit RowLoop(misc::QuadratureScratch& s)
      : nIterDims(s.nIterDims),
        iterDims(s.iterDims.data()),
        nGPOfIterDim(s.nGPOfIterDim.data()),
        nodesOfIterDim(s.nodesOfIterDim.data()),
        weightsOfIterDim(s.weightsOfIterDim.data()),
        weightProducts(s.weightProducts.data()),
        gpMI(s.gpMI.data()),
        gp(s.gp.data()),
        gpVec(s.gp),
        rowDim(iterDims[0]),
        nGPOfRow(nGPOfIterDim[0]),
        rowNodes(nodesOfIterDim[0]),
        rowWeights(weightsOfIterDim[0]) {}

  /// @brief Advances to the next row; false when all rows are done.
  bool nextRow() const {
    size_t j = 1;
    for (; j < nIterDims; j++) {
      const size_t dim = iterDims[j];
      if (++gpMI[j] < nGPOfIterDim[j]) {
        gp[dim] = nodesOfIterDim[j][gpMI[j]];
        break;
      }
      gpMI[j] = 0;
      gp[dim] = nodesOfIterDim[j][0];
    }
    if (j == nIterDims) return false;
    for (; j >= 1; j--) weightProducts[j] = weightsOfIterDim[j][gpMI[j]] * weightProducts[j + 1];
    return true;
  }

  const size_t nIterDims;
  const size_t* const iterDims;
  const GPCntType* const nGPOfIterDim;
  const double* const* const nodesOfIterDim;
  const double* const* const weightsOfIterDim;
  double* const weightProducts;
  GPCntType* const gpMI;
  double* const gp;
  const base::DataVector& gpVec;
  const size_t rowDim;
  const GPCntType nGPOfRow;
  const double* const rowNodes;
  const double* const rowWeights;
};

/*******************************************************************************
SG quadrature: row kernels
*******************************************************************************/

/// @brief Today: copy of quadrature.cpp's sumOverRows.
inline double sumOverRowsScalar(const ScalarFunc& f, misc::QuadratureScratch& scratch) {
  const RowLoop loop(scratch);
  double result = 0;

  do {
    double rowSum = 0;
    for (GPCntType rowIdx = 0; rowIdx < loop.nGPOfRow; rowIdx++) {
      loop.gp[loop.rowDim] = loop.rowNodes[rowIdx];
      rowSum += loop.rowWeights[rowIdx] * f.evaluate(loop.gpVec);
    }
    result += loop.weightProducts[1] * rowSum;
  } while (loop.nextRow());

  return result;
}

/*
 * Template on the value type: one generic row kernel (sumOverRowsT) for the value types
 * ScalarValues (today's double: the scalar instantiation is today's kernel), FixedValues<M>
 * (compile-time m, accumulators in registers) and DynamicValues (runtime m, accumulators in the
 * per-thread buffers). Acc: accumulator of m values; evaluate returns the m values of a point.
 */

/// @brief Value type "scalar": today's double.
struct ScalarValues {
  using Acc = double;

  ScalarValues(const ScalarFunc& f, VecBuffers&) : f(f) {}

  Acc zeroRow() const { return 0.0; }
  Acc zeroResult() const { return 0.0; }
  double evaluate(const DataVector& gp) const { return f.evaluate(gp); }
  static void addScaled(Acc& acc, const double w, const double v) { acc += w * v; }
  static void store(const Acc& acc, double* out) { out[0] = acc; }

  const ScalarFunc& f;
};

/// @brief Value type "m-vector" with compile-time m.
template <size_t M>
struct FixedValues {
  using Acc = std::array<double, M>;

  FixedValues(const VecFunc& f, VecBuffers& buf) : f(f), vals(buf.vals.data()) {}

  Acc zeroRow() const { return Acc{}; }
  Acc zeroResult() const { return Acc{}; }
  const double* evaluate(const DataVector& gp) const {
    f.evaluate(gp, std::span<double>(vals, M));
    return vals;
  }
  static void addScaled(Acc& acc, const double w, const double* v) {
    for (size_t k = 0; k < M; k++) acc[k] += w * v[k];
  }
  static void addScaled(Acc& acc, const double w, const Acc& v) { addScaled(acc, w, v.data()); }
  static void store(const Acc& acc, double* out) { std::copy_n(acc.data(), M, out); }

  const VecFunc& f;
  double* const vals;
};

/// @brief Value type "m-vector" with runtime m.
struct DynamicValues {
  /// @brief Handle of m accumulators in memory.
  struct Acc {
    double* p;
  };

  DynamicValues(const VecFunc& f, VecBuffers& buf)
      : f(f),
        m(f.nOutputs()),
        vals(buf.vals.data()),
        rowSum(buf.rowSum.data()),
        result(buf.result.data()) {}

  Acc zeroRow() const {
    for (size_t k = 0; k < m; k++) rowSum[k] = 0.0;
    return {rowSum};
  }
  Acc zeroResult() const {
    for (size_t k = 0; k < m; k++) result[k] = 0.0;
    return {result};
  }
  const double* evaluate(const DataVector& gp) const {
    f.evaluate(gp, std::span<double>(vals, m));
    return vals;
  }
  void addScaled(Acc acc, const double w, const double* v) const {
    for (size_t k = 0; k < m; k++) acc.p[k] += w * v[k];
  }
  void addScaled(Acc acc, const double w, const Acc v) const { addScaled(acc, w, v.p); }
  void store(const Acc acc, double* out) const { std::copy_n(acc.p, m, out); }

  const VecFunc& f;
  const size_t m;
  double* const vals;
  double* const rowSum;
  double* const result;
};

/// @brief The generic row kernel (same structure as sumOverRowsScalar).
template <typename Values>
void sumOverRowsT(const Values& values, misc::QuadratureScratch& scratch, double* const out) {
  const RowLoop loop(scratch);
  typename Values::Acc result = values.zeroResult();

  do {
    typename Values::Acc rowSum = values.zeroRow();
    for (GPCntType rowIdx = 0; rowIdx < loop.nGPOfRow; rowIdx++) {
      loop.gp[loop.rowDim] = loop.rowNodes[rowIdx];
      values.addScaled(rowSum, loop.rowWeights[rowIdx], values.evaluate(loop.gpVec));
    }
    values.addScaled(result, loop.weightProducts[1], rowSum);
  } while (loop.nextRow());

  values.store(result, out);
}

/**
 * @brief Row-buffered: evaluates chunks of up to VecBuffers::ROW_CHUNK points of a row point by
 * point into a buffer (point-major, m innermost), then accumulates the chunk (runtime m):
 * @p outputsOuter = false: point by point over the m outputs; true: output by output over the
 * chunk (a register accumulator per output).
 */
inline void sumOverRowsRowBuffered(const VecFunc& f, misc::QuadratureScratch& scratch,
                                   VecBuffers& buf, const bool outputsOuter,
                                   double* const result) {
  const RowLoop loop(scratch);
  const size_t m = f.nOutputs();
  double* const vals = buf.vals.data();
  double* const rowSum = buf.rowSum.data();

  for (size_t k = 0; k < m; k++) result[k] = 0.0;

  do {
    for (size_t k = 0; k < m; k++) rowSum[k] = 0.0;
    for (GPCntType start = 0; start < loop.nGPOfRow; start += VecBuffers::ROW_CHUNK) {
      const GPCntType end = std::min<GPCntType>(start + VecBuffers::ROW_CHUNK, loop.nGPOfRow);
      for (GPCntType rowIdx = start; rowIdx < end; rowIdx++) {
        loop.gp[loop.rowDim] = loop.rowNodes[rowIdx];
        f.evaluate(loop.gpVec, std::span<double>(vals + (rowIdx - start) * m, m));
      }
      if (outputsOuter) {
        for (size_t k = 0; k < m; k++) {
          double sum = rowSum[k];
          for (GPCntType rowIdx = start; rowIdx < end; rowIdx++) {
            sum += loop.rowWeights[rowIdx] * vals[(rowIdx - start) * m + k];
          }
          rowSum[k] = sum;
        }
      } else {
        for (GPCntType rowIdx = start; rowIdx < end; rowIdx++) {
          const double w = loop.rowWeights[rowIdx];
          const double* const v = vals + (rowIdx - start) * m;
          for (size_t k = 0; k < m; k++) rowSum[k] += w * v[k];
        }
      }
    }
    const double wp = loop.weightProducts[1];
    for (size_t k = 0; k < m; k++) result[k] += wp * rowSum[k];
  } while (loop.nextRow());
}

/**
 * @brief Batched: one VecFunc::evaluateRow call per chunk of up to VecBuffers::ROW_CHUNK points
 * of a row (runtime m), then output by output over the chunk.
 */
inline void sumOverRowsBatch(const VecFunc& f, misc::QuadratureScratch& scratch, VecBuffers& buf,
                             double* const result) {
  const RowLoop loop(scratch);
  const size_t m = f.nOutputs();
  double* const vals = buf.vals.data();
  double* const rowSum = buf.rowSum.data();
  base::DataVector& gpVec = scratch.gp;

  for (size_t k = 0; k < m; k++) result[k] = 0.0;

  do {
    for (size_t k = 0; k < m; k++) rowSum[k] = 0.0;
    for (GPCntType start = 0; start < loop.nGPOfRow; start += VecBuffers::ROW_CHUNK) {
      const GPCntType len = std::min<GPCntType>(VecBuffers::ROW_CHUNK, loop.nGPOfRow - start);
      f.evaluateRow(gpVec, loop.rowDim, std::span<const double>(loop.rowNodes + start, len),
                    std::span<double>(vals, len * m));
      for (size_t k = 0; k < m; k++) {
        double sum = rowSum[k];
        for (GPCntType i = 0; i < len; i++) sum += loop.rowWeights[start + i] * vals[i * m + k];
        rowSum[k] = sum;
      }
    }
    const double wp = loop.weightProducts[1];
    for (size_t k = 0; k < m; k++) result[k] += wp * rowSum[k];
  } while (loop.nextRow());
}

/// @brief Kernel selector of the vector quadrature cases.
enum class QuadKernel { RUNTIME, COMPILE_TIME, ROW_BUFFERED, ROW_BUFFERED_OUTPUTS_OUTER, BATCH };

/// @brief sumOverRowsT with the compile-time m (only the m of the benchmark cases).
inline void sumOverRowsCompileTime(const VecFunc& f, misc::QuadratureScratch& scratch,
                                   VecBuffers& buf, double* const result) {
  switch (f.nOutputs()) {
    case 1: return sumOverRowsT(FixedValues<1>(f, buf), scratch, result);
    case 2: return sumOverRowsT(FixedValues<2>(f, buf), scratch, result);
    case 4: return sumOverRowsT(FixedValues<4>(f, buf), scratch, result);
    case 8: return sumOverRowsT(FixedValues<8>(f, buf), scratch, result);
    case 32: return sumOverRowsT(FixedValues<32>(f, buf), scratch, result);
    default: std::abort();
  }
}

inline double processTGScalar(const misc::QuadratureContext& context, const TensorGrid& tg,
                              misc::QuadratureScratch& scratch, const ScalarFunc& f) {
  if (tg.nGP() == 0) return 0;
  prepareTG(context, tg, scratch);
  return sumOverRowsScalar(f, scratch);
}

/// @brief The scalar instantiation of the generic kernel (template on the value type).
inline double processTGGenericScalar(const misc::QuadratureContext& context, const TensorGrid& tg,
                                     misc::QuadratureScratch& scratch, const ScalarFunc& f,
                                     VecBuffers& buf) {
  if (tg.nGP() == 0) return 0;
  prepareTG(context, tg, scratch);
  double result;
  sumOverRowsT(ScalarValues(f, buf), scratch, &result);
  return result;
}

inline void processTGVec(const misc::QuadratureContext& context, const TensorGrid& tg,
                         misc::QuadratureScratch& scratch, const VecFunc& f, VecBuffers& buf,
                         const QuadKernel kernel, double* const result) {
  if (tg.nGP() == 0) {
    std::fill_n(result, f.nOutputs(), 0.0);
    return;
  }
  prepareTG(context, tg, scratch);
  switch (kernel) {
    case QuadKernel::RUNTIME: return sumOverRowsT(DynamicValues(f, buf), scratch, result);
    case QuadKernel::COMPILE_TIME: return sumOverRowsCompileTime(f, scratch, buf, result);
    case QuadKernel::ROW_BUFFERED:
      return sumOverRowsRowBuffered(f, scratch, buf, false, result);
    case QuadKernel::ROW_BUFFERED_OUTPUTS_OUTER:
      return sumOverRowsRowBuffered(f, scratch, buf, true, result);
    case QuadKernel::BATCH: return sumOverRowsBatch(f, scratch, buf, result);
  }
}

/*******************************************************************************
DSG quadrature (public row loop template of the production code)
*******************************************************************************/

inline double processDeltaTGScalar(const misc::DeltaQuadratureContext& context,
                                   const TensorGridCTData& tgData,
                                   misc::DeltaQuadratureScratch& scratch, const ScalarFunc& f) {
  double result = 0.0;
  delta_quadrature_operator::forEachGPWithGlobalWeight(
      context, tgData, scratch,
      [&](const base::DataVector& gp, const double w) { result += w * f.evaluate(gp); });
  return result;
}

/// @brief Runtime m: the global weight of a point (trie work) is computed once for all outputs.
inline void processDeltaTGVec(const misc::DeltaQuadratureContext& context,
                              const TensorGridCTData& tgData,
                              misc::DeltaQuadratureScratch& scratch, const VecFunc& f,
                              VecBuffers& buf, double* const result) {
  const size_t m = f.nOutputs();
  double* const vals = buf.vals.data();
  const std::span<double> out(vals, m);
  std::fill_n(result, m, 0.0);
  delta_quadrature_operator::forEachGPWithGlobalWeight(
      context, tgData, scratch, [&](const base::DataVector& gp, const double w) {
        f.evaluate(gp, out);
        for (size_t k = 0; k < m; k++) result[k] += w * vals[k];
      });
}

/**
 * @brief Copy of delta_quadrature_operator::forEachGPWithGlobalWeight that hands whole rows to
 * @p rowFunc(gp, rowDim, rowNodes, rowWeights) (@c gp: the first point of the row, all
 * coordinates but @c rowDim set) instead of single points.
 */
template <typename RowFunc>
[[gnu::aligned(64)]] void forEachRowWithGlobalWeights(const misc::DeltaQuadratureContext& context,
                                                      const TensorGridCTData& deltaTGData,
                                                      misc::DeltaQuadratureScratch& scratch,
                                                      RowFunc&& rowFunc) {
  if (deltaTGData.tensorGrid.nGP() == 0) return;

  delta_quadrature_operator::prepareDeltaTensorGrid(context, deltaTGData, scratch);

  const size_t nProjDepths = scratch.projDims.size();
  const size_t* const projDims = scratch.projDims.data();
  const GPCntType* const gpCntOfProjDepth = scratch.gpCntOfProjDepth.data();
  const double* const* const nodesOfProjDepth = scratch.nodesOfProjDepth.data();
  const double* const rowWeights = scratch.rowWeights.data();
  GPCntType* const gpMI = scratch.gpMI.data();
  double* const gp = scratch.gp.data();
  base::DataVector& gpVec = scratch.gp;
  const size_t rowDim = projDims[0];
  const GPCntType nGPPerRow = gpCntOfProjDepth[0];
  const double* const rowNodes = nodesOfProjDepth[0];

  delta_quadrature_operator::updateCoeffSums(scratch, nProjDepths - 1);

  while (true) {
    delta_quadrature_operator::compRowWeights(scratch, nGPPerRow);
    rowFunc(gpVec, rowDim, std::span<const double>(rowNodes, nGPPerRow),
            std::span<const double>(rowWeights, nGPPerRow));

    size_t depth = 1;
    for (; depth < nProjDepths; depth++) {
      double& coord = gp[projDims[depth]];
      if (++gpMI[depth] < gpCntOfProjDepth[depth]) {
        coord = nodesOfProjDepth[depth][gpMI[depth]];
        break;
      }
      gpMI[depth] = 0;
      coord = nodesOfProjDepth[depth][0];
    }
    if (depth == nProjDepths) return;

    delta_quadrature_operator::updateCoeffSums(scratch, depth);
  }
}

/// @brief Batched: one VecFunc::evaluateRow call per chunk of a row, output by output.
inline void processDeltaTGBatch(const misc::DeltaQuadratureContext& context,
                                const TensorGridCTData& tgData,
                                misc::DeltaQuadratureScratch& scratch, const VecFunc& f,
                                VecBuffers& buf, double* const result) {
  const size_t m = f.nOutputs();
  double* const vals = buf.vals.data();
  std::fill_n(result, m, 0.0);
  forEachRowWithGlobalWeights(
      context, tgData, scratch,
      [&](base::DataVector& gp, const size_t rowDim, std::span<const double> nodes,
          std::span<const double> weights) {
        for (size_t start = 0; start < nodes.size(); start += VecBuffers::ROW_CHUNK) {
          const size_t len = std::min(VecBuffers::ROW_CHUNK, nodes.size() - start);
          f.evaluateRow(gp, rowDim, nodes.subspan(start, len), std::span<double>(vals, len * m));
          for (size_t k = 0; k < m; k++) {
            double sum = result[k];
            for (size_t i = 0; i < len; i++) sum += weights[start + i] * vals[i * m + k];
            result[k] = sum;
          }
        }
      });
}

/*******************************************************************************
Global interpolation (copy of global_interpolation.cpp with in-TU 1D formulas)
*******************************************************************************/

/// @brief Copy of InterpolationMethod::hasTrivialResult over m values per node (m innermost).
inline bool trivial1D(const double pos, const std::vector<double>& nodes, const double* values,
                      const size_t m, double* out) {
  const size_t n = nodes.size();
  if (n == 0) {
    std::fill_n(out, m, 0.0);
    return true;
  } else if (n == 1) {
    std::copy_n(values, m, out);
    return true;
  } else if (n == 2) {
    for (size_t k = 0; k < m; k++) {
      out[k] = (values[m + k] - values[k]) / (nodes[1] - nodes[0]) * (pos - nodes[0]) + values[k];
    }
    return true;
  }
  for (size_t i = 0; i < n; i++) {
    if (tools::nearlyEqual(pos, nodes[i])) {
      std::copy_n(values + i * m, m, out);
      return true;
    }
  }
  return false;
}

/**
 * @brief Copies of OptBarycentricFormula (@p optCC) / BarycentricFormula over m values per node
 * (m innermost): the factor of a node is computed once for all m outputs. m = 1 is the scalar
 * formula. @p numerators: m doubles of scratch.
 */
inline void interp1D(const bool optCC, const double pos, const std::vector<double>& nodes,
                     const double* values, const size_t m, double* out, double* numerators) {
  if (trivial1D(pos, nodes, values, m, out)) return;
  const size_t n = nodes.size();
  double denominator;

  if (optCC) {
    double curWeight = 1.0;
    double curFactor = 1.0 / (2.0 * (pos - nodes[0]));
    for (size_t k = 0; k < m; k++) numerators[k] = values[k] * curFactor;
    denominator = curFactor;
    for (size_t i = 1; i < n - 1; i++) {
      curWeight *= -1.0;
      curFactor = curWeight / (pos - nodes[i]);
      for (size_t k = 0; k < m; k++) numerators[k] += curFactor * values[i * m + k];
      denominator += curFactor;
    }
    curFactor = -curWeight / (2.0 * (pos - nodes[n - 1]));
    for (size_t k = 0; k < m; k++) numerators[k] += curFactor * values[(n - 1) * m + k];
    denominator += curFactor;
  } else {
    const std::vector<double> weights = tools::barycentricFormulaWeights(nodes);
    std::fill_n(numerators, m, 0.0);
    denominator = 0.0;
    for (size_t i = 0; i < n; i++) {
      const double factor = weights[i] / (pos - nodes[i]);
      for (size_t k = 0; k < m; k++) numerators[k] += factor * values[i * m + k];
      denominator += factor;
    }
  }

  for (size_t k = 0; k < m; k++) out[k] = numerators[k] / denominator;
}

/**
 * @brief As interp1D, but the factors of all nodes are computed first (into @p factors, one per
 * node) and then the m outputs are accumulated output by output (a register accumulator each).
 */
inline void interp1DK(const bool optCC, const double pos, const std::vector<double>& nodes,
                      const double* values, const size_t m, double* out, double* factors) {
  if (trivial1D(pos, nodes, values, m, out)) return;
  const size_t n = nodes.size();
  double denominator;

  if (optCC) {
    double curWeight = 1.0;
    factors[0] = 1.0 / (2.0 * (pos - nodes[0]));
    denominator = factors[0];
    for (size_t i = 1; i < n - 1; i++) {
      curWeight *= -1.0;
      factors[i] = curWeight / (pos - nodes[i]);
      denominator += factors[i];
    }
    factors[n - 1] = -curWeight / (2.0 * (pos - nodes[n - 1]));
    denominator += factors[n - 1];
    for (size_t k = 0; k < m; k++) {
      double numerator = values[k] * factors[0];
      for (size_t i = 1; i < n; i++) numerator += factors[i] * values[i * m + k];
      out[k] = numerator / denominator;
    }
  } else {
    const std::vector<double> weights = tools::barycentricFormulaWeights(nodes);
    denominator = 0.0;
    for (size_t i = 0; i < n; i++) {
      factors[i] = weights[i] / (pos - nodes[i]);
      denominator += factors[i];
    }
    for (size_t k = 0; k < m; k++) {
      double numerator = 0.0;
      for (size_t i = 0; i < n; i++) numerator += factors[i] * values[i * m + k];
      out[k] = numerator / denominator;
    }
  }
}

/// @brief Scalar 1D formula (today's arithmetic).
inline double interp1DScalar(const bool optCC, const double pos, const std::vector<double>& nodes,
                             const double* values) {
  double out;
  double numerator;
  interp1D(optCC, pos, nodes, values, 1, &out, &numerator);
  return out;
}

/// @brief Inline copy of TensorGrid::getGridPoint + tools::denormalizeDataVector.
inline void denormalizedGridPoint(const TensorGrid& tg, size_t idx, const HyperCubeArea& domain,
                                  DataVector& gp) {
  const GPMI& nGPPerDim = tg.getGPCntPerDim();
  const base::DataVector& nodesPerDim = tg.getNodesPerDim();
  size_t offset = 0;
  for (size_t dim = 0; dim < gp.size(); dim++) {
    const GPCntType n = nGPPerDim[dim];
    gp[dim] = nodesPerDim[offset + idx % n];
    idx /= n;
    offset += n;
    gp[dim] *= domain[dim].second - domain[dim].first;
    gp[dim] += domain[dim].first;
  }
}

/// @brief Data of one per-tensor-grid interpolation that do not depend on the layout.
struct InterpTG {
  InterpTG(const TensorGridCTData& tgData, const SGGenInstr& genInstr, const bool optCC)
      : tgData(tgData), genInstr(genInstr), optCC(optCC) {}

  std::vector<double> nodes(const size_t dim) const {
    const GPCntType n = tgData.tensorGrid.getGPCntPerDim()[dim];
    const bool addBoundary = tgData.mi[dim] >= genInstr.getBoundaryLevelOffset();
    const base::DataVector nodesVec = genInstr.getNodeGenFuncForDim(dim)->genNodes(n, addBoundary);
    return std::vector<double>(nodesVec.begin(), nodesVec.end());
  }

  const TensorGridCTData& tgData;
  const SGGenInstr& genInstr;
  const bool optCC;
};

/// @brief Today: copy of global_interpolation::interpolate (per tensor grid).
inline double interpTGScalar(const ScalarFunc& f, const DataVector& point, const InterpTG& ctx) {
  const TensorGrid& tg = ctx.tgData.tensorGrid;
  const size_t nDim = tg.nDim();
  const std::vector<size_t> cnt = global_interpolation::getInterpolationCntPerDim(tg);
  const HyperCubeArea& domain = ctx.genInstr.getDomain();

  // First dimension (evaluates the source function)
  const GPCntType n0 = tg.getGPCntPerDim()[0];
  const std::vector<double> nodes0 = ctx.nodes(0);
  DataVector gp(nDim);
  std::vector<double> results(cnt[0]);
  std::vector<double> funcValues(n0);

  for (size_t i = 0; i < cnt[0]; i++) {
    for (size_t j = 0; j < n0; j++) {
      denormalizedGridPoint(tg, i * n0 + j, domain, gp);
      funcValues[j] = f.evaluate(gp);
    }
    results[i] = interp1DScalar(ctx.optCC, point[0], nodes0, funcValues.data());
  }

  // Later dimensions (in place)
  for (size_t dim = 1; dim < nDim; dim++) {
    const GPCntType n = tg.getGPCntPerDim()[dim];
    const std::vector<double> nodes = ctx.nodes(dim);
    std::vector<double> inputValues(n);
    for (size_t i = 0; i < cnt[dim]; i++) {
      std::copy_n(results.data() + i * n, n, inputValues.begin());
      results[i] = interp1DScalar(ctx.optCC, point[dim], nodes, inputValues.data());
    }
    results.resize(cnt[dim]);
  }

  return results[0];
}

/**
 * @brief m innermost: one tensor of m-vectors; the factor of a node serves all m outputs
 * (@p outputsOuter: interp1DK instead of interp1D).
 */
inline void interpTGVecInnerT(const VecFunc& f, const DataVector& point, const InterpTG& ctx,
                              const bool outputsOuter, double* const out) {
  const TensorGrid& tg = ctx.tgData.tensorGrid;
  const size_t nDim = tg.nDim();
  const size_t m = f.nOutputs();
  const std::vector<size_t> cnt = global_interpolation::getInterpolationCntPerDim(tg);
  const HyperCubeArea& domain = ctx.genInstr.getDomain();

  const GPCntType n0 = tg.getGPCntPerDim()[0];
  const std::vector<double> nodes0 = ctx.nodes(0);
  DataVector gp(nDim);
  std::vector<double> results(cnt[0] * m);
  std::vector<double> funcValues(n0 * m);
  size_t maxNodes = m;
  for (size_t dim = 0; dim < nDim; dim++) {
    maxNodes = std::max<size_t>(maxNodes, tg.getGPCntPerDim()[dim]);
  }
  std::vector<double> numerators(maxNodes);  // numerators (interp1D) or factors (interp1DK)
  auto interp = [&](const double pos, const std::vector<double>& nodes, const double* values,
                    double* result) {
    if (outputsOuter) {
      interp1DK(ctx.optCC, pos, nodes, values, m, result, numerators.data());
    } else {
      interp1D(ctx.optCC, pos, nodes, values, m, result, numerators.data());
    }
  };

  for (size_t i = 0; i < cnt[0]; i++) {
    for (size_t j = 0; j < n0; j++) {
      denormalizedGridPoint(tg, i * n0 + j, domain, gp);
      f.evaluate(gp, std::span<double>(funcValues.data() + j * m, m));
    }
    interp(point[0], nodes0, funcValues.data(), results.data() + i * m);
  }

  for (size_t dim = 1; dim < nDim; dim++) {
    const GPCntType n = tg.getGPCntPerDim()[dim];
    const std::vector<double> nodes = ctx.nodes(dim);
    std::vector<double> inputValues(n * m);
    for (size_t i = 0; i < cnt[dim]; i++) {
      std::copy_n(results.data() + i * n * m, n * m, inputValues.begin());
      interp(point[dim], nodes, inputValues.data(), results.data() + i * m);
    }
    results.resize(cnt[dim] * m);
  }

  std::copy_n(results.data(), m, out);
}

inline void interpTGVecInner(const VecFunc& f, const DataVector& point, const InterpTG& ctx,
                             double* const out) {
  interpTGVecInnerT(f, point, ctx, false, out);
}

inline void interpTGVecInnerK(const VecFunc& f, const DataVector& point, const InterpTG& ctx,
                              double* const out) {
  interpTGVecInnerT(f, point, ctx, true, out);
}

/// @brief m outermost: one tensor per output; the scalar 1D formula runs once per output.
inline void interpTGVecOuter(const VecFunc& f, const DataVector& point, const InterpTG& ctx,
                             double* const out) {
  const TensorGrid& tg = ctx.tgData.tensorGrid;
  const size_t nDim = tg.nDim();
  const size_t m = f.nOutputs();
  const std::vector<size_t> cnt = global_interpolation::getInterpolationCntPerDim(tg);
  const HyperCubeArea& domain = ctx.genInstr.getDomain();

  const GPCntType n0 = tg.getGPCntPerDim()[0];
  const std::vector<double> nodes0 = ctx.nodes(0);
  DataVector gp(nDim);
  std::vector<double> results(cnt[0] * m);  // results[k * cnt + i]
  std::vector<double> funcValues(n0 * m);   // funcValues[k * n0 + j]
  std::vector<double> pointValues(m);

  for (size_t i = 0; i < cnt[0]; i++) {
    for (size_t j = 0; j < n0; j++) {
      denormalizedGridPoint(tg, i * n0 + j, domain, gp);
      f.evaluate(gp, pointValues);
      for (size_t k = 0; k < m; k++) funcValues[k * n0 + j] = pointValues[k];
    }
    for (size_t k = 0; k < m; k++) {
      results[k * cnt[0] + i] =
          interp1DScalar(ctx.optCC, point[0], nodes0, funcValues.data() + k * n0);
    }
  }

  size_t prevCnt = cnt[0];
  for (size_t dim = 1; dim < nDim; dim++) {
    const GPCntType n = tg.getGPCntPerDim()[dim];
    const std::vector<double> nodes = ctx.nodes(dim);
    std::vector<double> inputValues(n);
    for (size_t k = 0; k < m; k++) {
      for (size_t i = 0; i < cnt[dim]; i++) {
        std::copy_n(results.data() + k * prevCnt + i * n, n, inputValues.begin());
        results[k * cnt[dim] + i] =
            interp1DScalar(ctx.optCC, point[dim], nodes, inputValues.data());
      }
    }
    prevCnt = cnt[dim];
    results.resize(cnt[dim] * m);
  }

  for (size_t k = 0; k < m; k++) out[k] = results[k * prevCnt];
}

/*******************************************************************************
Benchmark cases
*******************************************************************************/

inline std::string cfgName(const std::string& nodes, const LvlType boundary, const size_t nDim,
                           const LvlType maxLvl) {
  return nodes + "/b" + std::to_string(boundary) + "/d" + std::to_string(nDim) + "/L" +
         std::to_string(maxLvl);
}

/**
 * @brief Sets the counters @c bitwise_equal (1 if @p vec equals @p scalars bitwise) and
 * @c max_ulp (largest distance in units in the last place) of a case; reports distances above
 * 16 ulp on stderr.
 */
inline void compareResults(MicroRun& run, const std::string& name, const std::vector<double>& vec,
                           const std::vector<double>& scalars) {
  double maxUlp = 0.0;
  for (size_t k = 0; k < vec.size(); k++) {
    const int64_t a = std::bit_cast<int64_t>(vec[k]);
    const int64_t b = std::bit_cast<int64_t>(scalars[k]);
    maxUlp = std::max(maxUlp, std::abs(static_cast<double>(a - b)));
  }
  if (maxUlp > 16.0) std::fprintf(stderr, "WARN %s: %.0f ulp\n", name.c_str(), maxUlp);
  run.setCounter("bitwise_equal", (maxUlp == 0.0) ? 1.0 : 0.0);
  run.setCounter("max_ulp", maxUlp);
}

/// @brief Tensor grids processed by a quadrature case: the largest one, or all (@p allTGs).
template <typename Grid>
std::vector<const TensorGridCTData*> selectTGs(const Grid& grid, const bool allTGs) {
  std::vector<const TensorGridCTData*> tgs;
  const TensorGridCTData* largest = &*grid.begin();
  for (const TensorGridCTData& tgData : grid) {
    if (allTGs) tgs.push_back(&tgData);
    if (tgData.tensorGrid.nGP() > largest->tensorGrid.nGP()) largest = &tgData;
  }
  if (!allTGs) tgs.push_back(largest);
  return tgs;
}

/**
 * @brief SG quadrature cases micro/vec/quad/<variant>/<cfg>[/all]/m<m>, ns per grid point:
 * - scalar_x_m: m scalar runs (today's kernel, one per output);
 * - rt / ct: one vector run of the generic kernel with runtime / compile-time m;
 * - rowbuf / rowbufk: row-buffered, accumulated point by point / output by output;
 * - batch: one VecFunc::evaluateRow call per row chunk.
 * m = 1 additionally: scalar (today with perf::expSum as a function pointer, as the e2e cases),
 * tvt_scalar (scalar instantiation of the generic kernel), <kernel>_nested / <kernel>_direct
 * (perf::expSum wrapped as m = 1).
 */
inline void addQuadCases(Suite& suite, const std::string& nodes, const LvlType boundary,
                         const size_t nDim, const LvlType maxLvl, const bool allTGs,
                         const std::vector<size_t>& ms) {
  const std::string cfg = cfgName(nodes, boundary, nDim, maxLvl) + (allTGs ? "/all" : "");

  struct Setup {
    Setup(const std::string& nodes, LvlType boundary, size_t nDim, LvlType maxLvl, bool allTGs)
        : genInstr(maxLvl, nDim) {
      perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
      sg.emplace(genInstr);
      tgs = selectTGs(*sg, allTGs);
      for (const TensorGridCTData* tgData : tgs) {
        nGP += static_cast<double>(tgData->tensorGrid.nGP());
      }
    }
    misc::QuadratureContext context() const {
      return {.sparseGrid = *sg,
              .domain = sg->getGenInstr().getDomain(),
              .nodeGenFuncs = sg->getGenInstr().getNodeGenFuncs()};
    }
    CompleteSGGenInstr genInstr;
    std::optional<SparseGrid> sg;
    std::vector<const TensorGridCTData*> tgs;
    double nGP = 0.0;
  };

  // Runs the scalar kernel over the selected tensor grids
  auto scalarRun = [](const Setup& s, const misc::QuadratureContext& context,
                      misc::QuadratureScratch& scratch, const ScalarFunc& f) {
    double sum = 0.0;
    for (const TensorGridCTData* tgData : s.tgs) {
      sum += tgData->coefficient * processTGScalar(context, tgData->tensorGrid, scratch, f);
    }
    return sum;
  };
  // Per-tensor-grid results for the comparisons (the combined sums cancel: the distance of the
  // sums in ulp says little about the kernels); vector results tensor grid by tensor grid
  auto scalarPerTG = [](const Setup& s, const misc::QuadratureContext& context,
                        misc::QuadratureScratch& scratch, const auto& process) {
    std::vector<double> results;
    for (const TensorGridCTData* tgData : s.tgs) {
      results.push_back(process(context, tgData->tensorGrid, scratch));
    }
    return results;
  };
  auto scalarsPerTG = [scalarPerTG](const Setup& s, const misc::QuadratureContext& context,
                                    misc::QuadratureScratch& scratch,
                                    const std::vector<ScalarFunc>& funcs) {
    const size_t m = funcs.size();
    std::vector<double> results(s.tgs.size() * m);
    for (size_t k = 0; k < m; k++) {
      const std::vector<double> perTG = scalarPerTG(
          s, context, scratch,
          [&](const misc::QuadratureContext& c, const TensorGrid& tg,
              misc::QuadratureScratch& sc) { return processTGScalar(c, tg, sc, funcs[k]); });
      for (size_t t = 0; t < s.tgs.size(); t++) results[t * m + k] = perTG[t];
    }
    return results;
  };
  auto vecPerTG = [](const Setup& s, const misc::QuadratureContext& context,
                     misc::QuadratureScratch& scratch, const VecFunc& f, VecBuffers& buf,
                     QuadKernel kernel) {
    std::vector<double> results;
    for (const TensorGridCTData* tgData : s.tgs) {
      processTGVec(context, tgData->tensorGrid, scratch, f, buf, kernel, buf.tgResult.data());
      results.insert(results.end(), buf.tgResult.begin(), buf.tgResult.end());
    }
    return results;
  };
  auto vecRun = [](const Setup& s, const misc::QuadratureContext& context,
                   misc::QuadratureScratch& scratch, const VecFunc& f, VecBuffers& buf,
                   QuadKernel kernel, std::vector<double>& sum) {
    const size_t m = f.nOutputs();
    std::fill(sum.begin(), sum.end(), 0.0);
    for (const TensorGridCTData* tgData : s.tgs) {
      processTGVec(context, tgData->tensorGrid, scratch, f, buf, kernel, buf.tgResult.data());
      for (size_t k = 0; k < m; k++) sum[k] += tgData->coefficient * buf.tgResult[k];
    }
  };

  // m = 1 with perf::expSum: scalar cost of each design
  suite.addMicro("micro/vec/quad/scalar/" + cfg + "/m1", [=](MicroRun& run) {
    const Setup s(nodes, boundary, nDim, maxLvl, allTGs);
    const SourceFunc sourceFunc(perf::expSum);
    const misc::QuadratureContext context = s.context();
    misc::QuadratureScratch scratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim());
    const ScalarFunc f(perf::expSum);
    // The copy of the kernel against the production kernel
    compareResults(run, "quad/scalar/" + cfg,
                   scalarPerTG(s, context, scratch,
                               [&](const misc::QuadratureContext& c, const TensorGrid& tg,
                                   misc::QuadratureScratch& sc) {
                                 return processTGScalar(c, tg, sc, f);
                               }),
                   scalarPerTG(s, context, scratch,
                               [&](const misc::QuadratureContext& c, const TensorGrid& tg,
                                   misc::QuadratureScratch& sc) {
                                 return quadrature_operator::processTensorGrid(c, sourceFunc, tg,
                                                                               sc);
                               }));
    run.setItems(s.nGP);
    run.measure([&]() { doNotOptimize(scalarRun(s, context, scratch, f)); });
  });

  // The scalar instantiation of the generic kernel (template on the value type)
  suite.addMicro("micro/vec/quad/tvt_scalar/" + cfg + "/m1", [=](MicroRun& run) {
    const Setup s(nodes, boundary, nDim, maxLvl, allTGs);
    const misc::QuadratureContext context = s.context();
    misc::QuadratureScratch scratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim());
    const ScalarFunc f(perf::expSum);
    VecBuffers buf(1);
    auto genericRun = [&]() {
      double sum = 0.0;
      for (const TensorGridCTData* tgData : s.tgs) {
        sum += tgData->coefficient *
               processTGGenericScalar(context, tgData->tensorGrid, scratch, f, buf);
      }
      return sum;
    };
    compareResults(run, "quad/tvt_scalar/" + cfg,
                   scalarPerTG(s, context, scratch,
                               [&](const misc::QuadratureContext& c, const TensorGrid& tg,
                                   misc::QuadratureScratch& sc) {
                                 return processTGGenericScalar(c, tg, sc, f, buf);
                               }),
                   scalarsPerTG(s, context, scratch, {f}));
    run.setItems(s.nGP);
    run.measure([&]() { doNotOptimize(genericRun()); });
  });

  const std::vector<std::pair<std::string, QuadKernel>> kernels = {
      {"rt", QuadKernel::RUNTIME},
      {"ct", QuadKernel::COMPILE_TIME},
      {"rowbuf", QuadKernel::ROW_BUFFERED},
      {"rowbufk", QuadKernel::ROW_BUFFERED_OUTPUTS_OUTER},
      {"batch", QuadKernel::BATCH}};

  for (const bool nested : {true, false}) {
    for (const auto& [kernelName, kernel] : kernels) {
      if (nested && kernel != QuadKernel::RUNTIME && kernel != QuadKernel::BATCH) continue;
      const std::string name = "micro/vec/quad/" + kernelName + (nested ? "_nested/" : "_direct/") +
                               cfg + "/m1";
      suite.addMicro(name, [=](MicroRun& run) {
        const Setup s(nodes, boundary, nDim, maxLvl, allTGs);
        const misc::QuadratureContext context = s.context();
        misc::QuadratureScratch scratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim());
        const VecFunc f = nested ? VecFunc::wrapNested(perf::expSum)
                                 : VecFunc::wrapDirect(perf::expSum);
        VecBuffers buf(1);
        std::vector<double> sum(1);
        compareResults(run, name, vecPerTG(s, context, scratch, f, buf, kernel),
                       scalarsPerTG(s, context, scratch, {ScalarFunc(perf::expSum)}));
        run.setItems(s.nGP);
        run.measure([&]() {
          vecRun(s, context, scratch, f, buf, kernel, sum);
          doNotOptimize(sum.data());
          clobberMemory();
        });
      });
    }
  }

  // Vector gain: m outputs (expSumK) in one run vs. m scalar runs
  for (const size_t m : ms) {
    suite.addMicro("micro/vec/quad/scalar_x_m/" + cfg + "/m" + std::to_string(m),
                   [=](MicroRun& run) {
                     const Setup s(nodes, boundary, nDim, maxLvl, allTGs);
                     const misc::QuadratureContext context = s.context();
                     misc::QuadratureScratch scratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim());
                     const std::vector<ScalarFunc> funcs = scalarFuncs(expSumK, m);
                     run.setItems(s.nGP * static_cast<double>(m));
                     run.measure([&]() {
                       for (const ScalarFunc& f : funcs) {
                         doNotOptimize(scalarRun(s, context, scratch, f));
                       }
                     });
                   });

    for (const auto& [kernelName, kernel] : kernels) {
      const std::string name =
          "micro/vec/quad/" + kernelName + "/" + cfg + "/m" + std::to_string(m);
      suite.addMicro(name, [=](MicroRun& run) {
        const Setup s(nodes, boundary, nDim, maxLvl, allTGs);
        const misc::QuadratureContext context = s.context();
        misc::QuadratureScratch scratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim());
        const VecFunc f = vecFunc(expSumK, m);
        VecBuffers buf(m);
        std::vector<double> sum(m);
        compareResults(run, name, vecPerTG(s, context, scratch, f, buf, kernel),
                       scalarsPerTG(s, context, scratch, scalarFuncs(expSumK, m)));
        run.setItems(s.nGP * static_cast<double>(m));
        run.measure([&]() {
          vecRun(s, context, scratch, f, buf, kernel, sum);
          doNotOptimize(sum.data());
          clobberMemory();
        });
      });
    }
  }
}

/**
 * @brief DSG quadrature cases micro/vec/dsg/<variant>/<cfg>/m<m> on the largest delta tensor
 * grid, ns per grid point: scalar (m = 1, perf::expSum), <kernel>_direct (m = 1), scalar_x_m,
 * <kernel> (expSumK) with the kernels rt (per point) and batch (per row chunk).
 */
inline void addDeltaCases(Suite& suite, const std::string& nodes, const LvlType boundary,
                          const size_t nDim, const LvlType maxLvl, const std::vector<size_t>& ms) {
  const std::string cfg = cfgName(nodes, boundary, nDim, maxLvl);

  struct Setup {
    Setup(const std::string& nodes, LvlType boundary, size_t nDim, LvlType maxLvl)
        : genInstr(maxLvl, nDim) {
      perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
      dsg.emplace(genInstr);
      weights1D.emplace(delta_quadrature_operator::comp1DWeights(*dsg));
      coeffTrie.emplace(dsg->getTensorGrids(), nDim);
      tgData = selectTGs(*dsg, false)[0];
      context.emplace(misc::DeltaQuadratureContext{.deltasSparseGrid = *dsg,
                                                   .sourceFunc = sourceFunc,
                                                   .domain = dsg->getGenInstr().getDomain(),
                                                   .weights1D = *weights1D,
                                                   .coeffTrie = *coeffTrie});
      scratch.emplace(nDim, dsg->getMaxTGSumOverGPCntsPerDim());
    }
    CompleteSGGenInstr genInstr;
    std::optional<DeltasSparseGrid> dsg;
    std::optional<misc::FlatNestedVec<double, 3>> weights1D;
    std::optional<misc::CTCoeffTrie> coeffTrie;
    const TensorGridCTData* tgData = nullptr;
    const SourceFunc sourceFunc{perf::expSum};
    std::optional<misc::DeltaQuadratureContext> context;
    std::optional<misc::DeltaQuadratureScratch> scratch;
  };

  suite.addMicro("micro/vec/dsg/scalar/" + cfg + "/m1", [=](MicroRun& run) {
    Setup s(nodes, boundary, nDim, maxLvl);
    const ScalarFunc f(perf::expSum);
    const double production =
        delta_quadrature_operator::processDeltaTensorGrid(*s.context, *s.tgData, *s.scratch);
    compareResults(run, "dsg/scalar/" + cfg,
                   {processDeltaTGScalar(*s.context, *s.tgData, *s.scratch, f)}, {production});
    run.setItems(static_cast<double>(s.tgData->tensorGrid.nGP()));
    run.measure(
        [&]() { doNotOptimize(processDeltaTGScalar(*s.context, *s.tgData, *s.scratch, f)); });
  });

  // Vector kernels: rt (per point), batch (per row chunk)
  using DeltaKernel = void (*)(const misc::DeltaQuadratureContext&, const TensorGridCTData&,
                               misc::DeltaQuadratureScratch&, const VecFunc&, VecBuffers&,
                               double*);
  const std::vector<std::pair<std::string, DeltaKernel>> kernels = {{"rt", processDeltaTGVec},
                                                                    {"batch", processDeltaTGBatch}};

  for (const auto& [kernelName, kernel] : kernels) {
    const std::string name = "micro/vec/dsg/" + kernelName + "_direct/" + cfg + "/m1";
    suite.addMicro(name, [=](MicroRun& run) {
      Setup s(nodes, boundary, nDim, maxLvl);
      const VecFunc f = VecFunc::wrapDirect(perf::expSum);
      VecBuffers buf(1);
      double result;
      kernel(*s.context, *s.tgData, *s.scratch, f, buf, &result);
      const ScalarFunc scalar(perf::expSum);
      compareResults(run, name, {result},
                     {processDeltaTGScalar(*s.context, *s.tgData, *s.scratch, scalar)});
      run.setItems(static_cast<double>(s.tgData->tensorGrid.nGP()));
      run.measure([&]() {
        kernel(*s.context, *s.tgData, *s.scratch, f, buf, &result);
        doNotOptimize(result);
      });
    });
  }

  for (const size_t m : ms) {
    suite.addMicro("micro/vec/dsg/scalar_x_m/" + cfg + "/m" + std::to_string(m),
                   [=](MicroRun& run) {
                     Setup s(nodes, boundary, nDim, maxLvl);
                     const std::vector<ScalarFunc> funcs = scalarFuncs(expSumK, m);
                     run.setItems(static_cast<double>(s.tgData->tensorGrid.nGP() * m));
                     run.measure([&]() {
                       for (const ScalarFunc& f : funcs) {
                         doNotOptimize(processDeltaTGScalar(*s.context, *s.tgData, *s.scratch, f));
                       }
                     });
                   });

    for (const auto& [kernelName, kernel] : kernels) {
      const std::string name =
          "micro/vec/dsg/" + kernelName + "/" + cfg + "/m" + std::to_string(m);
      suite.addMicro(name, [=](MicroRun& run) {
        Setup s(nodes, boundary, nDim, maxLvl);
        const VecFunc f = vecFunc(expSumK, m);
        VecBuffers buf(m);
        std::vector<double> result(m);
        std::vector<double> scalars;
        for (const ScalarFunc& fk : scalarFuncs(expSumK, m)) {
          scalars.push_back(processDeltaTGScalar(*s.context, *s.tgData, *s.scratch, fk));
        }
        kernel(*s.context, *s.tgData, *s.scratch, f, buf, result.data());
        compareResults(run, name, result, scalars);
        run.setItems(static_cast<double>(s.tgData->tensorGrid.nGP() * m));
        run.measure([&]() {
          kernel(*s.context, *s.tgData, *s.scratch, f, buf, result.data());
          doNotOptimize(result.data());
          clobberMemory();
        });
      });
    }
  }
}

/**
 * @brief Global interpolation cases micro/vec/interp/<variant>/<cfg>/m<m> on the largest tensor
 * grid (shifted domain, one query point), ns per grid point: scalar (m = 1, perf::cosProd),
 * <layout>_direct (m = 1), scalar_x_m, <layout> (cosProdK) with the layouts inner (m innermost,
 * interp1D), innerk (m innermost, interp1DK) and outer (m outermost, scalar formula per output).
 */
inline void addInterpCases(Suite& suite, const std::string& nodes, const size_t nDim,
                           const LvlType maxLvl, const std::vector<size_t>& ms) {
  const std::string cfg = cfgName(nodes, 0, nDim, maxLvl);

  struct Setup {
    Setup(const std::string& nodes, size_t nDim, LvlType maxLvl)
        : domain(perf::shiftedDomain(nDim)), genInstr(maxLvl, nDim) {
      perf::configure(genInstr, nodes, 0, domain);
      sg.emplace(genInstr);
      const DataVector point = perf::randomPoints(domain, 1, 99)[0];
      normPoint = tools::normalizeDataVector(point, domain);
      ctx.emplace(*selectTGs(*sg, false)[0], sg->getGenInstr(), nodes == "cc");
    }
    double nGP() const { return static_cast<double>(ctx->tgData.tensorGrid.nGP()); }
    HyperCubeArea domain;
    CompleteSGGenInstr genInstr;
    std::optional<SparseGrid> sg;
    DataVector normPoint;
    std::optional<InterpTG> ctx;
  };

  using VecKernel = void (*)(const VecFunc&, const DataVector&, const InterpTG&, double*);
  const std::vector<std::pair<std::string, VecKernel>> kernels = {
      {"inner", interpTGVecInner}, {"innerk", interpTGVecInnerK}, {"outer", interpTGVecOuter}};

  suite.addMicro("micro/vec/interp/scalar/" + cfg + "/m1", [=](MicroRun& run) {
    const Setup s(nodes, nDim, maxLvl);
    const ScalarFunc f(perf::cosProd);
    const double production = global_interpolation::interpolate(SourceFunc(perf::cosProd),
                                                                s.normPoint, s.ctx->tgData,
                                                                s.sg->getGenInstr());
    compareResults(run, "interp/scalar/" + cfg, {interpTGScalar(f, s.normPoint, *s.ctx)},
                   {production});
    run.setItems(s.nGP());
    run.measure([&]() { doNotOptimize(interpTGScalar(f, s.normPoint, *s.ctx)); });
  });

  for (const auto& [kernelName, kernel] : kernels) {
    const std::string name = "micro/vec/interp/" + kernelName + "_direct/" + cfg + "/m1";
    suite.addMicro(name, [=](MicroRun& run) {
      const Setup s(nodes, nDim, maxLvl);
      const VecFunc f = VecFunc::wrapDirect(perf::cosProd);
      double result;
      kernel(f, s.normPoint, *s.ctx, &result);
      const ScalarFunc scalar(perf::cosProd);
      compareResults(run, name, {result}, {interpTGScalar(scalar, s.normPoint, *s.ctx)});
      run.setItems(s.nGP());
      run.measure([&]() {
        kernel(f, s.normPoint, *s.ctx, &result);
        doNotOptimize(result);
      });
    });
  }

  for (const size_t m : ms) {
    suite.addMicro("micro/vec/interp/scalar_x_m/" + cfg + "/m" + std::to_string(m),
                   [=](MicroRun& run) {
                     const Setup s(nodes, nDim, maxLvl);
                     const std::vector<ScalarFunc> funcs = scalarFuncs(cosProdK, m);
                     run.setItems(s.nGP() * static_cast<double>(m));
                     run.measure([&]() {
                       for (const ScalarFunc& f : funcs) {
                         doNotOptimize(interpTGScalar(f, s.normPoint, *s.ctx));
                       }
                     });
                   });

    for (const auto& [kernelName, kernel] : kernels) {
      const std::string name =
          "micro/vec/interp/" + kernelName + "/" + cfg + "/m" + std::to_string(m);
      suite.addMicro(name, [=](MicroRun& run) {
        const Setup s(nodes, nDim, maxLvl);
        const VecFunc f = vecFunc(cosProdK, m);
        std::vector<double> result(m);
        std::vector<double> scalars;
        for (const ScalarFunc& fk : scalarFuncs(cosProdK, m)) {
          scalars.push_back(interpTGScalar(fk, s.normPoint, *s.ctx));
        }
        kernel(f, s.normPoint, *s.ctx, result.data());
        compareResults(run, name, result, scalars);
        run.setItems(s.nGP() * static_cast<double>(m));
        run.measure([&]() {
          kernel(f, s.normPoint, *s.ctx, result.data());
          doNotOptimize(result.data());
          clobberMemory();
        });
      });
    }
  }
}

/// @brief Registers all WP-24 cases (micro/vec/...).
inline void addVectorProtoCases(Suite& suite) {
  const std::vector<size_t> ms = {1, 2, 4, 8, 32};
  const std::vector<size_t> none = {};

  // SG quadrature: the micro/quad/process_tg configurations (scalar cost) and low / high d
  // with the m sweep (b1 d20: all tensor grids, as micro/quad/process_all_tgs)
  addQuadCases(suite, "equi", 0, 2, 16, false, ms);
  addQuadCases(suite, "equi", 0, 5, 10, false, none);
  addQuadCases(suite, "equi", 0, 10, 4, false, ms);
  addQuadCases(suite, "cc", 0, 10, 4, false, none);
  addQuadCases(suite, "equi", 1, 20, 4, true, ms);

  // DSG quadrature: the micro/dsg/process_delta_tg configurations
  addDeltaCases(suite, "equi", 0, 10, 7, ms);
  addDeltaCases(suite, "equi", 1, 20, 6, ms);

  // Global interpolation: the micro/interp/global_tg configurations and low / high d
  addInterpCases(suite, "cc", 2, 12, ms);
  addInterpCases(suite, "cc", 5, 6, ms);
  addInterpCases(suite, "cheb1", 5, 4, ms);
  addInterpCases(suite, "cc", 10, 2, ms);
}

}  // namespace vec_proto
}  // namespace perf
}  // namespace combigrid
}  // namespace sgpp
