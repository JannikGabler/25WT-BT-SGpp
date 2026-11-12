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
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_query.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/deltas_sparse_grid_linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/scalar_values.hpp>
#include <sgpp/combigrid/miscellaneous/operators/value_types/vector_values.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation_tools.hpp>
#include <span>
#include <stdexcept>

using sgpp::combigrid::misc::DSGLinInterpIndex;
using sgpp::combigrid::misc::DSGLinInterpQuery;
using sgpp::combigrid::misc::DSGLinInterpScratch;
using sgpp::combigrid::tools::dsg_lin_interpolation::evaluateMarked;
using sgpp::combigrid::tools::dsg_lin_interpolation::gatherCorners;
using sgpp::combigrid::tools::dsg_lin_interpolation::getIndex;
using sgpp::combigrid::tools::dsg_lin_interpolation::markCorners;
using sgpp::combigrid::tools::dsg_lin_interpolation::prepareQuery;
using sgpp::combigrid::tools::dsg_lin_interpolation::runInParallel;

namespace sgpp {
namespace combigrid {

double interpolateLinear(const SourceFunc& sourceFunc, const base::DataVector& point,
                         const DeltasSparseGrid& deltasSparseGrid) {
  assert(point.size() == deltasSparseGrid.nDim());
  double result = 0.0;

  if (deltasSparseGrid.nDim() > 0) {
    const DSGLinInterpIndex& index = getIndex(deltasSparseGrid);
    const HyperCubeArea& domain = deltasSparseGrid.getGenInstr().getDomain();
    DSGLinInterpQuery query(index, 1);
    prepareQuery(index, tools::normalizeDataVector(point, domain), domain, query);

    // The three passes of a thread; the barriers of the first two separate them
    const auto passes = [&]() {
      DSGLinInterpScratch scratch(index.nDim());
      const misc::ScalarValues values(sourceFunc);
      markCorners(index, query, scratch);
      evaluateMarked(values, index, query, scratch);
      return gatherCorners(values, index, query, scratch);
    };

    // Small problems run outside of a parallel region: in a team of one thread, the OpenMP
    // runtime still makes a system call at every barrier
    if (runInParallel(index)) {
#pragma omp parallel reduction(+ : result)
      result += passes();
    } else {
      result = passes();
    }
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
  const size_t nOutputs = result.size();
  std::ranges::fill(result, 0.0);

  if (deltasSparseGrid.nDim() > 0) {
    const DSGLinInterpIndex& index = getIndex(deltasSparseGrid);
    const HyperCubeArea& domain = deltasSparseGrid.getGenInstr().getDomain();
    DSGLinInterpQuery query(index, nOutputs);
    prepareQuery(index, tools::normalizeDataVector(point, domain), domain, query);

    const auto passes = [&]() {
      DSGLinInterpScratch scratch(index.nDim(), nOutputs);
      const misc::VectorValues values(sourceFunc, scratch.valueBuffer);
      markCorners(index, query, scratch);
      evaluateMarked(values, index, query, scratch);
      const double* const threadResult = gatherCorners(values, index, query, scratch);

#pragma omp critical
      for (size_t k = 0; k < nOutputs; k++) {
        result[k] += threadResult[k];
      }
    };

    // As in the scalar overload
    if (runInParallel(index)) {
#pragma omp parallel
      passes();
    } else {
      passes();
    }
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
