// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_slot_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation_tools.hpp>
#include <span>
#include <stdexcept>

using sgpp::combigrid::misc::DSGLinInterpContext;
using sgpp::combigrid::misc::DSGLinInterpIndex;
using sgpp::combigrid::misc::DSGLinInterpScratch;
using sgpp::combigrid::misc::DSGLinInterpSlotScratch;

namespace sgpp {
namespace combigrid {

namespace {

/**
 * Interpolates at @p point and adds the m interpolated values to @p result (zero on entry):
 * prepares the context, runs the transforms of all dimensions and the evaluation (in parallel
 * unless the call is small: in a team of one thread, the OpenMP runtime still makes a system call
 * at every barrier) and adds the chunk sums in order. @p makeValues(scratch) returns the value
 * type of a thread; @p vectorValued: whether it is @c misc::VectorValues (buffer in the scratch).
 */
template <typename MakeValues>
void interpolateLinearDSG(const base::DataVector& point, const DeltasSparseGrid& deltasSparseGrid,
                          const std::span<double> result, const bool vectorValued,
                          MakeValues&& makeValues) {
  namespace dsg = tools::dsg_lin_interpolation;
  const DSGLinInterpIndex& index = dsg::getIndex(deltasSparseGrid);
  const HyperCubeArea& domain = deltasSparseGrid.getGenInstr().getDomain();
  const size_t nOutputs = result.size();
  DSGLinInterpContext context(index);
  dsg::prepareContext(index, tools::normalizeDataVector(point, domain), domain, context);
  DSGLinInterpSlotScratch slots(index, nOutputs);

  // The passes of a thread; the barriers of the transforms separate them
  const auto passes = [&]() {
    DSGLinInterpScratch scratch(index.nDim(), vectorValued ? nOutputs : 0);
    for (size_t dim = 0; dim < index.nDim(); dim++) {
      dsg::transformDimension(index, context, slots, dim);
    }
    dsg::evaluateChunks(makeValues(scratch), index, context, slots, scratch);
  };

  if (dsg::runInParallel(index)) {
#pragma omp parallel
    passes();
  } else {
    passes();
  }
  dsg::sumChunks(index, slots, result);
}

}  // namespace

double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const DeltasSparseGrid& deltasSparseGrid) {
  assert(point.size() == deltasSparseGrid.nDim());
  double result = 0.0;

  if (deltasSparseGrid.nDim() > 0) {
    interpolateLinearDSG(point, deltasSparseGrid, std::span<double>(&result, 1), false,
                         [&](DSGLinInterpScratch&) { return misc::ScalarValues(sourceFunc); });
  }

  sourceFunc.finishOperatorCall();

  return result;
}

void interpolateLinear(const VectorSourceFunc& sourceFunc, const base::DataVector& point,
                       const DeltasSparseGrid& deltasSparseGrid, const std::span<double> result) {
  if (result.size() != sourceFunc.nOutputs()) {
    throw std::invalid_argument(
        "interpolateLinear: The size of the result is not the number of outputs");
  }
  assert(point.size() == deltasSparseGrid.nDim());
  std::ranges::fill(result, 0.0);

  if (deltasSparseGrid.nDim() > 0) {
    interpolateLinearDSG(point, deltasSparseGrid, result, true, [&](DSGLinInterpScratch& scratch) {
      return misc::VectorValues(sourceFunc, scratch.valueBuffer);
    });
  }

  sourceFunc.finishOperatorCall();
}

base::DataVector interpolateLinear(const VectorSourceFunc& sourceFunc,
                                   const base::DataVector& point,
                                   const DeltasSparseGrid& deltasSparseGrid) {
  base::DataVector result(sourceFunc.nOutputs());
  interpolateLinear(sourceFunc, point, deltasSparseGrid, result);
  return result;
}

}  // namespace combigrid
}  // namespace sgpp
