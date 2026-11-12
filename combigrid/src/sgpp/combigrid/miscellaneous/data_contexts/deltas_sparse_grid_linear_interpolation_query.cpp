// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <memory>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_query.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

DSGLinInterpQuery::DSGLinInterpQuery(const DSGLinInterpIndex& index, const size_t nOutputs)
    : cornerIds(2 * index.nRows()),
      factors(2 * index.nRows()),
      coords(2 * index.nRows()),
      marks(index.nSlots(), 0),
      values(std::make_unique_for_overwrite<double[]>(index.nSlots() * nOutputs)) {}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
